CC = gcc
CFLAGS = -Wall -g

all: ttt mync

ttt: ttt.c
	$(CC) $(CFLAGS) -o ttt ttt.c

mync: mync.c
	$(CC) $(CFLAGS) -o mync mync.c

clean:
	rm -f ttt mync
