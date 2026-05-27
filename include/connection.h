#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>

int create_server_socket(int port);
int accept_client(int server_fd, struct sockaddr_in *client_addr);

#endif