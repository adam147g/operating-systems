#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>

void* hello(void* arg){
        
        sleep(1);
        while(1){
                printf("Hello world from thread number %d\n", *(int*)arg);
                
		/****************************************************
		* Przerwij działanie wątku jeśli było takie zadanie.
		*****************************************************/  
                
                pthread_testcancel();
                
                printf("Hello again world from thread number %d\n", *(int*)arg);
                sleep(2);
        }
        return NULL;
}


int main(int argc, char** args){

       if(argc != 3){
	    printf("Not a suitable number of program parameters\n");
    	return 1;
   	}

        int n = atoi(args[1]);

	/**************************************************
	* Utwórz n wątków realizujących funkcję hello
	**************************************************/
        
        pthread_t threads[n];
        int thread_args[n];
        
        for (int i = 0; i < n; i++) {
            thread_args[i] = i;
            if (pthread_create(&threads[i], NULL, hello, &thread_args[i]) != 0) {
                printf("Failed to create thread %d\n", i);
                return 1;
            }
        }
        

        int i = 0;
        while(i++ < atoi(args[2])) {
                printf("Hello from main %d\n", i);
                sleep(2);
        }
        
        i = 0;

	/*******************************************
	* "Skasuj" wszystkie uruchomione wątki i poczekaj na ich zakończenie
	*******************************************/
        for (int j = 0; j < n; j++) {
            pthread_cancel(threads[j]);
            if (pthread_join(threads[j], NULL) != 0) {
                printf("Failed to join thread %d\n", j);
                return 1;
            }
        }
        
        printf("DONE");
        
        return 0;
}
