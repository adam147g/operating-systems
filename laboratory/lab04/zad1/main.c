#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

void handler(int sig) {
    printf("signal no. %d \n", sig);
}

int main(int argc, char** argv) {
    if (argc != 2){
        printf("Bad arguments\n");
        return 1;
    }
    if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);

        if (fork() == 0) {
            raise(SIGUSR1);
            printf("SIGUSR1 is being ignored in child process\n");
            return 1;
        }
        if (fork() == 0) {
            execvp("./main", (char *[]){"main", "exec-ignore", NULL});
            return 1;
        }
    }
    else if(strcmp(argv[1], "exec-ignore") == 0) {
        raise(SIGUSR1);
        printf("SIGUSR1 ignored in exec\n");
    }
    else if (strcmp(argv[1], "mask") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        raise(SIGUSR1);

        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1) ) 
            printf("SIGUSR1 waiting after being masked\n");

        if (fork() == 0) {
            raise(SIGUSR1);
            sigset_t child_set;
            sigpending(&child_set);
            if (sigismember(&child_set, SIGUSR1))
                printf("SIGUSR1 is masked in child process\n");
            return 1;
        }
        if (fork() == 0) {
            execvp("./main", (char *[]){"main", "exec-mask", NULL});
            return 1;
        }
    
    }
    else if(strcmp(argv[1], "exec-mask") == 0) {
        raise(SIGUSR1);
        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1) ) {
            printf("SIGUSR1 is being masked in exec child process\n");
        }
    }
    else if (strcmp(argv[1], "pending") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        raise(SIGUSR1);

        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1) ) {
            printf("SIGUSR1 waiting after being masked\n");
        }

        if (fork() == 0) {
            sigset_t child_set;
            sigpending(&child_set);
            if (!sigismember(&child_set, SIGUSR1)){
                printf("parent's SIGUSR1 is not pending in child process\n");
            }
            return 1;
        }
        execvp("./main", (char *[]){"main", "exec-pending", NULL});
    }
    else if(strcmp(argv[1], "exec-pending") == 0) {
        sigset_t set;
        sigpending(&set);
        if (! sigismember(&set, SIGUSR1) ) {
            printf("parent's SIGUSR1 isn't pending in exec child process\n");
        } else {
            printf("parent's SIGUSR1 is pending in exec child process\n");
        }
    }
    else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handler);
        raise(SIGUSR1);

        if (fork() == 0) {
            printf("child process:\n");
            raise(SIGUSR1);
            return 1;
        }
    }
}