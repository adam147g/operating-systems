all:
	make dynamic

dynamic: bibl1.c
	gcc -Wall -fPIC -c bibl1.c
	gcc -Wall -fPIC -shared -o bibl1.so bibl1.o
	gcc -Wall -fPIC zad1.c -o main -L. -ldl -D DLL

clean:
	rm -f *.o *.so main