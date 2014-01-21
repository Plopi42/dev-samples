CC=gcc
CFLAGS=-W -Wall -Werror -g -ggdb -O0

all: queue-test stack-test


queue-test: queue-test.c queue.c queue.h
	$(CC) $(CFLAGS) queue-test.c queue.c -o $@

stack-test: stack-test.c stack.c stack.h
	$(CC) $(CFLAGS) stack-test.c stack.c -o $@

#EOF