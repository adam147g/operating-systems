#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ( int argc, char *argv[] )
{
    int i, pid;

    if(argc != 2){
        printf("Not a suitable number of program arguments");
        exit(2);
    }else {
       for (i = 0; i < atoi(argv[1]); i++) {
           pid = fork();

           if (pid == -1) {
               printf("Couldn't fork\n");
               exit(1);
           }
           else if (pid == 0) {
               printf("Kid number: %d. My PID is %d. My parent is %d.\n", i + 1, getpid(), getppid());
               exit(0);
           }
        //*********************************************************
        //Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1] procesow potomnych, bedacych dziecmi
        //   tego samego procesu macierzystego.
           // Kazdy proces potomny powinien:
               // - "powiedziec ktorym jest dzieckiem",
                //-  jaki ma pid,
                //- kto jest jego rodzicem
           //******************************************************
        }
    }
    return 0;
}
