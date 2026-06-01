#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>

#include "../../include/server.h"
#include "../../include/thread_manager.h"
#include "../../include/network_utils.h"
#include "../../include/logger.h"

static void send_to_client(Client *client, const char *format, ...)
{
    char buffer[BUFFER_SIZE];
    va_list args;

    if (client == NULL || !client->active)
    {
        return;
    }

    memset(buffer, 0, sizeof(buffer));

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    send_all(client->socket_fd, buffer, strlen(buffer));
}
static void clean_token(char *str)
{
    if (str == NULL)
    {
        return;
    }

    str[strcspn(str, "\r\n")] = '\0';

    int write_index = 0;

    for (int read_index = 0; str[read_index] != '\0'; read_index++)
    {
        unsigned char c = (unsigned char)str[read_index];

        if (c >= 32 && c <= 126)
        {
            str[write_index++] = str[read_index];
        }
    }

    str[write_index] = '\0';
}

static int nickname_exists(const char *nickname, Client *current_client)
{
    int exists = 0;

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active &&
            &clients[i] != current_client &&
            strcmp(clients[i].nickname, nickname) == 0)
        {
            exists = 1;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    return exists;
}

static void broadcast_to_room(Client *sender, const char *message)
{
    char formatted[BUFFER_SIZE + MAX_NAME_LEN + MAX_ROOM_LEN + 32];

    snprintf(
        formatted,
        sizeof(formatted),
        "[%s][%s]: %s\n",
        sender->room,
        sender->nickname,
        message);

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active &&
            clients[i].socket_fd != sender->socket_fd &&
            strcmp(clients[i].room, sender->room) == 0)
        {
            send_all(
                clients[i].socket_fd,
                formatted,
                strlen(formatted));
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

static void broadcast_system_to_room(Client *sender, const char *message)
{
    char formatted[BUFFER_SIZE + MAX_NAME_LEN + MAX_ROOM_LEN + 64];

    snprintf(
        formatted,
        sizeof(formatted),
        "[SERVER][%s]: %s\n",
        sender->room,
        message);

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active &&
            clients[i].socket_fd != sender->socket_fd &&
            strcmp(clients[i].room, sender->room) == 0)
        {
            send_all(
                clients[i].socket_fd,
                formatted,
                strlen(formatted));
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

static void list_rooms(Client *client)
{
    char sent_rooms[MAX_CLIENTS][MAX_ROOM_LEN];
    int room_count = 0;
    char buffer[BUFFER_SIZE];

    send_to_client(client, "Available rooms:\n");

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!clients[i].active)
        {
            continue;
        }

        int already_sent = 0;

        for (int j = 0; j < room_count; j++)
        {
            if (strcmp(sent_rooms[j], clients[i].room) == 0)
            {
                already_sent = 1;
                break;
            }
        }

        if (!already_sent)
        {
            snprintf(
                sent_rooms[room_count],
                MAX_ROOM_LEN,
                "%s",
                clients[i].room);

            snprintf(
                buffer,
                sizeof(buffer),
                "- %s\n",
                clients[i].room);

            send_all(client->socket_fd, buffer, strlen(buffer));
            room_count++;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

static void show_users_in_room(Client *client)
{
    char buffer[BUFFER_SIZE];

    send_to_client(client, "Users in room [%s]:\n", client->room);

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active &&
            strcmp(clients[i].room, client->room) == 0)
        {
            snprintf(
                buffer,
                sizeof(buffer),
                "- %s\n",
                clients[i].nickname);

            send_all(client->socket_fd, buffer, strlen(buffer));
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

static void private_message(Client *sender, const char *target_name, const char *text)
{
    char buffer[BUFFER_SIZE + MAX_NAME_LEN + 64];
    int found = 0;

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active &&
            strcmp(clients[i].nickname, target_name) == 0)
        {
            snprintf(
                buffer,
                sizeof(buffer),
                "[Private from %s]: %s\n",
                sender->nickname,
                text);

            send_all(clients[i].socket_fd, buffer, strlen(buffer));
            found = 1;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    if (!found)
    {
        send_to_client(sender, "[ERROR] User '%s' not found.\n", target_name);
    }
    else
    {
        send_to_client(sender, "[SERVER] Private message sent to %s.\n", target_name);
    }
}

static int process_command(Client *client, char *buffer)
{
    trim_newline(buffer);

    if (is_empty_string(buffer))
    {
        return 1;
    }

    if (strncmp(buffer, "/nick ", 6) == 0)
    {
        char new_name[MAX_NAME_LEN];

        memset(new_name, 0, sizeof(new_name));

        if (sscanf(buffer + 6, "%31s", new_name) != 1)
        {
            send_to_client(client, "[ERROR] Usage: /nick <name>\n");
            return 1;
        }
        clean_token(new_name);

        if (nickname_exists(new_name, client))
        {
            send_to_client(client, "[ERROR] Nickname already exists.\n");
            return 1;
        }

        pthread_mutex_lock(&clients_mutex);
        snprintf(client->nickname, MAX_NAME_LEN, "%s", new_name);
        pthread_mutex_unlock(&clients_mutex);

        send_to_client(client, "[SERVER] Nickname changed to %s.\n", new_name);
        return 1;
    }

    if (strncmp(buffer, "/join ", 6) == 0)
    {
        char new_room[MAX_ROOM_LEN];
        char message[BUFFER_SIZE];

        memset(new_room, 0, sizeof(new_room));

       if (sscanf(buffer + 6, "%31s", new_room) != 1)
        {
            send_to_client(client, "[ERROR] Usage: /join <room>\n");
            return 1;
        }

        clean_token(new_room);

        pthread_mutex_lock(&clients_mutex);
        snprintf(client->room, MAX_ROOM_LEN, "%s", new_room);
        pthread_mutex_unlock(&clients_mutex);

        send_to_client(client, "[SERVER] You joined room [%s].\n", new_room);

        snprintf(
            message,
            sizeof(message),
            "%s has joined room [%s].",
            client->nickname,
            new_room);

        broadcast_system_to_room(client, message);

        return 1;
    }

    if (strcmp(buffer, "/leave") == 0)
    {
        pthread_mutex_lock(&clients_mutex);
        snprintf(client->room, MAX_ROOM_LEN, "lobby");
        pthread_mutex_unlock(&clients_mutex);

        send_to_client(client, "[SERVER] You returned to lobby.\n");
        return 1;
    }

    if (strcmp(buffer, "/list") == 0)
    {
        list_rooms(client);
        return 1;
    }

    if (strcmp(buffer, "/who") == 0)
    {
        show_users_in_room(client);
        return 1;
    }

    if (strncmp(buffer, "/msg ", 5) == 0)
    {
        char *content = buffer + 5;
        char *space = strchr(content, ' ');

        if (space == NULL)
        {
            send_to_client(client, "[ERROR] Usage: /msg <user> <message>\n");
            return 1;
        }

        *space = '\0';

        char *target_name = content;
        char *message_text = space + 1;

        clean_token(target_name);
        clean_token(message_text);

        if (is_empty_string(target_name) || is_empty_string(message_text))
        {
            send_to_client(client, "[ERROR] Usage: /msg <user> <message>\n");
            return 1;
        }

        private_message(client, target_name, message_text);
        return 1;
    }

    if (strcmp(buffer, "/quit") == 0)
    {
        send_to_client(client, "[SERVER] Goodbye!\n");
        return 0;
    }

    if (buffer[0] == '/')
    {
        send_to_client(client, "[ERROR] Unknown command.\n");
        return 1;
    }

    broadcast_to_room(client, buffer);
    return 1;
}

void *client_thread(void *arg)
{
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];

    send_to_client(client, "Welcome to Multi-Room Chat Server!\n");
    send_to_client(client, "Your nickname is %s.\n", client->nickname);
    send_to_client(client, "You are currently in room [%s].\n", client->room);
    send_to_client(client, "Commands:\n");
    send_to_client(client, "  /nick <name>\n");
    send_to_client(client, "  /join <room>\n");
    send_to_client(client, "  /leave\n");
    send_to_client(client, "  /list\n");
    send_to_client(client, "  /who\n");
    send_to_client(client, "  /msg <user> <message>\n");
    send_to_client(client, "  /quit\n");

    log_info("Thread started for client %s", client->nickname);

    while (client->active)
    {
        memset(buffer, 0, sizeof(buffer));

        ssize_t bytes_received = recv(
            client->socket_fd,
            buffer,
            sizeof(buffer) - 1,
            0);

        if (bytes_received <= 0)
        {
            log_info("Client disconnected: %s", client->nickname);
            break;
        }

        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        int should_continue = process_command(client, buffer);

        if (!should_continue)
        {
            break;
        }
    }

    log_info("Cleaning up client %s", client->nickname);
    remove_client(client);

    return NULL;
}