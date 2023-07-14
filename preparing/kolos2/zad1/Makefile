CFLAGS = -Wall -std=c99 -lpthread
CC = gcc

main: main.c
	$(CC) $^ -o $@ $(CFLAGS)

main_v2: main_v2.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f main.o main main_v2.o main_v2

test:  main main_v2
	@echo -e "\nTEST\n"
	./main 3 3
	@echo -e "\nTEST - 2\n"
	./main_v2 3 3


all: main main_v2
