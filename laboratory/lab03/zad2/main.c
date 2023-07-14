#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Bad arguments\n");
        return 1;
    }

    printf("%s ", argv[0]);
    setbuf(stdout, NULL);    
    execl("/bin/ls", "ls", argv[1], NULL);
    perror("execl");
    return 0;
}