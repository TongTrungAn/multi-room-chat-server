#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>

#include "../../include/server.h"
#include "../../include/connection.h"
#include "../../include/thread_manager.h"
#include "../../include/logger.h"
#include "../../include/network_utils.h"

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static volatile sig_atomic_t server_running = 1;
static int server_socket_fd = -1;

static void handle_signal(int signal_number)
{
    if (signal_number == SIGINT || signal_number == SIGTERM)
    {
        server_running = 0;

        if (server_socket_fd >= 0)
        {
            close(server_socket_fd);
        }
    }
}

int add_client(int client_fd, struct sockaddr_in client_addr)
{
    pthread_mutex_lock(&clients_mutex);

    int index = -1;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!clients[i].active)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        pthread_mutex_unlock(&clients_mutex);
        return -1;
    }

    clients[index].socket_fd = client_fd;
    clients[index].address = client_addr;
    clients[index].active = 1;

    snprintf(
        clients[index].nickname,
        MAX_NAME_LEN,
        "User%d",
        index + 1);

    snprintf(
        clients[index].room,
        MAX_ROOM_LEN,
        "lobby");

    pthread_mutex_unlock(&clients_mutex);

    return index;
}

void remove_client(Client *client)
{
    if (client == NULL)
    {
        return;
    }

    pthread_mutex_lock(&clients_mutex);

    if (client->active)
    {
        close(client->socket_fd);
        client->socket_fd = -1;
        client->active = 0;
    }

    pthread_mutex_unlock(&clients_mutex);
}

void cleanup_server(int server_fd)
{
    log_info("Cleaning up server resources");

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active)
        {
            close(clients[i].socket_fd);
            clients[i].socket_fd = -1;
            clients[i].active = 0;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    if (server_fd >= 0)
    {
        close(server_fd);
    }

    pthread_mutex_destroy(&clients_mutex);

    log_info("Server stopped");
}

int start_server(int port)
{
    struct sockaddr_in client_addr;

    memset(clients, 0, sizeof(clients));

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    server_socket_fd = create_server_socket(port);

    if (server_socket_fd < 0)
    {
        log_error("Failed to start server");
        return 1;
    }

    log_info("Multi-Room Chat Server started on port %d", port);
    log_info("Waiting for clients...");

    while (server_running)
    {
        int client_fd = accept_client(server_socket_fd, &client_addr);

        if (client_fd < 0)
        {
            if (!server_running)
            {
                break;
            }

            continue;
        }

        int client_index = add_client(client_fd, client_addr);

        if (client_index < 0)
        {
            const char *full_msg = "Server is full. Please try again later.\n";
            send_all(client_fd, full_msg, strlen(full_msg));
            close(client_fd);
            log_error("Client rejected because server is full");
            continue;
        }

        if (pthread_create(
                &clients[client_index].thread,
                NULL,
                client_thread,
                (void *)&clients[client_index]) != 0)
        {
            log_error("pthread_create() failed");
            remove_client(&clients[client_index]);
            continue;
        }

        pthread_detach(clients[client_index].thread);

        log_info(
            "Client accepted as %s in room %s",
            clients[client_index].nickname,
            clients[client_index].room);
    }

    cleanup_server(server_socket_fd);

    return 0;
}

int main(int argc, char *argv[])
{
    int port = DEFAULT_PORT;

    if (argc == 2)
    {
        port = atoi(argv[1]);

        if (port <= 0 || port > 65535)
        {
            printf("Invalid port number.\n");
            return 1;
        }
    }

    return start_server(port);
}