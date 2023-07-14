#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEY "queuekey"

struct msgbuf {
    long mtype;
    int mvalue;
};

int main() {
    sleep(1);
    int val = 0;

    /**********************************
    Otworz kolejke systemu V "reprezentowana" przez KEY
    **********************************/

    key_t key_ = ftok(KEY, 'o');
    // generowanie unikalnego klucza IPC od nazwy PLIKU i wartości liczbowej
    // trzeba stworzyć taki plik!!!

    if (key_ == -1) {
        perror("ftok");
        exit(1);
    }

    int msg_id = msgget(key_, 0);
    // uzyskania identyfikatora kolejki komunikatów (message queue) 
    // na podstawie klucza IPC

    // wartość 0, co oznacza, że jeśli kolejka komunikatów o podanym 
    // identyfikatorze nie istnieje, to msgget() zwróci błąd

    if (msg_id == -1) {
        perror("msgget");
        exit(1);
    }

    struct msgbuf msg;

    while (1) {
        /**********************************
        Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
        obowiazuja funkcje systemu V
        ************************************/

        if (msgrcv(msg_id, &msg, sizeof(msg.mvalue), 1, 0) == -1) {
            // id kolejki, wiadomość, rozmiar, typ, flaga: 0 - blokowanie programu do otrzymania komunikatu
            // odbieranie komunikatów z kolejki komunikatów

            perror("msgrcv");
            exit(1);
        }

        val = msg.mvalue;

        printf("%d square is: %d\n", val, val * val);
        break;
    }

    /*******************************
    posprzataj
    ********************************/

    if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
        // kontrolowanie i wykonywanie różnych operacji na kolejce komunikatów
        // flaga: IPC_RMID - usunięcie kolejki komunikatów

        perror("msgctl");
        exit(1);
    }

    return 0;
}
