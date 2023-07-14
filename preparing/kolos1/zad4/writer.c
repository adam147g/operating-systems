//
// Created by adam147g on 19.04.23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    char *fifo_path = "/tmp/myfifo";
    int fifo_fd;

    // utworzenie potoku FIFO
    if (mkfifo(fifo_path, 0666) < 0) {
        perror("mkfifo error");
        exit(1);
    }

    // otwarcie potoku do zapisu
    fifo_fd = open(fifo_path, O_WRONLY);
    if (fifo_fd < 0) {
        perror("open error");
        exit(1);
    }

    // zapisanie danych do potoku
    const char *message = "Hello, World!";
    if (write(fifo_fd, message, strlen(message)) < 0) {
        perror("write error");
        exit(1);
    }

    // zamknięcie potoku
    close(fifo_fd);
    return 0;
}