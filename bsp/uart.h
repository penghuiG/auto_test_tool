#ifndef UARTH_H_
#define UARTH_H_


#include <wiringSerial.h>
#include <iostream>
#include <string>
#include "common.h"

#define MOVE_AVERAGE_LENGTH  100  //10秒内的平均值
class uart_dev
{
public:
    int fd;
    uart_dev();
    ~uart_dev();
    std::string dev_name;
    int get_dev_name_by_id();
    int uart_init();
    int uart_init(std::string dev);
    int uart_init(char* dev);
private:
    int baud_rate;
};

int uart_init(char * dev);
void uartHandler(int uart_fd);
#endif