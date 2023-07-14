#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "common.h"

#define MAX_CLIENTS 16
#define PING_INTERVAL 50
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int epollFD;

ClientStruct clients[MAX_CLIENTS];

void deleteClient(ClientStruct *client) {
    printf("Deleting %s\n", client->nickname);
    client->state = 0;
    client->nickname[0] = 0;
    epoll_ctl(epollFD, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
}

char* getMessageTypeString(MessageType type) {
    switch (type) {
        case PING:
            return "PING";
        case USERNAME_TAKEN:
            return "USERNAME_TAKEN";
        case SERVER_FULL:
            return "SERVER_FULL";
        case GET:
            return "GET";
        case LIST:
            return "LIST";
        case TO_ONE:
            return "2ONE";
        case TO_ALL:
            return "2ALL";
        case STOP:
            return "STOP";
        default:
            return "UNKNOWN";
    }
}

void clientMessage(ClientStruct *client) {
    if (client->state == 2) {
        size_t nickLen = read(client->fd, client->nickname, sizeof client->nickname - 1);
        int clientIndex = (int) (client - clients);

        pthread_mutex_lock(&mutex);

        int idx = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (i != clientIndex && strncmp(client->nickname, clients[i].nickname, sizeof client->nickname) == 0) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            client->nickname[nickLen] = '\0';
            client->state = 1;
            printf("New client %s\n", client->nickname);
        } else {
            printf("Nickname %s is occupied\n", client->nickname);
            deleteClient(client);
        }
        pthread_mutex_unlock(&mutex);

    } else {
        message msg;
        read(client->fd, &msg, sizeof msg);

        time_t currentTime;
        struct tm *localTime;

        currentTime = time(NULL);

        localTime = localtime(&currentTime);
        char* messageTypeString = getMessageTypeString(msg.type);
        printf("Got message %s\n at %02d:%02d:%02d, %02d-%02d-%04d\n", messageTypeString, localTime->tm_hour, localTime->tm_min, localTime->tm_sec, localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);

        if (msg.type == PING) {
            pthread_mutex_lock(&mutex);
            printf("%s\n ", client->nickname);
            client->responding = true;
            pthread_mutex_unlock(&mutex);

        } else if (msg.type == STOP) {
            pthread_mutex_lock(&mutex);
            deleteClient(client);
            pthread_mutex_unlock(&mutex);
        } else if (msg.type == TO_ALL) {
            char out[256] = "";
            strcat(out, client->nickname);
            strcat(out, "send message\n ");
            strcat(out, msg.text);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].state != 0) {
                    message msg;
                    msg.type = GET;
                    memcpy(&msg.text, out, MESSAGE_LEN * sizeof(char));
                    write((clients+i)->fd, &msg, sizeof(msg));
                }
            }
        } else if (msg.type == LIST) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].state != 0) {
                    message msg;
                    msg.type = GET;
                    memcpy(&msg.text, clients[i].nickname, MESSAGE_LEN * sizeof(char));
                    write(client->fd, &msg, sizeof(msg));
                }
            }
        } else if (msg.type == TO_ONE) {
            char out[256] = "";

            strcat(out, client->nickname);
            strcat(out, " send message\n");
            strcat(out, msg.text);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].state != 0) {
                    if (strcmp(clients[i].nickname, msg.nickname) == 0) {
                        message msg;
                        msg.type = GET;
                        memcpy(&msg.text, out, MESSAGE_LEN * sizeof(char));
                        write((clients+i)->fd, &msg, sizeof(msg));
                    }
                }
            }
        }
    }
}


void initSocket(int socket, void *addr, int addr_size) {
    if (bind(socket, (struct sockaddr *) addr, addr_size) == -1) {
        fprintf(stderr, "bind, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (listen(socket, MAX_CLIENTS) == -1) {
        fprintf(stderr, "listen, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct epoll_event event = {.events = EPOLLIN | EPOLLPRI};

    EventDataStruct *event_data = event.data.ptr = malloc(sizeof *event_data);
    event_data->type = socket_event;
    event_data->socketData.socket = socket;

    epoll_ctl(epollFD, EPOLL_CTL_ADD, socket, &event);
}


ClientStruct *newClient(int client_fd) {
    pthread_mutex_lock(&mutex);

    int i = -1;
    for (int j = 0; j < MAX_CLIENTS; j++) {
        if (clients[j].state == 0) {
            i = j;
            break;
        }
    }

    if (i == -1) return NULL;

    ClientStruct *client = &clients[i];

    client->fd = client_fd;
    client->state = 2;
    client->responding = true;

    pthread_mutex_unlock(&mutex);

    return client;
}


void *ping() {
    message msg = {.type = PING};

    while (1) {
        sleep(PING_INTERVAL);
        pthread_mutex_lock(&mutex);
        printf("Pinging clients\n");

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != 0) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    write(clients[i].fd, &msg, sizeof msg);
                } else {
                    deleteClient(&clients[i]);
                }
            }
        }

        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong arguments. Usage: [port] [path]\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    char *socketPath = argv[2];

    epollFD = epoll_create1(0);
    if (epollFD == -1) {
        fprintf(stderr, "epoll_create1, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un localAddress = {.sun_family = AF_UNIX};
    strncpy(localAddress.sun_path, socketPath, sizeof localAddress.sun_path);

    struct sockaddr_in webAddress = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };

    unlink(socketPath);
    int localSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (localSock == -1) {
        fprintf(stderr, "socket, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    initSocket(localSock, &localAddress, sizeof localAddress);

    int webSock = socket(AF_INET, SOCK_STREAM, 0);
    if (webSock == -1) {
        fprintf(stderr, "socket, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    initSocket(webSock, &webAddress, sizeof webAddress);

    pthread_t pingThread;
    pthread_create(&pingThread, NULL, ping, NULL);

    printf("Server is listening on: %d, '%s'\n", port, socketPath);

    struct epoll_event events[10];

    while (1) {
        int n = epoll_wait(epollFD, events, 10, -1);
        if (n == -1) {
            fprintf(stderr, "epoll_wait, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {
            EventDataStruct *data = events[i].data.ptr;

            if (data->type == socket_event) {
                int clientFD = accept(data->socketData.socket, NULL, NULL);
                ClientStruct *client = newClient(clientFD);

                if (!client) {
                    printf("No free places\n");
                    message msg = {.type = SERVER_FULL};
                    write(clientFD, &msg, sizeof msg);
                    close(clientFD);
                    continue;
                }

                EventDataStruct *eData = malloc(sizeof(EventDataStruct));
                eData->type = client_event;
                eData->socketData.client = client;

                struct epoll_event event = {
                        .events = EPOLLIN | EPOLLET | EPOLLHUP,
                        .data = {eData}
                };

                if (epoll_ctl(epollFD, EPOLL_CTL_ADD, clientFD, &event) == -1) {
                    fprintf(stderr, "epoll_ctl, %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            } else if (data->type == client_event) {
                if (events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    deleteClient(data->socketData.client);
                    pthread_mutex_unlock(&mutex);
                } else {
                    clientMessage(data->socketData.client);
                }
            }
        }
    }
}