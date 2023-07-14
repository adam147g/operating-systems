#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "/kol_sem1"


int main(int argc, char **args) {
    if (argc != 4) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /**************************************************
    Stworz semafor systemu V
    Ustaw jego wartosc na 1
    ***************************************************/

    key_t sem_key = ftok(FILE_NAME, 'a');

    int sem_id = semget(sem_key, 1, 0);
    // Sprawdź czy istnieje semafor o podanym kluczu i usuń go

    if (sem_id != -1) {
        if (semctl(sem_id, 0, IPC_RMID) == -1) {
            perror("semctl");
            exit(1);
        }
    }

    sem_id = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
    // jeśli semafor o podanym kluczu nie istnieje, to zostanie 
    // utworzony nowy zbiór semaforów
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(1);
    }

    // ----------------------------start
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    int parentLoopCounter = atoi(args[1]);
    int childLoopCounter = atoi(args[2]);

    char buf[40];
    pid_t childPid;
    int max_sleep_time = atoi(args[3]);


    if ((childPid = fork())) {
        int status = 0;
        srand((unsigned) time(0));

        while (parentLoopCounter--) {
            int s = rand() % max_sleep_time + 1;
            sleep(s);
    // ----------------------------stop

            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/

            struct sembuf sem_op;
            sem_op.sem_num = 0;
            // pierwszy semafor
            sem_op.sem_op = -1;
            // dekrementacja semafora
            sem_op.sem_flg = 0;
            // brak flag
            if (semop(sem_id, &sem_op, 1) == -1) {
                //                     ^
                //                liczba operacji
                perror("semop");
                exit(1);
            }

    // ----------------------------start
            sprintf(buf, "Wpis rodzica. Petla %d. Spalem %d\n", parentLoopCounter, s);
            write(fd, buf, strlen(buf));
            write(1, buf, strlen(buf));
    // ----------------------------stop

            sem_op.sem_op = 1;
            if (semop(sem_id, &sem_op, 1) == -1) {
                perror("semop");
                exit(1);
            }

            /*********************************
            Koniec sekcji krytycznej
            **********************************/

        }
        waitpid(childPid, &status, 0);
    } else {

        srand((unsigned) time(0));
        while (childLoopCounter--) {

            int s = rand() % max_sleep_time + 1;
            sleep(s);


            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/

            struct sembuf sem_op;
            sem_op.sem_num = 0;
            sem_op.sem_op = -1;
            sem_op.sem_flg = 0;
            if (semop(sem_id, &sem_op, 1) == -1) {
                perror("semop");
                exit(1);
            }
    // ----------------------------start

            sprintf(buf, "Wpis dziecka. Petla %d. Spalem %d\n", childLoopCounter, s);
            write(fd, buf, strlen(buf));
            write(1, buf, strlen(buf));
    // ----------------------------stop

            sem_op.sem_op = 1;
            if (semop(sem_id, &sem_op, 1) == -1) {
                perror("semop");
                exit(1);
            }

            /*********************************
            Koniec sekcji krytycznej
            **********************************/

        }
        _exit(0);
    }

    /*****************************
    posprzataj semafor
    ******************************/

    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }
    // ----------------------------start

    close(fd);
    return 0;
}
     
        
