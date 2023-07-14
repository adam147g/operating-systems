#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>


static int server_queue_id;
int active_clients = 0;
int finished = false;

Client *clients;




void end_function() {
    finished = true;
    free(clients);
    msgctl(server_queue_id, IPC_RMID, NULL);
    exit(0);
}

void handle_function(Message* received_message) {
    int client_idx = received_message->index;
    
    FILE* log = fopen("log.txt", "a+");
    char line[128];
    char buff[24];
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strcpy(buff, asctime(timeinfo));
    buff[24] = '\0';
    snprintf(line, 128, "%s | %d | %s\n", buff, received_message->index, taskTypeToString(received_message->mtype));
    fwrite(line, strlen(line), sizeof (char), log);
    fclose(log);
    
    Message to_send_message;
        if (received_message->mtype==1) {
            int msgid;
            if ((msgid = msgget((key_t) client_idx, 0666))== -1) {
                perror("msgget INIT");
                return;
            }

            to_send_message.mtype = 1;
            int client_index;
            for (int i = 0; i < 128; ++i) {
                if (!clients[i].status){
                    client_index =  i;
                    break;
                }
                if (i == 128){
                    sprintf(&to_send_message.text[0], "%d", -1);
                    if (msgsnd(msgid, &to_send_message, MESSAGE_LEN, 0) == -1){
                        perror("msgsnd INIT");
                    }
                    return;    
                }
            }

            clients[client_index].index = client_index;
            clients[client_index].status = true;
            clients[client_index].que_id = msgid;
            to_send_message.index = clients[client_index].index;
            active_clients++;
            Message init_mess; 
            init_mess.mtype = 1; 
            init_mess.index = clients[client_index].index; 
            init_mess.text[0] = '\0';
            if(msgsnd(clients[client_index].que_id, &init_mess, MESSAGE_LEN, 0) == -1) {
                perror("msgsnd INIT");
                return;
            }

            printf("INIT | client_index: %d, que_id: %d\n",
                   clients[client_index].index,
                   clients[client_index].que_id);
            }
        
        else if (received_message->mtype==2) {

            printf("2ONE | client_index: %d, que_id: %d ,receiver_id: %d\n",
                   clients[client_idx].index,
                   clients[client_idx].que_id,received_message->to_idx);
            to_send_message.mtype = 2;
            to_send_message.to_idx = clients[client_idx].index;
            to_send_message.index = client_idx;
            int target_index = received_message->to_idx;
            snprintf(&to_send_message.text[0], MESSAGE_LEN, "%s", received_message->text);

            if (clients[target_index].status!=true) {
                perror("target client not CONNECTED");
                return;
            }
            if(msgsnd(clients[target_index].que_id, &to_send_message, MESSAGE_LEN, 0) == -1) {
                perror("msgsnd 2ONE");
                return;
            }
        }
        
        
        else if (received_message->mtype==3) {
            printf("2ALL | client_index: %d, que_id: %d\n",
                   clients[client_idx].index,
                   clients[client_idx].que_id);
            to_send_message.mtype = 3;
            to_send_message.index = client_idx;
            snprintf(to_send_message.text, MESSAGE_LEN, "%s", received_message->text);

            for (int i = 0; i < MAX_NO_CLIENTS; ++i) {
                if (clients[i].status && i != client_idx) {
                    if(msgsnd(clients[i].que_id, &to_send_message, MESSAGE_LEN, 0) == -1) {
                        perror("msgsnd 2ALL");
                        return;
                    }
                }
            }
        }
        
        else if (received_message->mtype==4) {
            printf("LIST | client_index: %d, que_id: %d\n",
                   clients[client_idx].index,
                   clients[client_idx].que_id);

            to_send_message.mtype = 4;
            to_send_message.index = client_idx;
            int j = 0;
            j += snprintf(&to_send_message.text[j], MESSAGE_LEN, "You\t%d\n", clients[client_idx].index);
            for (int i = 0; i < MAX_NO_CLIENTS; ++i) {
                if (clients[i].status && client_idx != i) {
                    j += snprintf(&to_send_message.text[j], MESSAGE_LEN - j, " -\t%d\n", clients[i].index);
                }
            }

            if (msgsnd(clients[client_idx].que_id, &to_send_message, MESSAGE_LEN, 0) == -1) {
                perror("msgsnd LIST");
                return;
            }
        }
        else if (received_message->mtype==5) {
            printf("STOP | client_index: %d\n", clients[client_idx].index);

            clients[client_idx].index = -1;
            clients[client_idx].status = false;
            clients[client_idx].que_id = 0;
            active_clients -= 1;
            if (active_clients == 0) {
                end_function();
            }
        }
    }

void listen() {
    Message received_message;
    clients = (Client*) calloc(MAX_NO_CLIENTS, sizeof(Client));

    while (!finished) {
        if (
                msgrcv(server_queue_id, &received_message, MESSAGE_LEN, 5, IPC_NOWAIT) != -1 ||
                msgrcv(server_queue_id, &received_message, MESSAGE_LEN, 4, IPC_NOWAIT) != -1 ||
                msgrcv(server_queue_id, &received_message, MESSAGE_LEN, 3, IPC_NOWAIT) != -1 ||
                msgrcv(server_queue_id, &received_message, MESSAGE_LEN, 2, IPC_NOWAIT) != -1 ||
                msgrcv(server_queue_id, &received_message, MESSAGE_LEN, 1, IPC_NOWAIT) != -1
                ){
            handle_function(&received_message);
        }
        else{
            sleep(1);
            continue;
        }
    }

}

int main(int argc, char** argv) {
    
    signal(SIGINT, end_function);
    FILE* log = fopen("log.txt", "w");
    char line[128];
    snprintf(line, 128, "    Czas zlecenia   |Indeks klienta|Typ zlecenia\n");
    fwrite(line, strlen(line), sizeof (char), log);
    fclose(log);
    server_queue_id = create_queue(argv[0], ftok_id, IPC_CREAT);
    
    listen();

    return 0;
}

