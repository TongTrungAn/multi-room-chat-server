#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stddef.h>
#include <sys/types.h>

ssize_t send_all(int socket_fd, const char *buffer, size_t length);
void trim_newline(char *str);
int is_empty_string(const char *str);

#endif