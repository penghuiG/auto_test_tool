#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#define info_printf(msg,arg ...)  test_info_fprintf((char*)msg,## arg)
enum gl_status_t
{
    GL_STATE_OK,
    GL_STATE_FAIL,
    GL_NO_STARTUP,
    GL_SLEEP_FAIL,
    GL_WEAKUP_FAIL,
    GL_KEEP_ERR,
    GL_CURRENT_LOW,
    GL_TIMEOUT_ERR,
    GL_SINGLE_ERR,
    GL_ADB_CON_ERR,
    GL_PROCESS_QUERY_ERR,
    GL_DIAL_ERR
} ;

typedef struct _test_info_t
{
    int status;
    float cur_val;
    float avg_val;
    // int timer;
    // int power_range_status;
    int time_limit; // 时间限制
    int retry_flag;
    int curent_order;
    int succes_flag;
} test_info_t;
typedef struct _process_info
{
    char process_name[64][64];
    int err_process_num;
} process_info_t;

typedef enum
{
    PROCESS_QUERY = 1,
    CALL_NUMBRE
} adb_operation_t;

extern process_info_t process_info;

extern test_info_t test_info;

void log_clear();
int test_info_fprintf(char *msg,...);
