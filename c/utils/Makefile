CC=gcc
CFLAGS=-W -Wall -Werror -g -ggdb -O0

all: queue-test


queue-test: queue-test.c queue.c queue.h
	$(CC) $(CFLAGS) queue-test.c queue.c -o $@

#EOF