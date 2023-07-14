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

    sleep(1);
    int val =0;
    /*******************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    odczytaj zapisana tam wartosc i przypisz ja do zmiennej val
    posprzataj
    *********************************************/


    int shared_mem_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shared_mem_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    ftruncate(shared_mem_fd, sizeof(int));
    void *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);

    if (ptr == NULL) {
        perror("mmap");
        exit(1);
    }
    memcpy(&val, ptr, sizeof(int));
    munmap(ptr, sizeof(int));
    close(shared_mem_fd);
    shm_unlink(SHM_NAME);
	printf("%d square is: %d \n",val, val*val);
    return 0;
}
