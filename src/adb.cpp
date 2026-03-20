
#include "adb.h"
#include "log.h"
#include "power_manage.h"
static gl_status_t process_query_func(char *input);

// process_info_t process_info = {0};
gl_status_t adb_shell_cmd(char * adb_cmd,adb_operation_t adb_operation)
{
    
    char cmd[ADB_CMD_LENGTH] = {0};
    gl_status_t adb_result = GL_STATE_FAILED;
    char out[ADB_CMD_OUT_LENGTH] = {0};
    FILE *fp = NULL;
    int i =0,err_num = 0,is_cmd_ok = 0;
    memset(&process_info,0,sizeof(process_info));
    sprintf(cmd,"adb shell \" %s\"",adb_cmd);
    printf("%s\n",cmd);
    fp = popen(cmd,"r");
    //如果adb shell不成功,就进不了下面这个while
    while (fgets(out,ADB_CMD_OUT_LENGTH,fp) > (char*)0)
    {   
        ++is_cmd_ok;
        
        if (PROCESS_QUERY == adb_operation)
        {
            adb_result = process_query_func(out);
            /* 错了之后的处理逻辑 */
            if (adb_result != GL_STATE_OK)
            {
                ++err_num;
                strcpy(process_info.process_name[process_info.err_process_num++],out);
            }
                
        }
        memset(out,0,ADB_CMD_OUT_LENGTH);
    }    
    pclose(fp);
    if (err_num && is_cmd_ok)
    {
        adb_result = GL_PROCESS_QUERY_ERR;
    }
    if (!err_num && is_cmd_ok)
    {
        adb_result = GL_STATE_OK;
    }
    return adb_result;
}

static gl_status_t process_query_func(char *input)
{
    char * p;
    char temp[128] = {0};
    int adb_result = 0;
    char haha[256] = {0};
    int err_num = 0;
    p = input;memset(temp,0,strlen(temp));
    for (int j=0;j<strlen(input);j++)
    {
        if (input[j] != '\n')temp[j] = input[j]; 
    }
    sprintf(haha,"adb shell \"ps -ef|grep /oemapp/bin/%s|grep -v grep\"",temp);
    adb_result = system(haha);
    if (adb_result == 256)
    {
        log_error("%s is not started\n",temp);
        ++err_num;
    }
    else
    {}
    if (err_num)
    {
        return GL_PROCESS_QUERY_ERR;
    }
    return GL_STATE_OK;
}

adb_dev::adb_dev()
{
    adb_connected_flag = false;
}
bool adb_dev::connect(int timeout)
{
    bool connect_status = false;
    if (timeout < 0)
    {
        return connect_status;
    }
    while (timeout--)
    {
        if ( 0 == system("adb root"))
        {
            connect_status = true;
            adb_connected_flag = true;
            break;
        }
        else
        {
            adb_connected_flag = false;
        }
        sleep(1);
    }
    return connect_status;
}
bool adb_dev::get_adb_connect_status()
{
    if (0 == system("adb root"))
    {
        adb_connected_flag = true;
        return true;
    }
    adb_connected_flag = false;
    return false;
}

bool adb_dev::is_connect()
{
    return adb_connected_flag;
}


int adb_dev::edl_enter()
{
    bool adb_connected_flag = false;
    adb_connected_flag = connect(30);
    if (!adb_connected_flag )
    {
        return -1;
    }
    sleep(15);
    if (system("adb shell \"reboot edl\"") == 0)
    {
        log_debug("adb shell \"reboot edl\"");
    }
    sleep(30);
    if (system("lsusb |grep 9008") == 0)
    {
        log_debug("Successfully entered 9008 mode");
        return 0;
    }
    return -1;
}
int adb_dev::edl_out()
{
    int counter = 30;
    power_restart();
    while (--counter)
    {
        if (system("adb root") == 0)
        {
            return 0;
        }
        sleep(1);
    }
    return -1;
}