#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "common.h"


static int get_shared_memory(const char *filename, int size) {
    // uzyskuje pamięć współdzieloną na podstawie nazwy pliku
    key_t key = ftok(filename, 0);
    if (key == -1) {
        printf("afa");
        return -1;
    }
    // tworzy nową/uzyskuje dostęp do pamięci współdzielonej
    return shmget(key, size, 0644 | IPC_CREAT);
}

char *attach_shared_memory(const char* filename, int size) {
    int shared_memory_id = get_shared_memory(filename, size);
    if (shared_memory_id == -1) {
        printf("Error in get_shared_memory");
        return NULL;
    }
    // podłącza pamięć współdzieloną o określonym 
    // identyfikatorze do przestrzeni adresowej procesu
    char *shared_memory = shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (char*)(-1)) {
        printf("Error in shmat");
        return NULL;
    }
    return shared_memory;
}


int open_semaphore(const char *filename) {
    key_t key = ftok(PROJECT_IDENTIFIER, filename[0]);
    if (key == -1) {
        return -1;
    }
    // tworzy lub otwiera semafor o podanym kluczu
    return semget(key, 1, 0);
}


void aquire(int sem) {
    struct sembuf operation = { 0, -1, 0 };
    if(semop(sem, &operation, 1) == -1) {
        printf("Error in aquire"); 
    }
}


void release(int sem) {
    struct sembuf operation = { 0, 1, 0 };
    if(semop(sem, &operation, 1) == -1){
        printf("Error in release");
    }
}
