/**
 ** \file   pandaICMPListener.c
 ** \brief  Listen for icmp packets and display their messages
 ** \author Panda
 ** \date   2014-01-09
 **
 ** This sample aims at displaying icmp messages
 **
 ** \see http://ftp.fr.openbsd.org/pub/OpenBSD/src/sbin/ping/
 ** 
 */

/**
 ** Includes
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

/**
 ** Types
 */
typedef struct sockaddr_in saddr_in;
typedef struct in_addr     in_addr;
typedef struct icmp        icmp;
typedef struct ip          ip;

/**
 ** Prototypes
 */
/**
 ** Set the biggest possible size for the socket receiving buffer
 ** Exit the program if the buffer is less than 1024
 **
 ** \param  sd          Socket descriptor
 ** \param  bufspace    The excpected buffer size
 **
 ** \return The allocated size.
 */
int        setSockRecvBuf(int  sd,
                          int  bufspace);
/**
 ** Display the options of the IP header
 **
 ** \param  ip          Pointer to the IP header
 */
void       prIPhdr(ip*         ip);
/**
 ** Display the options of the returned IP header
 **
 ** \param  ip          Pointer to the returned IP header
 */
void       prICMPretip(ip*     ip);
/**
 ** Display the options of the ICMP header
 **
 ** \param  icmp        Pointer to the ICMP header
 */
void       prICMPhdr(icmp*     icmp);
/**
 ** Print the given binary ip address in a human readable way
 **
 ** \param  ip          Table of arguments
 */
void       printIp(in_addr*    ip);
/**
 ** Analyze the content of an ICMP/IP packet and display the data part
 **
 ** \param  packet      Pointer to the received packet
 ** \param  cc          Size of the received packet
 ** \param  from        Pointer to the foreign IP structure
 ** \param  fromLen     Size of the foreign IP structure
 */
void       anPktICMP(char*     packet,
                     int       cc,
                     saddr_in* from,
                     socklen_t fromLen);
/**
 ** Listen for ICMP/IP packet and display their data
 */
void       icmpReceiveLoop();


/**
 ** Implementation
 */
int        setSockRecvBuf(int  sd,
                          int  bufspace)
{
  while (setsockopt(sd, SOL_SOCKET, SO_RCVBUF,
                    &bufspace, sizeof(bufspace)) < 0)
  {
    if ((bufspace -= 1024) <= 0)
      err(1, "Cannot set the receive buffer size");
  }

  if (bufspace < IP_MAXPACKET)
  {
    warnx("Could only allocate a receive buffer of %d bytes (default %d)",
          bufspace, IP_MAXPACKET);
    return 0;
  }

  return bufspace;
}

void       prIPhdr(ip*         ip)
{
  int                          hlen;
  u_char*                      cp;

  hlen = ip->ip_hl << 2;
  cp   = (u_char*) ip + 20;                /* point to options */

  printf("  Vr HL TOS  Len   ID Flg  off TTL Pro  cks\tSrc\t\tDst\tData\n");
  printf("   %1x  %1x  %02x %04x %04x",
               ip->ip_v, ip->ip_hl, ip->ip_tos, ip->ip_len, ip->ip_id);
  printf("   %1x %04x", ((ip->ip_off) & 0xe000) >> 13,
               (ip->ip_off) & 0x1fff);
  printf("  %02x  %02x %04x", ip->ip_ttl, ip->ip_p, ip->ip_sum);
  printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->ip_src.s_addr));
  printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->ip_dst.s_addr));
  
  /* dump and option bytes */
  while (hlen-- > 20)
  {
    printf("%02x", *cp++);
  }
  putchar('\n');
}

void       prICMPretip(ip*     ip)
{
  int                          hlen;
  u_char*                      cp;

  prIPhdr(ip);
  hlen = ip->ip_hl << 2;
  cp   = (u_char*) ip + hlen;

  if (ip->ip_p == 6)
    printf("TCP: from port %u, to port %u (decimal)\n",
                 (*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
  else if (ip->ip_p == 17)
    printf("UDP: from port %u, to port %u (decimal)\n",
                 (*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
}

void       prICMPhdr(icmp*     icmp)
{
  switch(icmp->icmp_type)
  {
  case ICMP_ECHOREPLY:
    printf("Echo Reply\n");
    break;
  case ICMP_UNREACH:
    switch(icmp->icmp_code)
    {
    case ICMP_UNREACH_NET:
      printf("Destination Net Unreachable\n");
      break;
    case ICMP_UNREACH_HOST:
      printf("Destination Host Unreachable\n");
      break;
    case ICMP_UNREACH_PROTOCOL:
      printf("Destination Protocol Unreachable\n");
      break;
    case ICMP_UNREACH_PORT:
      printf("Destination Port Unreachable\n");
      break;
    case ICMP_UNREACH_NEEDFRAG:
      if (icmp->icmp_nextmtu != 0)
        printf("frag needed and DF set (MTU %d)\n",
                     ntohs(icmp->icmp_nextmtu));
      else
        printf("frag needed and DF set\n");
      break;
    case ICMP_UNREACH_SRCFAIL:
      printf("Source Route Failed\n");
      break;
    case ICMP_UNREACH_NET_UNKNOWN:
      printf("Network Unknown\n");
      break;
    case ICMP_UNREACH_HOST_UNKNOWN:
      printf("Host Unknown\n");
      break;
    case ICMP_UNREACH_ISOLATED:
      printf("Source Isolated\n");
      break;
    case ICMP_UNREACH_NET_PROHIB:
      printf("Dest. Net Administratively Prohibited\n");
      break;
    case ICMP_UNREACH_HOST_PROHIB:
      printf("Dest. Host Administratively Prohibited\n");
      break;
    case ICMP_UNREACH_TOSNET:
      printf("Destination Net Unreachable for TOS\n");
      break;
    case ICMP_UNREACH_TOSHOST:
      printf("Destination Host Unreachable for TOS\n");
      break;
    case ICMP_UNREACH_FILTER_PROHIB:
      printf("Route administratively prohibited\n");
      break;
    case ICMP_UNREACH_HOST_PRECEDENCE:
      printf("Host Precedence Violation\n");
      break;
    case ICMP_UNREACH_PRECEDENCE_CUTOFF:
      printf("Precedence Cutoff\n");
      break;
    default:
      printf("Dest Unreachable, Unknown Code: %d\n",
                   icmp->icmp_code);
      break;
    }
    /* Print returned IP header information */
#ifndef icmp_data
    prICMPretip(&icmp->icmp_ip);
#else
    prICMPretip((ip*) icmp->icmp_data);
#endif
    break;
  case ICMP_SOURCEQUENCH:
    printf("Source Quench\n");
#ifndef icmp_data
    prICMPretip(&icmp->icmp_ip);
#else
    prICMPretip((ip*) icmp->icmp_data);
#endif
    break;
  case ICMP_REDIRECT:
    switch(icmp->icmp_code)
    {
    case ICMP_REDIRECT_NET:
      printf("Redirect Network");
      break;
    case ICMP_REDIRECT_HOST:
      printf("Redirect Host");
      break;
    case ICMP_REDIRECT_TOSNET:
      printf("Redirect Type of Service and Network");
      break;
    case ICMP_REDIRECT_TOSHOST:
      printf("Redirect Type of Service and Host");
      break;
    default:
      printf("Redirect, Unknown Code: %d", icmp->icmp_code);
      break;
    }
    printf("(New addr: %s)\n",
                 inet_ntoa(icmp->icmp_gwaddr));
#ifndef icmp_data
    prICMPretip(&icmp->icmp_ip);
#else
    prICMPretip((ip*) icmp->icmp_data);
#endif
    break;
  case ICMP_ECHO:
    printf("Echo Request\n");
    break;
  case ICMP_ROUTERADVERT:
    /* RFC1256 */
    printf("Router Discovery Advertisement\n");
    printf("(%d entries, lifetime %d seconds)\n",
                 icmp->icmp_num_addrs, ntohs(icmp->icmp_lifetime));
    break;
  case ICMP_ROUTERSOLICIT:
    /* RFC1256 */
    printf("Router Discovery Solicitation\n");
    break;
  case ICMP_TIMXCEED:
    switch(icmp->icmp_code)
    {
    case ICMP_TIMXCEED_INTRANS:
      printf("Time to live exceeded\n");
      break;
    case ICMP_TIMXCEED_REASS:
      printf("Frag reassembly time exceeded\n");
      break;
    default:
      printf("Time exceeded, Unknown Code: %d\n",
                   icmp->icmp_code);
      break;
    }
#ifndef icmp_data
    prICMPretip(&icmp->icmp_ip);
#else
    prICMPretip((ip*) icmp->icmp_data);
#endif
    break;
  case ICMP_PARAMPROB:
    switch(icmp->icmp_code)
    {
    case ICMP_PARAMPROB_OPTABSENT:
      printf("Parameter problem, required option "
                   "absent: pointer = 0x%02x\n",
                   ntohs(icmp->icmp_hun.ih_pptr));
      break;
    default:
      printf("Parameter problem: pointer = 0x%02x\n",
                   ntohs(icmp->icmp_hun.ih_pptr));
      break;
    }
#ifndef icmp_data
    prICMPretip(&icmp->icmp_ip);
#else
    prICMPretip((ip*) icmp->icmp_data);
#endif
    break;
  case ICMP_TSTAMP:
    printf("Timestamp\n");
    break;
  case ICMP_TSTAMPREPLY:
    printf("Timestamp Reply\n");
    break;
  case ICMP_IREQ:
    printf("Information Request\n");
    break;
  case ICMP_IREQREPLY:
    printf("Information Reply\n");
    break;
#ifdef ICMP_MASKREQ
  case ICMP_MASKREQ:
    printf("Address Mask Request\n");
    break;
#endif
#ifdef ICMP_MASKREPLY
  case ICMP_MASKREPLY:
    printf("Address Mask Reply (Mask 0x%08x)\n",
                 ntohl(icmp->icmp_mask));
    break;
#endif
  default:
    printf("Unknown ICMP type: %d\n", icmp->icmp_type);
  }
}


void       printIp(in_addr*    ip)
{
  char                         buf[INET_ADDRSTRLEN + 1];

  if (inet_ntop(AF_INET, ip, buf, INET_ADDRSTRLEN) == NULL)
  {
    perror("inet_ntop() failed");
    return;
  }

  buf[INET_ADDRSTRLEN] = 0;
  printf("Ip : %s\n", buf);
}

void       anPktICMP(char*     packet,
                     int       cc,
                     saddr_in* from,
                     socklen_t fromLen)
{
  struct icmp*                 icmp;
  struct ip*                   ip;
  u_int32_t                    packetSize;
  u_int32_t                    hlen;
  u_int32_t                    i;

  if (cc < 0)
    errx(2, "Invalid packet size %d", cc);
  packetSize = cc;

  if (packetSize < sizeof(struct ip))
  {
    warnx("Packet is too short %d", packetSize);
    return;
  }

  if (fromLen < sizeof(saddr_in))
    warnx("IP length too short");
  else
    printIp(&(from->sin_addr));

  ip = (struct ip*) packet;

  if (IPPROTO_ICMP != ip->ip_p)
    return;

  if ((i = ntohs(ip->ip_len)) != packetSize)
  {
    warnx("IP length %x is different from the gotten one %x",
          i, packetSize);
    packetSize = ((packetSize < i) ? packetSize : i);
  }

  if (packetSize < (sizeof(struct icmphdr) + sizeof(struct ip) + 1))
  {
    warnx("Packet is too short");
    return;
  }

  hlen = ip->ip_hl << 2;
  icmp = (struct icmp*) (packet + hlen);

  if (icmp->icmp_type != ICMP_ECHOREPLY)
  {
    prICMPhdr(icmp);
    return;
  }

  if (icmp->icmp_id != 0x4242)
  {
    printf("Not our id : %x\n", ntohs(icmp->icmp_id));
    return;
  }
  
  for (i = sizeof(struct icmphdr) + hlen; i < packetSize; ++i)
  {
    if (((0x20 <= packet[i]) && (packet[i] <= 0x7e))
        || ('\n' == packet[i]))
      printf("%c", packet[i]);
    else
      printf("\\x%x", packet[i]);
  }
  printf("\n");
}

void       icmpReceiveLoop()
{
  struct sockaddr_in           from;
  socklen_t                    fromLen;
  char                         packet[IP_MAXPACKET];
  int                          sd;
  int                          bufspace;
  int                          cc;

  if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
  {
    perror("socket() failed to get socket descriptor");
    exit(EXIT_FAILURE);
  }

  bufspace = setSockRecvBuf(sd, IP_MAXPACKET);

  while (1)
  {
    fromLen = sizeof(struct sockaddr_in);

    if ((cc = recvfrom(sd, packet, bufspace, 0,
                       (struct sockaddr *)&from, &fromLen)) < 0)
    {
      if (errno == EINTR)
        continue;
      perror("ping: recvfrom");
      continue;
    }
    anPktICMP(packet, cc, &from, fromLen);
  }
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
int        main(void)
{
  icmpReceiveLoop();
  return 0;
}
