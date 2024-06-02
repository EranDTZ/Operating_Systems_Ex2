CC = gcc
CFLAGS = -Wall -g

all: ttt mync mync_tcp mync_udp mync_unix

ttt: ttt.c
	$(CC) $(CFLAGS) -o ttt ttt.c

mync: mync.c
	$(CC) $(CFLAGS) -o mync mync.c

mync_tcp: mync_tcp.c
	$(CC) $(CFLAGS) -o mync_tcp mync_tcp.c

mync_udp: mync_udp.c
	$(CC) $(CFLAGS) -o mync_udp mync_udp.c

mync_unix: mync_unix.c
	$(CC) $(CFLAGS) -o mync_unix mync_unix.c

clean:
	rm -f ttt mync mync_tcp mync_udp mync_unix
