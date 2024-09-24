CFLAGS = -g -Wall -Werror

all: aesdsocket

default: aesdsocket

aesdsocket.o: aesdsocket.c
	$(CC) $(CFLAGS) -c aesdsocket.c

writer: writer.o
	$(CC) $(LDFLAGS) aesdsocket.o -o aesdsocket

clean :
	rm -f aesdsocket aesdsocket.o
