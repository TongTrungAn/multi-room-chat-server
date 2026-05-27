#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>

#include "../../include/network_utils.h"

ssize_t send_all(int socket_fd, const char *buffer, size_t length)
{
    size_t total_sent = 0;

    while (total_sent < length)
    {
        ssize_t sent = send(
            socket_fd,
            buffer + total_sent,
            length - total_sent,
            0);

        if (sent < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        if (sent == 0)
        {
            break;
        }

        total_sent += sent;
    }

    return (ssize_t)total_sent;
}

void trim_newline(char *str)
{
    if (str == NULL)
    {
        return;
    }

    size_t len = strlen(str);

    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
        len--;
    }
}

int is_empty_string(const char *str)
{
    if (str == NULL)
    {
        return 1;
    }

    while (*str != '\0')
    {
        if (!isspace((unsigned char)*str))
        {
            return 0;
        }

        str++;
    }

    return 1;
}