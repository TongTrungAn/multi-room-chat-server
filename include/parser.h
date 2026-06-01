#ifndef PARSER_H
#define PARSER_H

#define CMD_NORMAL  0
#define CMD_NICK    1
#define CMD_JOIN    2
#define CMD_LEAVE   3
#define CMD_LIST    4
#define CMD_WHO     5
#define CMD_MSG     6
#define CMD_QUIT    7
#define CMD_INVALID -1

int parse_command(const char *input);
int validate_command(const char *input);

#endif