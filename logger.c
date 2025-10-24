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

void log_write(LogLevel level, const char *file, int line, const char *fmt, ...)
{
    if (level < current_level)
        return;

    time_t now = time(NULL);
    struct tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &now);
#else
    localtime_r(&now, &tm_now);
#endif
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_now);

    va_list args;
    va_start(args, fmt);

    // Write to file
    if (log_file)
    {
        fprintf(log_file, "[%s][%s][%s:%d] ", timestamp, level_strings[level], file, line);
        vfprintf(log_file, fmt, args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }

    // Write to console with color
    printf("%s[%s][%s][%s:%d]%s ", level_colors[level], timestamp, level_strings[level], file, line, color_reset);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}
