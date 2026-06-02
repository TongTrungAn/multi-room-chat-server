#include <stdio.h>
#include <string.h>
#include "parser.h"

static void clean_input(const char *input, char *output, size_t size)
{
    if (input == NULL || output == NULL || size == 0)
        return;

    strncpy(output, input, size - 1);
    output[size - 1] = '\0';

    output[strcspn(output, "\r\n")] = '\0';

    while (strlen(output) > 0 && output[strlen(output) - 1] == ' ')
    {
        output[strlen(output) - 1] = '\0';
    }
}

int parse_command(const char *input)
{
    char cmd_input[512];

    clean_input(input, cmd_input, sizeof(cmd_input));

    if (strlen(cmd_input) == 0)
        return CMD_INVALID;

    if (cmd_input[0] != '/')
        return CMD_NORMAL;

    if (strncmp(cmd_input, "/nick ", 6) == 0)
        return CMD_NICK;

    if (strncmp(cmd_input, "/join ", 6) == 0)
        return CMD_JOIN;

    if (strcmp(cmd_input, "/leave") == 0)
        return CMD_LEAVE;

    if (strcmp(cmd_input, "/list") == 0)
        return CMD_LIST;

    if (strcmp(cmd_input, "/who") == 0)
        return CMD_WHO;

    if (strncmp(cmd_input, "/msg ", 5) == 0)
        return CMD_MSG;

    if (strcmp(cmd_input, "/quit") == 0)
        return CMD_QUIT;

    return CMD_INVALID;
}

int validate_command(const char *input)
{
    char cmd_input[512];

    clean_input(input, cmd_input, sizeof(cmd_input));

    int cmd = parse_command(cmd_input);

    if (cmd == CMD_INVALID)
        return 0;

    if (cmd == CMD_NORMAL)
        return 1;

    if (cmd == CMD_NICK)
        return strlen(cmd_input) > 6;

    if (cmd == CMD_JOIN)
        return strlen(cmd_input) > 6;

    if (cmd == CMD_MSG)
    {
        char user[32];
        char message[256];

        return sscanf(cmd_input, "/msg %31s %255[^\n]", user, message) == 2;
    }

    if (cmd == CMD_LEAVE ||
        cmd == CMD_LIST ||
        cmd == CMD_WHO ||
        cmd == CMD_QUIT)
        return 1;

    return 0;
}
