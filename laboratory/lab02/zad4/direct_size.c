#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ftw.h>

size_t total_size = 0;

int file_info(const char *path, const struct stat *sb, int typeflag) {
    if (!S_ISDIR(sb->st_mode)) {
        total_size += sb->st_size;
        printf("%s - %ld\n", path, sb->st_size);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong arg\n");
        return -1;
    }
    if (ftw(argv[1], file_info, 1) == -1) {
        fprintf(stderr, "Error in ftw\n");
        return -2;
    }
    fprintf(stderr, "Total size: %ld\n", total_size);

    return 0;
}