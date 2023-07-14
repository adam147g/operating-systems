
all:
	gcc -c stringlibrary.c
	ar crs libstringlibrary.a stringlibrary.o
	gcc -c main.c
	gcc main.o libstringlibrary.a -o main
	./main
clean:
	rm -f  libstringlibrary.a stringlibrary.o main main.o