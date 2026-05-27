#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <netinet/in.h>

#define DEFAULT_PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_NAME_LEN 32
#define MAX_ROOM_LEN 32
#define SERVER_BACKLOG 10

typedef struct Client
{
    int socket_fd;
    char nickname[MAX_NAME_LEN];
    char room[MAX_ROOM_LEN];
    struct sockaddr_in address;
    pthread_t thread;
    int active;
} Client;

extern Client clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;

int start_server(int port);
int add_client(int client_fd, struct sockaddr_in client_addr);
void remove_client(Client *client);
void cleanup_server(int server_fd);

#endif