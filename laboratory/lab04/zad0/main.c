//
// Created by adam147g on 19.04.23.
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void sighandler(int signo, siginfo_t *info, void *context) {
    printf("Received SIGUSR1 signal from parent with value: %d\n", info->si_value.sival_int);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        printf("Error\n");
        exit(1);
    }

    int child = fork();
    if(child == 0) {
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
            printf("Cannot block signals");
            exit(1);
        }

        pause(); // Waiting for signal
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
    }
    else {
        union sigval value;
        value.sival_int = atoi(argv[1]);
        int sig = atoi(argv[2]);

        sigqueue(child, sig, value);
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
    }

    return 0;
}
