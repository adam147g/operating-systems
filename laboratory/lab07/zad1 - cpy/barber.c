#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "common.h"

#define BREAK 100000

int main(void)
{
    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, P_NUMBER);
    if (queue == NULL){        
        printf("Error in queue");
        exit(2);
    }
    int sem_queue = open_semaphore(SEM_QUEUE_FNAME);
    int sem_chairs = open_semaphore(SEM_CHAIRS_FNAME);
    int sem_barbers = open_semaphore(SEM_BARBERS_FNAME);

    printf("BARBER with pid: %d Initialised \n", getpid());

    while (true){
        aquire(sem_barbers);

        if(strlen(queue) == 0) {
            printf("Empty queue\n");
            shmdt(queue);
            return 2;
        }
        char haircut = queue[0];
        memcpy(queue, queue + 1, strlen(queue) + 1);

        printf("BARBER with pid: %d Making hairuct: %d\n", getpid(), haircut);

        usleep(haircut * 1000);

        printf("BARBER with pid: %d Hairuct: %d ended\n", getpid(), haircut);
        
        release(sem_chairs);
        release(sem_queue);

        if (strlen(queue) == 0){
            usleep(BREAK);
            if (strlen(queue) == 0)
                break;
        }
    }
    printf("BARBER with pid: %d ends job\n", getpid());
    
    // odłącza pamięć współdzieloną
    shmdt(queue);
    return EXIT_SUCCESS;
}

