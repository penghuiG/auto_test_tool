#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#define LOG_MAX_LEN 4096

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_TYPE_MAX
} LogLevel;

// #define out_log(save,msg,arg ...)  _out_log((int)save,(char*)msg,## arg)
// #define out_log(save, msg, ...)  _out_log((int)save, (char*)msg, ##__VA_ARGS__)

// int _out_log(int save, char *msg,...);
void wait_in_place(int time);
void log_init(const char *filename);
void log_close() ;
void log_clear(std::string filename);
void log_internal(LogLevel level, const char *format, ...) ;
// 宏封装（简化调用）
#define log_debug(fmt, ...)   log_internal(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    log_internal(LOG_INFO, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)    log_internal(LOG_WARN, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   log_internal(LOG_ERROR, fmt, ##__VA_ARGS__)

