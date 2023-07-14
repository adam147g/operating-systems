#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KEY "queuekey"

struct msgbuf {
    long mtype;
    int mvalue;
};

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /******************************************************
    Utworz kolejke komunikatow systemu V "reprezentowana" przez KEY
    Wyslij do niej wartosc przekazana jako parametr wywolania programu
    Obowiazuja funkcje systemu V
    ******************************************************/

    key_t key_ = ftok(KEY, 'o');
    // w receiver jest


    if (key_ == -1) {
        perror("ftok");
        exit(1);
    }

    int msg_id = msgget(key_, IPC_CREAT | IPC_EXCL | 0666);
    // w receiver jest
    
    if (msg_id == -1) {
        perror("msgget");
        exit(1);
    }

    struct msgbuf msg;
    msg.mtype = 1;
    msg.mvalue = atoi(argv[1]);
    // ustawienie wartości do wysłania

    if (msgsnd(msg_id, &msg, sizeof(msg.mvalue), 0) == -1) {
        // wysyłanie komunikatu do kolejki komunikatów.
        // 0 - blokowanie wykonania programu, jeśli nie ma miejsca w kolejce komunikatów
     
        perror("msgsnd");
        exit(1);
    }

    return 0;
}



