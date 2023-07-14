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

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /***************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    zapisz tam wartosc przekazana jako parametr wywolania programu
    posprzataj
    *****************************************/

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    // w receiver jest
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(int));
    // w receiver jest

    void *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // w receiver jest

    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    int value = atoi(argv[1]);
    memcpy(ptr, &value, sizeof(int));
    // w receiver jest tylko na odwrót parametry ptr i &val

    munmap(ptr, sizeof(int));
    // w receiver jest

    close(shm_fd);
    // w receiver jest

    return 0;
}
