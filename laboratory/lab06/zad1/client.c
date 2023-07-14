#include <stdio.h>
#include "common.h"
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
#include <time.h>

static int client_queue_id;
static int server_queue_id;
static int client_index;

void end_function(){
    Message end_message = {.mtype=5, .index=client_index};
    if (msgsnd(server_queue_id, &end_message, MESSAGE_LEN, 0) == -1) {
        perror("msgsnd error on end_function");
        exit(1);
    }
    msgctl(client_queue_id, IPC_RMID, NULL);
    exit(0);
}

int init_connection_to_server(key_t key) {
    server_queue_id = create_queue("./server", ftok_id, 0);
    Message init_message = {.mtype = 1, .index = key};

    sprintf(&init_message.text[0], "%d", getpid());
    if (msgsnd(server_queue_id, &init_message, MESSAGE_LEN, 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    if (msgrcv(client_queue_id, &init_message, MESSAGE_LEN, 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    if (strcmp(init_message.text, "-1") == 0) {
        printf("Too much clients\n");
        exit(0);
    }

    return init_message.index;
}

void handle_function(char* function) {
    Message to_send_message;
    
    if (strcmp(function, "2ONE") == 0) {
        printf("enter receiver and text: ");

        int idx;
        char text[MESSAGE_LEN];
        scanf("%d %[^\n]", &idx, text);

        to_send_message.to_idx = idx;
        to_send_message.mtype = 2; 
        to_send_message.index = client_index;
        sprintf(to_send_message.text, "%s", text);        

        if (msgsnd(server_queue_id, &to_send_message, MESSAGE_LEN, 0) == -1) {
            perror("msgsnd 2ONE client");
            return;
        }
        printf("message was send to index: %d\n", to_send_message.to_idx);
    }
    
    else if (strcmp(function, "2ALL") == 0) {
        printf("Wprowadz wiadomosc: ");
        char text[MESSAGE_LEN];
        scanf(" %[^\n]", text);

        to_send_message.mtype = 3; 
        to_send_message.index = client_index;
        strcpy(to_send_message.text, text);

        if (msgsnd(server_queue_id, &to_send_message, MESSAGE_LEN, 0) == -1) {
            perror("msgsnd 2ALL client");
            return;
        }

        printf("message was send 2ALL\n");
    }
    else if (strcmp(function, "LIST") == 0) {
        to_send_message.mtype = 4; 
        to_send_message.index = client_index;

        if (msgsnd(server_queue_id, &to_send_message, MESSAGE_LEN, 0) == -1) {
            perror("msgsnd LIST client");
            return;
        }

        if (msgrcv(client_queue_id, &to_send_message, MESSAGE_LEN, 4, 0) == -1) {
            perror("msgrcv LIST client");
            return;
        }

        printf("Clients List:\n");
        printf("%s\n", to_send_message.text);
    }
    
}


void listen(key_t key) {
    client_index = init_connection_to_server(key);
    printf("My ID: %d\n", client_index);
    Message received_message;
    printf("Functions:\n2ONE,2ALL,LIST, STOP\n");


    while (true) {
        if (
                msgrcv(client_queue_id, &received_message, MESSAGE_LEN, 4, IPC_NOWAIT) == -1 &&
                msgrcv(client_queue_id, &received_message, MESSAGE_LEN, 3, IPC_NOWAIT) == -1 &&
                msgrcv(client_queue_id, &received_message, MESSAGE_LEN, 2, IPC_NOWAIT) == -1 
                )
        {
            printf("--");
            char function_type[5];
            scanf("%s", function_type);
            if (strcmp(function_type, "2ALL") == 0||strcmp(function_type, "2ONE") == 0||strcmp(function_type, "LIST") == 0) {
                handle_function(function_type);
            }
            else if (strcmp(function_type, "STOP") == 0) {
                break;
            }
            else if (strlen(function_type) > 0){
                continue;
            }
        } 
        else {
            printf("Message %s\n%s\n", taskTypeToString(received_message.mtype), received_message.text );
        }
    }
}

int main(int argc, char** argv) {
    signal(SIGINT, end_function);
    
    client_queue_id = create_queue(argv[0], getpid(), IPC_CREAT);

    key_t key;
    if ((key = ftok(argv[0], getpid())) == -1) {
        perror("ftok client");
        exit(1);
    }

    listen(key);
    end_function();

    return 1;
}

