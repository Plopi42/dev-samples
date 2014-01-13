CC=gcc

ifeq ($(DEBUG),1)
CFLAGS=-W -Wall -Werror -pedantic -g -ggdb
else
CFLAGS=-W -Wall -Werror -pedantic
endif

all: pandaICMPListener pandaICMPSender

pandaICMPListener: pandaICMPListener.c
	$(CC) $(CFLAGS) $< -o $@

pandaICMPSender: pandaICMPSender.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm pandaICMPListener pandaICMPSender
	find . -name '*~' -delete

# EOF