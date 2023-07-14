#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>

#include "common.h"

int main(void) {
    srand(getpid());

    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, P_NUMBER);
    if (queue == NULL) {
        printf("Error in queue");
        exit(2);
    }
    int sem_queue = open_semaphore(QUEUE_NAME);
    int sem_chairs = open_semaphore(CHAIRS_NAME);
    int sem_barbers = open_semaphore(BARBERS_NAME);

    take(sem_queue);

    char haircut = (char) (rand() % F_NUMBER + 1);
    printf("CLIENT with pid: %d - haircut no.%d \n", getpid(), haircut);
    
    int size = strlen(queue);
    if(size + 1 == P_NUMBER) {
        printf("Full queue\n");
        shmdt(queue);
        return 2;
    }
    queue[size] = haircut;
    queue[size + 1] = '\0';

    free_sem(sem_barbers);
    take(sem_chairs);

    printf("CLIENT with pid: %d done.\n", getpid());
    shmdt(queue);

    return EXIT_SUCCESS;
}
