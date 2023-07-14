#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>

#define MESSAGE_LEN 2048
#define MAX_NO_CLIENTS 32

const int ftok_id = 0xAAAA;

int create_queue(const char *path, int ftok_flags, int ipc_flags) {
    key_t key;

    if ((key = ftok(path, ftok_flags)) == -1) {
        perror("ftok");
        exit(1);
    }

    int msgid;
    if ((msgid = msgget(key, ipc_flags)) == -1) {
        perror("msgget");
        exit(1);
    }

    if (ipc_flags == IPC_CREAT) {
        printf("Queue %d created\n", msgid);
    } else {
        printf("Queue %d opened\n", msgid);
    }

    return msgid;
}

const char *taskTypeToString(int taskType) {
    switch (taskType) {
        case 1:
            return "INIT";
        case 2:
            return "TOONE";
        case 3:
            return "TOALL";
        case 4:
            return "LIST";
        case 5:
            return "STOP";
        default:
            return "UNKNOWN";
    }
}


typedef struct {
    long mtype;
    int index;
    int to_idx;
    char text[MESSAGE_LEN];
} Message;


typedef struct {
    int que_id;
    int index;
    bool status;
} Client;

