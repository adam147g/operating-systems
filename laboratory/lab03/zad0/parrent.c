#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t child_pid;
    child_pid = fork();
    char* args[]= {NULL};
    if(child_pid==0) {
        execvp("./IHelloword", args);
    }
    return 0;
}