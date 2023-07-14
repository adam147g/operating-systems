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

    sleep(1);
    int val = 0;
    /*******************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    odczytaj zapisana tam wartosc i przypisz ja do zmiennej val
    posprzataj
    *********************************************/

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    // zwraca deskryptor plików reprezentujący segment pamięci wspólnej

    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(int)); 
    // służy do zmiany rozmiaru pliku do określonej wartości

    void *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // zwraca adres dołączonego segmentu
    // służy do mapowania obszaru pamięci w przestrzeni adresowej procesu

    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    memcpy(&val, ptr, sizeof(int));
    // do kopiowania danych z jednego obszaru pamięci do innego obszaru pamięci
    // do val kopiujemy, z ptr'a

    munmap(ptr, sizeof(int));
    // odłączenie pamięci wspólnej od przestrzeni adresowej procesu

    close(shm_fd);
    // zamknięcie otwartego deskryptora pliku
    
    shm_unlink(SHM_NAME);
    // usunięcie nazwanego obszaru pamięci współdzielonej

    printf("%d square is: %d \n", val, val * val);
    return 0;
}
