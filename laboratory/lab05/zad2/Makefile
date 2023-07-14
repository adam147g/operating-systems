all: zad2.c
	gcc -Wall zad2.c -o zad2

raport:
	make all
	./zad2 0.000000001 1  >> raport.txt
	./zad2 0.0000001 1  >> raport.txt
	./zad2 0.000000001 2  >> raport.txt
	./zad2 0.0000001 2  >> raport.txt
	./zad2 0.000000001 5  >> raport.txt
	./zad2 0.0000001 5  >> raport.txt
	./zad2 0.0000000001 10 >> raport.txt
	./zad2 0.000000001 10 >> raport.txt
	./zad2 0.0000001 10 >> raport.txt

clean:
	rm -f zad2
cleanAll:
	rm -f raport.txt
