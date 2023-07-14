#include "libwc.h"

char command[LIBWC_COMMAND_BUFF_SIZE] = "";

LibWCMemory LibWCMemory_create(size_t size) {
    LibWCMemory memory;
    memory.array = calloc(size, sizeof(char*));
    memory.top = 0;
    memory.size = size;
    return memory;
}

void LibWCMemory_init(LibWCMemory* LibWCMemory, size_t size) {
    (*LibWCMemory).array = calloc(size, sizeof(char*));
    (*LibWCMemory).top = 0;
    (*LibWCMemory).size = size;
}


void LibWCMemory_destruct(LibWCMemory* LibWCMemory) {
    for (size_t i = 0; i < (*LibWCMemory).top; i++)
        free((*LibWCMemory).array[i]);
    (*LibWCMemory).top = 0;
    free((*LibWCMemory).array);
}

char* LibWCMemory_get(LibWCMemory* LibWCMemory, size_t index) {
    if ((*LibWCMemory).top <= index) {
        fprintf(stderr, "INDEX OUT OF RANGE\n");
        return ""; 
    }
    return (*LibWCMemory).array[index];
}

void LibWCMemory_pop(LibWCMemory* LibWCMemory, size_t index) {
    if ((*LibWCMemory).top <= index) {
        fprintf(stderr, "INDEX OUT OF RANGE\n");
        return ; 
    }
    for (size_t i = index; i < (*LibWCMemory).top - 1; i++){
        (*LibWCMemory).array[i] = (*LibWCMemory).array[i + 1];
    }
    (*LibWCMemory).top -= 1;
    free((*LibWCMemory).array[(*LibWCMemory).top]);
}


void LibWCMemory_push(LibWCMemory* LibWCMemory, char* input_filename) {
    char tmp_filename[] = "/tmp/wclib_XXXXXX";
    int tmp_file = mkstemp(tmp_filename);

    if (tmp_file == 0) {
        fprintf(stderr, "FAILED TO CREATE A TEMPORARY FILE\n");
        return;
    }

    snprintf(command, LIBWC_COMMAND_BUFF_SIZE, "wc '%s' 1> '%s'", input_filename, tmp_filename);
    system(command);
    
    FILE* file = fopen(tmp_filename, "r");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buff = calloc(size, sizeof(char));
    fread(buff, sizeof(char), size, file);
    fclose(file);

    if (strlen(buff) == 0) {
        fprintf(stderr, "FAILED READING FILE \n");
        return;
    }

    snprintf(command, LIBWC_COMMAND_BUFF_SIZE, "rm -f '%s'", tmp_filename);
    system(command);

    if ((*LibWCMemory).top >= (*LibWCMemory).size) 
        fprintf(stderr, "NOT ENOUGH MEMORY\n");
    else {
        (*LibWCMemory).array[(*LibWCMemory).top] = buff;
        ((*LibWCMemory).top)++;
    }
}