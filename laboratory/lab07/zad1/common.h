#include <stdbool.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/shm.h>

#define PROJECT_IDENTIFIER "/home/adam147g"
#define QUEUE_NAME "0"
#define CHAIRS_NAME "1"
#define BARBERS_NAME "2"
#define P_NUMBER 5
#define F_NUMBER 10

int open_semaphore(const char *filename);
void take(int sem);
void free_sem(int sem);

char *attach_shared_memory(const char* filename, int size);
