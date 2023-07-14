CC=gcc
CFLAGS= -Wall
ARG1 = ignore
ARG2 = handler
ARG3 = mask
ARG4 = pending

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: main
	rm -f raport2.txt

	for arg in $(ARG1) $(ARG2) $(ARG3) $(ARG4) ; do\
		printf "$$arg: \n" >> raport2.txt ;\
		./main $$arg >> raport2.txt ;\
		printf "\n" >> raport2.txt ;\
	done ;\

clean:
	rm -f main