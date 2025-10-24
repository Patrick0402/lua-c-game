#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

void log_init(const char* logfile, LogLevel min_level);
void log_shutdown(void);
void log_set_level(LogLevel level);
void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warn(const char* fmt, ...);
void log_error(const char* fmt, ...);

#endif // LOGGER_H
