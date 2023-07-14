#ifdef USE_DLL
    #include "libwc_so.h"
#else
    #include "libwc.h"
#endif

#include "dllmanager.h"
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/times.h>

#define MAX_INPUT_LENGTH 2048
#define MAX_PATH_LENGTH 2048

typedef enum {
    EXIT,
    INVALID,
    INIT,
    COUNT,
    SHOW,
    DELETE,
    DESTROY,
} COMMAND_ID;

bool running = true;

char* command_buf;
char* command_arg;

COMMAND_ID command_id;
int input_arg;
char* path;

LibWCMemory* memory = NULL;
bool memo_initialised = false;

void error_message() {
    fprintf(stderr, " * INVALID COMMAND, HERE IS A LIST OD VALID ONES:\n");
    fprintf(stderr, " * init {size}\n");
    fprintf(stderr, " * count {path}\n");
    fprintf(stderr, " * show {index}\n");
    fprintf(stderr, " * delete {index}\n");
    fprintf(stderr, " * destroy\n");
    fprintf(stderr, " * exit\n");
}


int parse_input(char* str) {
    int i = 0;
    char* tmp = "\0";
    if(*str != '\0') {
        while(*str != ' ') {
            if(isalpha(*str)) {
                *(command_buf + i) = *str;
                i++;
                *(command_buf + i) = *tmp;
            }
            else if(*str == '\n') {
                *(command_buf + i) = *tmp;
                return 0;
            }
            else {
                printf("BAD COMMAND\n");
                return -1;
            }
            str++;
        }
        if(*(++str) == ' ') {
            printf("MORE THAN 1 SPACE BETWEEN COMMAND AND ARG\n");
            return -2;
        }
        i = 0;
        while(*str != '\0' && *str != '\n') {
            *(command_arg + i) = *str;
            i++;
            str++;
        }
        return 0;
    }
    printf("EMPTY COMMAND\n");
    return 1;
}

int parse_command() {
    if(strcmp(command_buf, "init") == 0) {
        command_id = INIT;
    } else if(strcmp(command_buf, "count") == 0) {
        command_id = COUNT;
    } else if(strcmp(command_buf, "show") == 0) {
        command_id = SHOW;
    } else if(strcmp(command_buf, "delete") == 0) {
        command_id = DELETE;
    } else if(strcmp(command_buf, "destroy") == 0) {
        command_id = DESTROY;
    } else if(strcmp(command_buf, "exit") == 0) {
        command_id = EXIT;
    } else {
        command_id = INVALID;
    }
    return command_id;
}

int parse_arg() {
    if(command_id == EXIT || command_id == DESTROY) {
        return 0;
    }
    else if(isdigit(*command_arg) && command_id != COUNT && command_id != EXIT && command_id != DESTROY) {
        input_arg = atoi(command_arg);
    }
    else if(command_id == COUNT) {
        if(sizeof(command_arg) > MAX_PATH_LENGTH) {
            fprintf(stderr, " > COMMAND ARGUMENT MUST BE SHORTER THAN %i CHARS\n", MAX_PATH_LENGTH);
            return -1;
        }
        path = command_arg;
    }
    else {
        printf("INCORRECT ARG TO PROVIDED COMMAND\n");
        return -2;
    }
    return 0;
}

void make_action() {
    if ((command_id > INIT) && (!memo_initialised)) {
        fprintf(stderr, " * UNITIALIZED MEMORY. RUN 'init {size}' FIRST\n");
        return;
    }
    if ((command_id == INIT) && memo_initialised) {
        fprintf(stderr, " * MEMORY ALREADY INITIALIZED. RUN'destroy'BEFORE REINITIALIZING\n");
        return;
    }
    switch (command_id) {
        case INIT:
            LibWCMemory_init(memory, input_arg);
            memo_initialised = true;
            break;
        case COUNT:
            LibWCMemory_push(memory, path);
            break;
        case SHOW:
            puts(LibWCMemory_get(memory, input_arg));
            break;
        case DELETE:
            LibWCMemory_pop(memory, input_arg);
            break;
        case DESTROY:
            LibWCMemory_destruct(memory);
            memo_initialised = false;
            break;
        case EXIT:
            running = false;
            break;
        case INVALID:
            error_message();
    }
}

int main(int argc, char** argv) {
    load_dll_symbols("libwc.so");

    memory = malloc(sizeof(LibWCMemory));

    struct timespec timespec_buff_start, timespec_buff_end;
    struct tms tms_buff_start, tms_buff_end;

    while (running) {
        command_buf = (char*)malloc(sizeof(char) * MAX_INPUT_LENGTH);
        command_arg = (char*)malloc(sizeof(char) * MAX_PATH_LENGTH);

        printf(">>> ");

        char* line = NULL;
        size_t line_len;

        getline(&line, &line_len, stdin);
        int status = parse_input(line);

        if(status < 0) {
            error_message();
            continue;
        } else if(status == 1) {
            continue;
        }
        if(parse_command() == INVALID) {
            error_message();
            continue;
        }
        if(parse_arg() != 0) {
            continue;
        }
        clock_gettime(CLOCK_REALTIME, &timespec_buff_start);
        times(&tms_buff_start);

        make_action();

        clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
        times(&tms_buff_end);

        puts("TIMES:");
        printf("REAL: %ldns\n", timespec_buff_end.tv_nsec - timespec_buff_start.tv_nsec);
        printf("USER: %ldticks\n", tms_buff_end.tms_cutime - tms_buff_start.tms_cutime);
        printf("SYST: %ldticks\n", tms_buff_end.tms_cstime - tms_buff_start.tms_cstime);
        free(line);
        free(command_buf);
        free(command_arg);
    }
    free(memory);
    return 0;
}