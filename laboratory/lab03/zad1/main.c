#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Bad arguments\n");
        return 1;
    }

    for (int i = 0; i < atoi(argv[1]); i++) {
        if (fork() == 0) {
            printf("ParentPID: %d, MyPID: %d\n", getppid(), getpid());
            return 0;
        }
    }

    for (int i = 0; i < atoi(argv[1]); i++)         
        wait(NULL);
    
    printf("Input = %s\n", argv[1]);

    return 0;
}