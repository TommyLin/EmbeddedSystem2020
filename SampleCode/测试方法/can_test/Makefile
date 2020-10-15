CROSS_COMPILE = arm-linux-gnueabihf-
CC = gcc

all: can_read can_write

tcprecv: can_read.c
	$(CROSS_COMPILE)$(CC) -lpthread -o can_read can_read.c

tcpsend: can_write.c
	$(CROSS_COMPILE)$(CC) -lpthread -o can_write can_write.c

clean:
	rm -rf can_read can_write
