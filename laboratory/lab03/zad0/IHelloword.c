#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    printf("HelloWord\n");
    printf("PID glownego programu: %d\n", (int)getpid());
    return 0;
}