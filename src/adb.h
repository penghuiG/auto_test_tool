#pragma once
#include "power_manage.h"
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
extern "C"
{
#include "common.h"
}

#define ADB_CMD_LENGTH 512
#define ADB_CMD_OUT_LENGTH 512

class adb_dev
{
public:
    adb_dev();
    ~adb_dev()
    {
        power_gpio_set(1,1,1,1);
    }
    bool connect(int timeout);
    bool is_connect();
    int edl_enter();
    int edl_out();
private:
    bool adb_connected_flag;
};



gl_status_t adb_shell_cmd(char * adb_cmd,adb_operation_t adb_operation);
int edl_enter();
int edl_out();
