// program, który wyśle sygnał SIGUSR1
#include <stdio.h>
#include <signal.h>


void au(){
    printf("Wyslij sygnal\n");
}

int main(){
    printf("Start\n");
    signal(SIGUSR1, au);
    raise(SIGUSR1); // = kill(getpid(), SIGUSR1); - to samo

    return 0;
}