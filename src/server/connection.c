#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../../include/server.h"
#include "../../include/connection.h"
#include "../../include/logger.h"

int create_server_socket(int port)
{
    int server_fd;
    int opt = 1;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        log_error("socket() failed");
        return -1;
    }

    if (setsockopt(
            server_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt)) < 0)
    {
        log_error("setsockopt() failed");
        close(server_fd);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(
            server_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0)
    {
        log_error("bind() failed. Port may already be in use");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, SERVER_BACKLOG) < 0)
    {
        log_error("listen() failed");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int accept_client(int server_fd, struct sockaddr_in *client_addr)
{
    socklen_t client_len = sizeof(struct sockaddr_in);

    int client_fd = accept(
        server_fd,
        (struct sockaddr *)client_addr,
        &client_len);

    if (client_fd < 0)
    {
        if (errno != EINTR)
        {
            log_error("accept() failed");
        }

        return -1;
    }

    log_info(
        "New connection from %s:%d",
        inet_ntoa(client_addr->sin_addr),
        ntohs(client_addr->sin_port));

    return client_fd;
}