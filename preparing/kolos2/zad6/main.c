#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX 20
#define KEY "./path"

int main() {
    int shmid, i;
    int *buf;
    key_t key;

    //Wygeneruj klucz dla kolejki na podstawie KEYi znaku 'a'

    if ((key = ftok(KEY, 'a')) == -1) {
        perror("ftok");
        exit(1);
    }

    //Utwórz segment pamięci współdzielonej shmid o prawach dostępu 600, rozmiarze MAX
    //jeśli segment już istnieje, zwróć błąd, jeśli utworzenie pamięci się nie powiedzie zwróć błąd i wartość 1

    if ((shmid = shmget(key, MAX, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perror("shmget");
        exit(1);
    }

    //Przyłącz segment pamięci współdzielonej do procesu do buf, obsłuż błędy i zwróć 1

    if ((buf = (int *) shmat(shmid, NULL, 0)) == (int *) -1) {
        perror("shmat");
        exit(1);
    }

    for (i = 0; i < MAX; i++) {
        buf[i] = i * i;
        printf("Wartość: %d \n", buf[i]);
    }
    printf("Memory written\n");

    //odłącz i usuń segment pamięci współdzielonej

    if (shmdt(buf) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
