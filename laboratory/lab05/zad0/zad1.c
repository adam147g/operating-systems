// tworzyć potomny, orzesłać od rodzica do potomka komunikat hello
// z wykorzystaniem potoku nienazwanego

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int fd[2];
    char message[12] = "hello world";

    if (pipe(fd) == -1) {
        printf("Błąd tworzenia potoku.\n");
        return 1;
    }

    if (fork() == 0) {
        char read_mess[12];
        close(fd[1]);
        read(fd[0], read_mess, 12);
        printf("Otrzymano wiadomość %s\n", message);
        close(fd[0]);
    } else {
        close(fd[0]);
        write(fd[1], message, 12);
        printf("Wysłano wiadomość\n");
        close(fd[1]);
    }

    return 0;
}