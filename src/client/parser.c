#include <stdio.h>
#include <string.h>
#include "parser.h"

int parse_command(const char *input)
{
    if (input == NULL || strlen(input) == 0)
        return CMD_INVALID;

    if (input[0] != '/')
        return CMD_NORMAL;

    if (strncmp(input, "/nick ", 6) == 0)
        return CMD_NICK;

    if (strncmp(input, "/join ", 6) == 0)
        return CMD_JOIN;

    if (strcmp(input, "/leave") == 0)
        return CMD_LEAVE;

    if (strcmp(input, "/list") == 0)
        return CMD_LIST;

    if (strcmp(input, "/who") == 0)
        return CMD_WHO;

    if (strncmp(input, "/msg ", 5) == 0)
        return CMD_MSG;

    if (strcmp(input, "/quit") == 0)
        return CMD_QUIT;

    return CMD_INVALID;
}

int validate_command(const char *input)
{
    int cmd = parse_command(input);

    if (cmd == CMD_INVALID)
        return 0;

    if (cmd == CMD_NORMAL)
        return 1;

    if (cmd == CMD_NICK)
        return strlen(input) > 6;

    if (cmd == CMD_JOIN)
        return strlen(input) > 6;

    if (cmd == CMD_MSG)
    {
        char user[32];
        char message[256];

        return sscanf(input, "/msg %31s %255[^\n]", user, message) == 2;
    }

    if (cmd == CMD_LEAVE ||
        cmd == CMD_LIST ||
        cmd == CMD_WHO ||
        cmd == CMD_QUIT)
        return 1;

    return 0;
}