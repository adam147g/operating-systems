#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

char read_block[BLOCK_SIZE + 1] = "";


int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Wrong args\n");
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        fprintf(stderr, "Cannot read: %s\n", argv[1]);
        return 2;
    }

    FILE *output_file = fopen(argv[2], "wb");
    if (!output_file) {
        fclose(input_file);
        fprintf(stderr, "Cannot read: %s\n", argv[2]);
        return 3;
    }

    struct timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME, &time_start);

    fseek(input_file, 0, SEEK_END);
    size_t file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    
    size_t n;
    for (int i = 0; i <= file_size / ( BLOCK_SIZE); i++) {
        fseek(input_file, -BLOCK_SIZE*i, SEEK_END);
        fread(read_block, sizeof(char), BLOCK_SIZE, input_file);
    
        char copy[BLOCK_SIZE+1];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            copy[BLOCK_SIZE-1-i] = read_block[i];
        }
        copy[BLOCK_SIZE] = '\0';
        fprintf(output_file, "%s", copy);
    }

    fseek(input_file, 0, SEEK_SET);
    size_t readed_elems = fread(read_block, sizeof(char), file_size % ((size_t) BLOCK_SIZE ), input_file);
    read_block[readed_elems] = '\0';
    n = strlen(read_block);
    char copy[n+1];
    for (int i = 0; i < n; i++) {
        copy[n-1-i] = read_block[i];
    }
    copy[n] = '\0';
    fprintf(output_file, "%s", copy);

    fclose(input_file);
    fclose(output_file);

    clock_gettime(CLOCK_REALTIME, &time_end);

    printf("TIME for %d: %ldns\n", BLOCK_SIZE,  time_end.tv_nsec - time_start.tv_nsec);

    return 0;
}