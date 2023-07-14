#define MESSAGE_LEN 256
#define NICKNAME_LEN 16

typedef enum {
    PING,
    USERNAME_TAKEN,
    SERVER_FULL,
    GET,
    LIST,
    TO_ONE,
    TO_ALL,
    STOP,
} MessageType;

typedef struct {
    MessageType type;
    char nickname[NICKNAME_LEN];
    char text[MESSAGE_LEN];
} message;


typedef struct {
    int fd;
    int state;
    char nickname[NICKNAME_LEN];
    char symbol;
    bool responding;
} ClientStruct;

typedef enum {
    socket_event, client_event
} EventTypeEnum;

typedef union {
    ClientStruct *client;
    int socket;
} socketDataUnion;

typedef struct {
    EventTypeEnum type;
    socketDataUnion socketData;
} EventDataStruct;

