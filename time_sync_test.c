#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#define DEVICE_ADDR  0x10

#define RELAY1  0x01
#define RELAY2  0x02
#define RELAY3  0x03
#define RELAY4  0x04

#define ON      0xFF
#define OFF     0x00

#define GPIO_B    RELAY1
#define GPIO_IG   RELAY2
#define GPIO_ACC  RELAY3
#define GPIO_USB_POWER RELAY4

#define GPIO_MAX_NUM 4

#define LOG_MAX_LEN 4096

void gpio_write(int pin, int val);
void power_manage_init();
void power_gpio_set(int bat, int ig, int acc, int usb);

typedef enum {
    LOG_DEBUG, 
    LOG_INFO, 
    LOG_WARN, 
    LOG_ERROR, 
    LOG_TYPE_MAX
} LogLevel;

// #define out_log(save, msg, arg ...)  _out_log((int)save, (char*)msg, ## arg)
// #define out_log(save, msg, ...)  _out_log((int)save, (char*)msg, ##__VA_ARGS__)

// int _out_log(int save, char *msg, ...);
void wait_in_place(int time);
void log_init(const char *filename);
void log_close() ;
void log_internal(LogLevel level, const char *format, ...) ;
// 宏封装（简化调用）
#define log_debug(fmt, ...)   log_internal(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    log_internal(LOG_INFO, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)    log_internal(LOG_WARN, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   log_internal(LOG_ERROR, fmt, ##__VA_ARGS__)

static FILE *log_file = NULL;

void log_init(const char *filename) {
    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("Failed to open log file");
        exit(1);
    }
}

void log_close() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_internal(LogLevel level, const char *format, ...) {
    // time_t now;
    // time(&now);
    // char *time_str = ctime(&now);
    // time_str[strlen(time_str) - 1] = '\0';
//
    char time_str[LOG_MAX_LEN] = {0};

    time_t current_time = time(NULL);
    struct tm *tm       = NULL;
  
    tm = localtime(&current_time);
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
      (tm->tm_hour)%24, tm->tm_min, tm->tm_sec);
//
    const char *level_str;
    switch (level) {
        case LOG_DEBUG: level_str = "DEBUG"; break;
        case LOG_INFO:  level_str = "INFO";  break;
        case LOG_WARN:  level_str = "WARN";  break;
        case LOG_ERROR: level_str = "ERROR"; break;
        default:        level_str = "UNKNOWN";
    }

    printf("[%s] [%s] ", time_str, level_str);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");

    if (log_file) {
        fprintf(log_file, "[%s] [%s] ", time_str, level_str);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        fflush(log_file); 
    }
}


int fd;

void power_manage_init()
{
    fd = wiringPiI2CSetup(DEVICE_ADDR);
}
void gpio_write(int pin, int val)
{
    if (pin < 0 || val < 0)
    {
        return;
    }
    wiringPiI2CWriteReg8(fd, pin, val);
}
void power_gpio_set(int bat, int ig, int acc, int usb)
{
    if (bat)
    {
        gpio_write(GPIO_B, OFF);
    }
    else
    {
        gpio_write(GPIO_B, ON); 
    }

    if (ig)
    {
        gpio_write(GPIO_IG, OFF);
    }
    else
    {
        gpio_write(GPIO_IG, ON); 
    }

    if (acc)
    {
        gpio_write(GPIO_ACC, OFF);
    }
    else
    {
        gpio_write(GPIO_ACC, ON); 
    }

    if (usb)
    {
        gpio_write(GPIO_USB_POWER, OFF);
    }
    else
    {
        gpio_write(GPIO_USB_POWER, ON); 
    }
}
int usb_reset()
{
    gpio_write(GPIO_USB_POWER, ON);
    sleep(1);
    gpio_write(GPIO_USB_POWER, OFF);
}

void wait_in_place(int time)
{
  if (time <= 0)
  {
    return ;
  }
//   log_debug("Wait for %d seconds\n", time);
  sleep(time);
}
int connect(int timeout)
{
    int connect = -1;
    if (timeout < 0)
    {
        return -1;
    }
    while (timeout--)
    {
        if ( 0 == system("adb root"))
        {
            connect = 0;
            break;
        }
        else
        {
            usb_reset();
        }
        sleep(1);
    }
    return connect;
}

int sync_statu_get(int timeout)
{
    int connect = -1;
    if (timeout < 0)
    {
        return -1;
    }
    while (timeout--)
    {
        if ( 0 == system("adb shell \"date|grep 2025\""))
        {
            connect = 0;
            break;
        }
        else
        {
            usb_reset();
        }
        sleep(1);
    }
    return connect;
}
int info_get(char * cmd)
{
    FILE *fp;
    char out[256] = {0};
    fp = popen(cmd, "r");
    while (fgets(out, 256, fp) > (char*)0)
    {
        log_debug("%s", out);
        memset(out, 0, 256);
    }
    fclose(fp);
    return -1;
}

int main()
{
    int i = 0;
    log_init("/home/cx/auto_test_test/time_sync_test.log");
    power_manage_init();
    while (1)
    {
        log_debug("******************************** %03d ********************************", i++);
        power_gpio_set(0, 0, 0, 0);
        sleep(1);
        power_gpio_set(1, 1, 1, 1);
        system("./start.sh 3BWT JP EEU");
        if (connect(60) != 0)
        {

        }

        if (sync_statu_get(150) == 0)
        {
            log_debug("time sync SUCCESS ");
            info_get("adb shell \"date \"");
            info_get("adb shell \"cat /tmp/ql_time_set_flag \"");
            info_get("adb shell \"cat /tmp/ql_time_sync_record_flag\"");
            
        }
        else
        {
            
            log_error("time sync ERROR");
            info_get("adb shell \"date \"");
            info_get("adb shell \"cat /tmp/ql_time_set_flag \"");
            info_get("adb shell \"cat /tmp/ql_time_sync_record_flag\"");
            break;
            
        }
    }
}