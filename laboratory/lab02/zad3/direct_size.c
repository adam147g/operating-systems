#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

int main(void) {
    DIR *dirp;              
    struct dirent *dp;         
    struct stat filestat;      
    long long total_size = 0;

    dirp = opendir(".");
    if (dirp == NULL) {
        fprintf(stderr, "Cannot open curr directory\n");
        return -1;
    }
    dp = readdir(dirp);
    while (dp != NULL) {
        if (stat(dp->d_name, &filestat) < 0) {
            fprintf(stderr, "Stat error\n");
            dp = readdir(dirp);
            continue;
        }

        if (!S_ISDIR(filestat.st_mode)) {  
            total_size += filestat.st_size;
            fprintf(stderr,"%s - %ld\n", dp->d_name, (long int)filestat.st_size);
        }
        dp = readdir(dirp);
    }

    fprintf(stderr, "Total size: %lld\n", total_size);

    closedir(dirp);
    return 0;
}
