#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <math.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE_ 256
#define END 1
#define BEGIN 0



double power(double x){return x*x;}

double integral(double beginning, double end, double width) {
    double result = 0.0;
    for (double i = beginning; i < end; i += width)
        result += 4 / (power(i) + 1) * width;
    return result;
}


int main(int argc, char** argv) {
    double width = strtod(argv[1], NULL);
    int n = atoi(argv[2]);
    double length = END - BEGIN;
    double part_len = length/n;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    // alokacja pamięci na tablicę deskryptorów plików dla potoków
    int* pipes = calloc(n, sizeof(int));
    pid_t pid;
    for (int i = BEGIN; i < n; i++) {
        int fd[2];
        pipe(fd);
        pid = fork();
        if (pid == 0) {
            close(fd[0]);
            double curr = integral(i*part_len, (i+1)*part_len, width);
            char buff_w[SIZE_];
            int size = sprintf(buff_w, "%lf", curr);
            write(fd[1], buff_w, size);
            return 0;
        } else {
            close(fd[1]); 
            pipes[i] = fd[0];            
        }
    }

    while (wait(NULL) > 0){}
    char buff_r[SIZE_];
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        read(pipes[i], buff_r, SIZE_);
        result += strtod(buff_r, NULL);
    }

    free(pipes);

    clock_gettime(CLOCK_REALTIME, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
    printf("number of processes, n: %d, width: %.17f\n", n, width);
    printf("Program time: %fs\n", elapsed_time);
    printf("Result: %lf\n\n", result);
    return 0;
}
