#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{

   if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

   /***************************************
   Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
   zapisz tam wartosc przekazana jako parametr wywolania programu
   posprzataj
   *****************************************/

    int shared_mem_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shared_mem_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shared_mem_fd, sizeof(int));

    void *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    int value = atoi(argv[1]);
    memcpy(ptr, &value, sizeof(int));

    munmap(ptr, sizeof(int));

    close(shared_mem_fd);



    return 0;
}
