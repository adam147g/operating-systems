CC = gcc
CFLAGS = -Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server -pthread

client: client.c
	$(CC) $(CFLAGS) client.c -o client -pthread

clean:
	rm -f server client


# ./server 12345 /tmp/my_socket
# ./client Client0 web 127.0.0.1 12345
# ./client Client1 unix /tmp/my_socket