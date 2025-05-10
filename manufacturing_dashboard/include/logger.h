#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// Function declarations
void log_error(const char *format, ...);
void log_info(const char *format, ...);
void log_change(const char *filename);

#endif