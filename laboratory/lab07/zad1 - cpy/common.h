#include <stdbool.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/shm.h>

// identyfikator ścieżki katalogu domowego
#define PROJECT_IDENTIFIER "/home/adam147g"
#define SEM_QUEUE_FNAME "0"
#define SEM_CHAIRS_FNAME "1"
#define SEM_BARBERS_FNAME "2"
#define P_NUMBER 5
#define F_NUMBER 10

int open_semaphore(const char *filename);
void aquire(int sem);
void release(int sem);

char *attach_shared_memory(const char* filename, int size);
