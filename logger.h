#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

void log_init(const char *logfile, LogLevel min_level);
void log_shutdown(void);
void log_set_level(LogLevel level);

// Macros para capturar arquivo e linha automaticamente
#define log_debug(fmt, ...) log_write(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) log_write(LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) log_write(LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log_write(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

// Função interna
void log_write(LogLevel level, const char *file, int line, const char *fmt, ...);

#endif // LOGGER_H
