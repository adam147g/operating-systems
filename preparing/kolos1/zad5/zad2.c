#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int fd[2];

void handler(int sig) {
    static int counter = 0;
    counter++;
    write(fd[1], &counter, sizeof(counter));
}

/*program tworzy dwa procesy potomne. Proces macierzysty co sekunde wysyla
SIGUSR1 do procesu potomnego 1. Proces potomny 1 po otrzymaniu sygnalu przesyla
kolejna liczbe przez potok nienazwany do procesu potomnego 2, ktory wyswietla
te liczbe */

//
//
int main(int lpar, char *tab[]) {
    pid_t pid1, pid2;
    int d, i;
    signal(SIGUSR1, handler);

    if (pipe(fd) == -1) {
        perror("pipe\n");
        return 1;
    }

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        return 0;
    } else if (pid1 == 0) {//proces 1
        close(fd[0]);

        while (1) {
            pause();
        };
        return 0;
    } else {
        pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            return 0;
        } else if (pid2 == 0) {//proces 2
            close(fd[1]);
            while (1) {
                read(fd[0], &i, sizeof(int));
                d = sizeof(i);
                printf("przyjeto %d bajtow, wartosc:%d\n", d, i);
                sleep(1);
            }
            return 0;
        }
    }
    close(fd[0]);
    close(fd[1]);
    while (1) {
        kill(pid1, SIGUSR1);
        printf("wyslano SIGUSR1\n");
        sleep(1);
    }
}