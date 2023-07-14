#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int main() {
    pid_t child;
    int status;
    if ((child = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child == 0) {
        sleep(100);
        exit(EXIT_SUCCESS);
    } else {
        // Proces macierzysty pobiera status zakończenia potomka child,
        // nie zawieszając swojej pracy.
        if (waitpid(child, &status, WNOHANG) == 0) {
            // Proces potomny nie zakończył się, wysyłamy do niego sygnał SIGKILL.
            if (kill(child, SIGKILL) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            // Czekamy na zakończenie procesu potomka, zawieszając pracę.
            if (waitpid(child, &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }
        // Proces potomny zakończył się, wypisujemy informację o sukcesie.
        if (WIFEXITED(status)) {
            printf("Proces potomny o PID %d zakończył się ze statusem %d.\n", child, WEXITSTATUS(status));
        }
    } // else

    exit(EXIT_SUCCESS);
}