#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static FILE* log_file = NULL;
static LogLevel current_level = LOG_INFO;
static const char* level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};

void log_init(const char* logfile, LogLevel min_level) {
    log_file = fopen(logfile, "a");
    current_level = min_level;
}

void log_shutdown(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_set_level(LogLevel level) {
    current_level = level;
}

static void log_write(LogLevel level, const char* fmt, va_list args) {
    if (level < current_level) return;
    
    time_t now = time(NULL);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[24] = '\0';  // Remove newline
    
    // Write to file
    if (log_file) {
        fprintf(log_file, "[%s][%s] ", timestamp, level_strings[level]);
        vfprintf(log_file, fmt, args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
    
    // Also write to stdout
    printf("[%s][%s] ", timestamp, level_strings[level]);
    vprintf(fmt, args);
    printf("\n");
}

void log_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_write(LOG_DEBUG, fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_write(LOG_INFO, fmt, args);
    va_end(args);
}

void log_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_write(LOG_WARN, fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_write(LOG_ERROR, fmt, args);
    va_end(args);
}
