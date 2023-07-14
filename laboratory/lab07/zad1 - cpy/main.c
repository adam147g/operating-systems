#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "common.h"

#define M_NUMBER 4
#define N_NUMBER 2
#define CUSTOMERS_TOTAL 10

int create_semaphore(const char *filename, int init_val) {
    key_t key = ftok(PROJECT_IDENTIFIER, filename[0]);
    if (key == -1) {
        printf("Error in ftok");
        return -1;
    }
    int semid = semget(key, 1, 0664 | IPC_CREAT);
    if (semid == -1) {
        printf("Error in semid");
        return -1;
    }
    // inicjalizacja wartości semafora. 
    if(semctl(semid, 0, SETVAL, init_val) == -1) {
        printf("Error in semctl");
        return -1;
    }
    return semid;
}

static int get_shared_memory(const char *filename, int size) {
    // uzyskuje pamięć współdzieloną na podstawie nazwy pliku
    key_t key = ftok(filename, 0);
    if (key == -1) {
        return -1;
    }
    // tworzy nową/uzyskuje dostęp do pamięci współdzielonej
    return shmget(key, size, 0644 | IPC_CREAT);
}

int destroy_shared_memory(const char *filename) {
    int shared_memory_id = get_shared_memory(filename, 0);
    if(shared_memory_id == -1) {
        return false;
    }
    if (shmctl(shared_memory_id, IPC_RMID, NULL) == -1){
        return 0;
    }
    return 1;
}

void unlink_semaphore(const char* filename) {
    //usunięcie semafora na podstawie nazwy
    key_t key = ftok(PROJECT_IDENTIFIER, filename[0]);
    if (key == -1) {
        printf("Error in ftok\n");
        return;
    }
    int semid = semget(key, 1, 0);
    if( semid == -1) {
        printf("Error in open_semaphore\n");
        return;
    }
    semctl(semid, 0, IPC_RMID);
}


int main(int argc, char* argv[]) {
    if(attach_shared_memory(PROJECT_IDENTIFIER, P_NUMBER) == NULL) {
        exit(2);
    }
    create_semaphore(SEM_QUEUE_FNAME, N_NUMBER);
    create_semaphore(SEM_CHAIRS_FNAME, 0);
    create_semaphore(SEM_BARBERS_FNAME, 0);

    for(int i = 0;i < M_NUMBER;++i)
        if (fork() == 0)
            execl("./barber", "./barber", NULL);


    for(int i = 0; i < CUSTOMERS_TOTAL;++i)
        if (fork() == 0)
            execl("./client", "./client", NULL);
    
    while(wait(NULL) > 0);

    unlink_semaphore(SEM_QUEUE_FNAME);
    unlink_semaphore(SEM_CHAIRS_FNAME);
    unlink_semaphore(SEM_BARBERS_FNAME);
 
    if (!destroy_shared_memory(PROJECT_IDENTIFIER)) {
        printf("Error in destroy_shared_memory\n");
        exit(2);
    }
    return EXIT_SUCCESS;
}
