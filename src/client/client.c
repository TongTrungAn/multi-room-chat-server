#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int client_socket;

void *receive_messages(void *arg)
{
    (void)arg;
    char buffer[BUFFER_SIZE];

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0)
        {
            printf("\nDisconnected from server.\n");
            close(client_socket);
            exit(0);
        }

        buffer[bytes_received] = '\0';

        printf("\n%s\n", buffer);
        printf("> ");
        fflush(stdout);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_addr;
    pthread_t recv_thread;
    char input[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(client_socket);
        return 1;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    printf("Connected to server %s:%d\n", server_ip, port);

    if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0)
    {
        perror("Failed to create receive thread");
        close(client_socket);
        return 1;
    }

    while (1)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(input, BUFFER_SIZE, stdin) == NULL)
            break;

        input[strcspn(input, "\r\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        send(client_socket, input, strlen(input), 0);

        if (strcmp(input, "/quit") == 0)
            break;
    }

    close(client_socket);
    return 0;
}