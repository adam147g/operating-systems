CC = gcc
FLAG = -Wall
SECOND = common.c

all: barber.c client.c
	$(CC) $(FLAG) client.c $(SECOND) -lpthread -o client
	$(CC) $(FLAG) barber.c $(SECOND) -lpthread -o barber
	$(CC) $(FLAG) main.c $(SECOND) -lpthread -o main

clean:
	rm -rf ./barber ./client ./main .vscode