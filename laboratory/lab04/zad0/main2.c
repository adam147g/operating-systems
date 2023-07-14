#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void au(){
    printf("Wyslij sygnal\n");
}


int main(){
    signal(SIGUSR1, au);
    pid_t childpid;
    childpid = fork();
    while (1){
        if (childpid != 0){
            kill(childpid, SIGUSR1);
        }   
        sleep(1);
    }
    return 0;
}