#pragma once
#include <string.h>
#include <wiringPi.h>

#include "common.h"

#define DEVICE_ADDR  0x10

#define RELAY1  0x01
#define RELAY2  0x02
#define RELAY3  0x03
#define RELAY4  0x04

#define GPIO_ON      0xFF
#define GPIO_OFF     0x00

#define GPIO_B    RELAY1
#define GPIO_IG   RELAY2
#define GPIO_ACC  RELAY3
#define GPIO_USB_POWER RELAY4

#define GPIO_MAX_NUM 4

void gpio_write(int pin,int val);
void power_manage_init();
void power_gpio_set(int bat,int ig,int acc,int usb);
