#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "../include/logger.h"

static void print_time()
{
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);

    if (time_info != NULL)
    {
        printf("[%02d:%02d:%02d] ",
               time_info->tm_hour,
               time_info->tm_min,
               time_info->tm_sec);
    }
}

void log_info(const char *format, ...)
{
    va_list args;

    print_time();
    printf("[INFO] ");

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

void log_error(const char *format, ...)
{
    va_list args;

    print_time();
    printf("[ERROR] ");

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}