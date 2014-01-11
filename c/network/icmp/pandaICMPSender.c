/**
 ** \file   pandaICMPSender.c
 ** \brief  Simple standalone pinger
 ** \author Panda
 ** \date   2014-01-09
 **
 ** This sample aims create the simpliest standalone pinger.
 **
 ** \see http://ftp.fr.openbsd.org/pub/OpenBSD/src/sbin/ping/
 ** \see http://www.pdbuchan.com/rawsock/icmp4.c
 */

/**
 ** Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           /* close() */
#include <string.h>           /* strcpy, memset(), and memcpy() */
#include <errno.h>            /* errno, perror() */
#include <err.h>              /* err() */

#include <netdb.h>            /* struct addrinfo */
#include <sys/types.h>        /* needed for socket(), */
                              /*  uint8_t, uint16_t, uint32_t */
#include <sys/socket.h>       /* needed for socket() */
#include <sys/ioctl.h>        /* macro ioctl is defined */
#include <bits/ioctls.h>      /* values for argument "request" of ioctl. */
#include <netinet/in.h>       /* IPPROTO_RAW, IPPROTO_IP, */
                              /*  IPPROTO_ICMP, INET_ADDRSTRLEN */
#include <netinet/ip.h>       /* struct ip and IP_MAXPACKET (which is 65535) */
#include <netinet/ip_icmp.h>  /* struct icmp, ICMP_ECHO */
#include <net/if.h>           /* struct ifreq */
#include <arpa/inet.h>        /* inet_pton() and inet_ntop() */

/**
 ** Defines
 */
#define DEBUG         1
#define MAX_SEND_SIZE 500

/**
 ** Types
 */
typedef struct sockaddr_in sockaddr_in;
typedef struct in_addr     in_addr;

typedef struct
{
  char*    iface;
  char*    srcAddr;
  char*    dstAddr;
  char*    data;
} options;

/**
 ** Prototypes
 */

/**
 ** Checksum routine for Internet Protocol family headers (C Version)
 **
 ** \param  addr        Address of the packet to checksum
 ** \param  len         The length of the packet pointed by @addr
 **
 ** \return Computed checksum
 */
int        inCksum(u_short       *addr,
                   int           len);
/**
 ** Function used to control malloc execution
 **
 ** \param  size        The size of the memory to allocate
 **
 ** \return An untyped pointer to the allocated memory
 */
void*      securedMalloc(int     size);
/**
 ** Used to display the program usage
 */
void       usage();
/**
 ** Parse the command line options
 **
 ** \param  argc        Number of arguments
 ** \param  argv        Table of arguments
 **
 ** \return A structure containing computed options
 */
options*   optionsParse(int      argc,
                        char**   argv);
/**
 ** Print the given binary ip address in a human readable way
 **
 ** \param  ip          Table of arguments
 */
void       printIp(in_addr*      ip);
/**
 ** Generate an icmp echo packet containing the supplied data
 **
 ** \param  data        Array of data to send
 ** \param  dataLen     Length of the data to send
 ** \param  len         Pointer used to return the length of the packet
 **
 ** \return The content of the generated icmp packet
 */
u_char*    icmpPkt(u_char*       data,
                   size_t        dataLen,
                   int*          len);
/**
 ** Generate an ip l2 packet containing the supplied l3 packet
 **
 ** \param  l3protocol  IP id of the inner protocol
 ** \param  l3packet    Content of the inner packet
 ** \param  l3packetLen Length of the inner packet
 ** \param  dstAddr     Destination address
 ** \param  srcAddr     Source address (can be NULL)
 ** \param  len         Pointer used to return the length of the packet
 **
 ** \return The content of the generated ip packet
 */
u_char*    ipPkt(u_int8_t        l3protocol,
                 u_char*         l3packet,
                 size_t          l3packetLen,
                 in_addr*        dstAddr,
                 in_addr*        srcAddr,
                 int*            len);

/**
 ** Bind the given socket descriptor to the given interface.
 ** Do nothing if iface
 **
 ** \param  sd          Socket descriptor
 ** \param  iface       String identifying the physical interface
 **
 ** \return 0 for errors.
 */
int        setIface(int          sd,
                    char*        iface);
/**
 ** Resolv the destination host
 **
 ** \param  addr        String identifying the destination host
 ** \param  resolved    Pointer used to return the resolved address
 **
 ** \return 0 for errors.
 */
int        resolv(char*          addr,
                  in_addr*       resolved);
/**
 ** Set the max send size of a socket
 **
 ** \param  sd          Socket descriptor
 ** \param  size        Wanted size
 */
void       setMaxSendSize(int    sd,
                          int    size);
/**
 ** Resolv the destination host
 **
 ** \param  dstAddr     String identifying the destination host
 ** \param  iface       Pointer used to return the resolved address
 ** \param  dstAddr     String identifying the source address
 ** \param  data        String containing the icmp message
 ** \param  dataLen     Length of the icmp message
 **
 ** \return 0 for errors.
 */
void       sendICMPPacket(char*  dstAddr,
                          char*  iface,
                          char*  srcAddr,
                          char*  data,
                          size_t dataLen);


/**
 ** Implementation
 */
int        inCksum(u_short       *addr,
                   int           len)
{
  register int                   nleft  = len;
  register u_short               *w     = addr;
  register int                   sum    = 0;
  u_short                        answer = 0;

  /*
   * Our algorithm is simple, using a 32 bit accumulator (sum), we add
   * sequential 16 bit words to it, and at the end, fold back all the
   * carry bits from the top 16 bits into the lower 16 bits.
   */
  while (nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }

  /* mop up an odd byte, if necessary */
  if (nleft == 1)
  {
    *(u_char *)(&answer) = *(u_char *)w ;
    sum += answer;
  }

  /* add back carry outs from top 16 bits to low 16 bits */
  sum = (sum >> 16) + (sum & 0xffff);  /* add hi 16 to low 16 */
  sum += (sum >> 16);                  /* add carry */
  answer = ~sum;                       /* truncate to 16 bits */
  return(answer);
}

void*      securedMalloc(int     size)
{
  void*                          tmp;

  if (size <= 0)
    errx(EXIT_FAILURE, "ERROR: Cannot allocate memory of len = %i\n", size);

  if (!!(tmp = malloc(size)))
  {
    memset(tmp, 0, size);
    return tmp;
  }

  errx(EXIT_FAILURE, "ERROR: Cannot allocate memory");
}

void       usage()
{
  printf("Usage:\n"
         "  pandaICMPSample [OPTIONS] -d <host>\n"
         "\nOptions:\n"
         "  -i <iface> Physical interface to use (ex: -i eth0)\n"
         "  -s <addr>  Source address (ex: -s 192.168.0.2)\n"
         "  -m <data>  Messag to send (ex: -m 1337.42)\n"
         "\nSynthax:\n"
         "  <host> : IP address or DNS name\n"
              "  <addr> : IP address\n"
         "  <data> : String\n");
  exit(EXIT_FAILURE);
}


options*   optionsParse(int      argc,
                        char**   argv)
{
  int                            i;
  options*                       options = securedMalloc(sizeof(options));

  for (i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 'd':
        if (i++ < argc)
          options->dstAddr = argv[i];
        else
          usage();
        break;
      case 's':
        if (i++ < argc)
          options->srcAddr = argv[i];
        else
          usage();
        break;
      case 'i':
        if (i++ < argc)
          options->iface = argv[i];
        else
          usage();
        break;
      case 'm':
        if (i++ < argc)
          options->data = argv[i];
        else
          usage();
        break;
      default:
        usage();
        break;
      }
    }
    else
    {
      /* Nothing here yet */
      usage();
    }
  }

  /* Check if mandatory option has been submitted */
  if (!options->dstAddr)
    usage();

#if DEBUG
  printf("Starting with options:\n"
     " Destination: %s\n"
     " Interface:   %s\n"
     " Source:      %s\n",
     options->dstAddr,
     options->iface,
     options->srcAddr);
#endif
  return options;
}

void       printIp(in_addr*      ip)
{
  char                           buf[INET_ADDRSTRLEN + 1];

  if (inet_ntop(AF_INET, ip, buf, INET_ADDRSTRLEN) == NULL)
  {
    perror("inet_ntop() failed");
    return;
  }

  buf[INET_ADDRSTRLEN] = 0;
  printf("Ip : %s\n", buf);
}

u_char*    icmpPkt(u_char*       data,
                   size_t        dataLen,
                   int*          len)
{
  struct icmp*                   icmp;
  u_char*                        outpack;

  *len             = sizeof(struct icmphdr) + dataLen;
  *len            += ((dataLen % 4) == 0) ? 0 : 4 - (dataLen % 4);
  icmp             = securedMalloc(*len);

  icmp->icmp_type  = ICMP_ECHO;
  icmp->icmp_code  = 0;
  icmp->icmp_cksum = 0;
  icmp->icmp_seq   = htons(0);
  icmp->icmp_id    = htons(0x4242);

  outpack          = ((u_char*) icmp) + sizeof(struct icmphdr);
  memcpy(outpack, data, dataLen);

  icmp->icmp_cksum = inCksum((u_short*) icmp, *len);

  return (u_char*) icmp;
}

u_char*    ipPkt(u_int8_t        l3protocol,
                 u_char*         l3packet,
                 size_t          l3packetLen,
                 in_addr*        dstAddr,
                 in_addr*        srcAddr,
                 int*            len)
{
  struct ip*                     iphdr;
  u_char*                        outpack;

  *len          = sizeof(struct ip) + l3packetLen;
  iphdr         = securedMalloc(*len);              /* 65535 */
  outpack       = (u_char*) iphdr + sizeof(struct ip);

  iphdr->ip_v   = 4;                                /* version */
  iphdr->ip_hl  = sizeof(struct ip) >> 2;           /* header length */
  iphdr->ip_tos = 0;                                /* type of service */
  iphdr->ip_len = sizeof(struct ip) + l3packetLen;  /* total length */
  iphdr->ip_id  = 0x4242;                           /* identification */
  iphdr->ip_off = 0;                                /* fragment offset field */
  iphdr->ip_ttl = 120;                              /* time to live */
  iphdr->ip_p   = l3protocol;                       /* protocol */
  iphdr->ip_sum = 0;                                /* checksum before calc */

  iphdr->ip_src = *srcAddr;                         /* src address */
  iphdr->ip_dst = *dstAddr;                         /* dst address */

  iphdr->ip_sum = inCksum((u_short*) iphdr, sizeof(struct ip));

  memcpy(outpack, l3packet, l3packetLen);

  return (u_char*) iphdr;
}


int        setIface(int          sd,
                    char*        iface)
{
  struct ifreq                   ifr;

  if (!iface)
    return 0;

  /*
   * Use ioctl() to look up interface index which we will use to
   * bind socket descriptor sd to specified interface with setsockopt() since
   * none of the other arguments of sendto() specify which interface to use.
   */
  memset(&ifr, 0, sizeof(struct ifreq));
  strncpy(ifr.ifr_name, iface, IFNAMSIZ);
  if (ioctl(sd, SIOCGIFINDEX, &ifr) < 0)
  {
    perror("ioctl() failed to find interface");
    return 0;
  }
#if DEBUG
  printf("Index for interface %s is %i\n", iface, ifr.ifr_ifindex);
#endif

  /* Bind socket to interface index. */
  if (setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(struct ifreq)) < 0)
  {
    perror ("setsockopt() failed to bind to interface ");
    return 0;
  }

#if DEBUG
  printf("Socket binded to interface %s\n", iface);
#endif

  return 1;
}


int        resolv(char*          addr,
                  in_addr*       resolved)
{
  struct addrinfo                hints;
  struct addrinfo*               result;
  struct addrinfo*               rp;
  sockaddr_in*                   tmp;
  int                            status;
  char                           dstIp[INET_ADDRSTRLEN];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_INET;      /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM;   /* Datagram socket */
  hints.ai_flags    = 0;            /* No flags */
  hints.ai_protocol = 0;            /* Any protocol */

  if (0 != (status = getaddrinfo(addr, NULL, &hints, &result)))
  {
    fprintf(stderr, "Error: getaddrinfo: %s\n", gai_strerror(status));
    return 0;
  }

  if (!result)
  {
    fprintf(stderr, "Error: getaddrinfo could not resolv %s\n", addr);
    return 0;
  }

  tmp = (sockaddr_in*) result->ai_addr;
  *resolved = tmp->sin_addr;

#if DEBUG
  printIp(resolved);
#endif

  /*
   * getaddrinfo() returns a list of address structures.
   *  Display each address as warning, and keep the first
   */
  if (NULL != result->ai_next)
  {
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      tmp = (sockaddr_in*) rp->ai_addr;
      if (inet_ntop(AF_INET, &(tmp->sin_addr), dstIp, INET_ADDRSTRLEN) == NULL)
        perror("inet_ntop failed.");

      dstIp[INET_ADDRSTRLEN - 1] = 0;
      fprintf(stderr, "Warning: getaddrinfo multiple results for %s : %s\n",
              addr,
              dstIp);
    }
  }

  /*BUG:freeaddrinfo(result);*//* No longer needed */
  return 1;
}

void       setMaxSendSize(int    sd,
                          int    size)
{
  int                            maxSize;
  socklen_t                      len;

  if (getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &maxSize, &len) < 0)
    err(1, "getsockopt, unable to get the max send size");
  if ((maxSize < size)
      && setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(maxSize)) < 0)
    err(1, "setsockopt, unable to set the max send size");
}

void       sendICMPPacket(char*  dstAddr,
                          char*  iface,
                          char*  srcAddr,
                          char*  data,
                          size_t dataLen)
{
  int                            status;
  in_addr                        srcIP;
  in_addr                        dstIP;
  sockaddr_in                    whereto;
  int                            sd;
  u_char*                        ipPacket;
  int                            ipPacketLen;
  u_char*                        icmpPacket;
  int                            icmpPacketLen;
  int                            on = 1;

  /* truncate data */
  if (dataLen > (MAX_SEND_SIZE - sizeof(struct icmphdr) - sizeof(struct ip)))
    dataLen = MAX_SEND_SIZE;

  /* convert source addr */
  if ((!!srcAddr) && ((status = inet_pton (AF_INET, srcAddr, &srcIP) != 1)))
    fprintf(stderr, "inet_pton() failed.\nError message: %s",
            strerror (status));

  if ((!srcAddr) || (status != 1))
  {
    memset(&srcIP, 0, sizeof(in_addr));
    srcIP.s_addr = INADDR_ANY;
  }

  /* resolv destination */
  if (!resolv(dstAddr, &dstIP))
    exit(EXIT_FAILURE);

  /* icmp packet */
  icmpPacket = icmpPkt((u_char*) data, dataLen, &icmpPacketLen);

  /* ip packet */
  ipPacket = ipPkt(IPPROTO_ICMP, icmpPacket, icmpPacketLen,
                   &dstIP, &srcIP, &ipPacketLen);
  free(icmpPacket);

  /* Submit request for a socket descriptor */
  if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
  {
    free(ipPacket);
    err(EXIT_FAILURE, "socket() failed to get socket descriptor");
  }

  /* Set flag so socket expects us to provide IPv4 header. */
  if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0)
  {
    close(sd);
    free(ipPacket);
    err(EXIT_FAILURE, "setsockopt() failed to set IP_HDRINCL");
  }

  setIface(sd, iface);

  setMaxSendSize(sd, MAX_SEND_SIZE);

  /* Send packet. */
  memset(&whereto, 0, sizeof(sockaddr_in));
  whereto.sin_family = AF_INET;
  whereto.sin_addr   = dstIP;

  if (sendto(sd, ipPacket, ipPacketLen, 0,
             (struct sockaddr*) &whereto, sizeof(sockaddr_in)) < 0)
  {
    free(ipPacket);
    close(sd);
    err(EXIT_FAILURE, "sendto() failed");
  }

  free(ipPacket);
  close(sd);
}

/**
 ** Entry point of the program
 **
 ** \param  argc    Number of arguments
 ** \param  argv    Table of arguments
 **
 ** \return The exit value of the program
 */
int        main(int              argc,
                char**           argv)
{
  options* options;

  options = optionsParse(argc, argv);

  sendICMPPacket(options->dstAddr,
                 options->iface,
                 options->srcAddr,
                 options->data,
                 (!options->data) ? 0 : strlen(options->data));

  free(options);
  return EXIT_SUCCESS;
}
