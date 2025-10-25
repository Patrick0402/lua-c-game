#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

static FILE *log_file = NULL;
static LogLevel current_level = LOG_INFO;

static const char *level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};
static const char *level_colors[] = {"\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"}; // Cyan, Green, Yellow, Red
static const char *color_reset = "\x1b[0m";

void log_init(const char *logfile, LogLevel min_level)
{
    log_file = fopen(logfile, "a");
    current_level = min_level;
}

void log_shutdown(void)
{
    if (log_file)
    {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_set_level(LogLevel level)
{
    current_level = level;
}

static void log_vwrite(LogLevel level, const char *file, int line, const char *fmt, va_list args)
{
    if (level < current_level)
        return;

    // Timestamp
    char timestamp[20];
    time_t now = time(NULL);
    struct tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &now);
#else
    localtime_r(&now, &tm_now);
#endif
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_now);

    // Monta a mensagem final
    char message[1024];
    vsnprintf(message, sizeof(message), fmt, args);

    // Escreve no arquivo
    if (log_file)
        fprintf(log_file, "[%s][%s][%s:%d] %s\n", timestamp, level_strings[level], file, line, message);

    // Escreve no console com cor
    printf("%s[%s][%s][%s:%d]%s %s\n", level_colors[level], timestamp, level_strings[level], file, line, color_reset, message);
}

void log_write(LogLevel level, const char *file, int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_vwrite(level, file, line, fmt, args);
    va_end(args);
}
