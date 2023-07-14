//
// Created by adam147g on 19.04.23.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char *fifo_path = "/tmp/myfifo";
    int fifo_fd;
    char buffer[1024];

    // otwarcie potoku do odczytu
    fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd < 0) {
        perror("open error");
        exit(1);
    }

    // odczytanie danych z potoku
    int nbytes = read(fifo_fd, buffer, sizeof(buffer));
    if (nbytes < 0) {
        perror("read error");
        exit(1);
    }

    // wyświetlenie odczytanych danych na ekranie
    write(STDOUT_FILENO, buffer, nbytes);

    // zamknięcie potoku
    close(fifo_fd);
    return 0;
}