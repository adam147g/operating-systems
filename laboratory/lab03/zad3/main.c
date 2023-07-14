#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PATH_SIZE 255

void traverse_dir(char *path, char *desired_str) {
    DIR *dir;
    struct dirent *current;
    struct stat file_stat;
    pid_t child_pid;
    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((current = readdir(dir)) != NULL) {
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, current->d_name);

        if (stat(file_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (!S_ISDIR(file_stat.st_mode)){
            FILE *file = fopen(file_path, "r");
            if (file == NULL) {
                perror("fopen");
                continue;
            }

            char line[PATH_MAX];
            if (fgets(line, PATH_MAX, file) == NULL ) 
                perror("fgets");
            
            else if (strstr(line, desired_str) != NULL)
                    printf("%s, pid = %d\n", file_path, getpid());
                    
            if (fclose(file) == -1) 
                perror("fclose");
            
        }
        else {
            if (strcmp(current->d_name, ".") == 0 || strcmp(current->d_name, "..") == 0) {
                continue;
            }
            child_pid = fork();
            if (child_pid == 0) {
                traverse_dir(file_path, desired_str);
                return;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Bad args\n");
        return 1;
    }

    if (strlen(argv[2]) > MAX_PATH_SIZE){
        fprintf(stderr, "Too long path\n");
        return 2;
    }

    char *path = argv[1];

    char *desired_str = argv[2];

    traverse_dir(path, desired_str);

    return 0;
}
