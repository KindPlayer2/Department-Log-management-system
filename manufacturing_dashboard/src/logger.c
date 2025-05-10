#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include "config.h"

void log_error(const char *format, ...) 
{
    va_list args;
    va_start(args, format);
    FILE *fp = fopen(ERROR_LOG, "a");
    if (!fp) return;
    fprintf(fp, "[ERROR] ");
    vfprintf(fp, format, args);
    fprintf(fp, "\n");
    fclose(fp);
    va_end(args);
}

void log_info(const char *format, ...) 
{
    va_list args;
    va_start(args, format);
    FILE *fp = fopen(CHANGE_LOG, "a");
    if (!fp) return;
    fprintf(fp, "[INFO] ");
    vfprintf(fp, format, args);
    fprintf(fp, "\n");
    fclose(fp);
    va_end(args);
}

void log_change(const char *filename) 
{
    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));

    FILE *fp = fopen(CHANGE_LOG, "a");
    if (!fp) return;
    fprintf(fp, "[%s] File changed: %s\n", timestamp, filename);
    fclose(fp);
}