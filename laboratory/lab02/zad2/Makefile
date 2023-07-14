all: 
	make single BLOCK_SIZE=1
	make single BLOCK_SIZE=1024
	make result
	
single: 
	gcc -Wall reverse.c -o reverse_$(BLOCK_SIZE) -D BLOCK_SIZE=$(BLOCK_SIZE)

result:
	touch tmp1 tmp2 checkidentity_1 checkidentity_2
	./reverse_1 plik_binarny tmp1 >> pomiar_zad_2_1.txt ;
	./reverse_1 tmp1 checkidentity_1;
	./reverse_1024 plik_binarny tmp2 >> pomiar_zad_2_1024.txt ; 
	./reverse_1024 tmp2 checkidentity_2; 

	diff -s checkidentity_1 plik_binarny 
	diff -s checkidentity_2 plik_binarny 
clean:
	rm -f reverse_* pomiar_zad_2_1.txt pomiar_zad_2_1024.txt tmp* checkidentity*