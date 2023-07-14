#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main ( int argc, char *argv[] )
{
    int i, pid;

    if(argc != 2){
        printf("Not a suitable number of program arguments");
        exit(2);
    }else {
       for (i = 0; i < atoi(argv[1]); i++) {
           pid = (int)getpid();
           if (fork()==0){
               printf("Jestem dzieckiem nr : %d , mój pid to: %d, pid rodzica to %d\n", i+1, getpid(), pid);
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
