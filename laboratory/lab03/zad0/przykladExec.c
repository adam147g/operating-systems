#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t child_pid;

    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
//        char *args[] = {NULL};
        if (execlp("./child", "./child", NULL) == -1) {
//        to samo
//        if (execvp("./child", args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Ten napis zostal wyswietlony w programie 'main'!\n");
    }

    return 0;
}
