#include "bsp/gpio.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>
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
        gpio_write(GPIO_B, GPIO_OFF);
    }
    else
    {
        gpio_write(GPIO_B, GPIO_ON); 
    }

    if (ig)
    {
        gpio_write(GPIO_IG, GPIO_OFF);
    }
    else
    {
        gpio_write(GPIO_IG, GPIO_ON); 
    }

    if (acc)
    {
        gpio_write(GPIO_ACC, GPIO_OFF);
    }
    else
    {
        gpio_write(GPIO_ACC, GPIO_ON); 
    }

    if (usb)
    {
        gpio_write(GPIO_USB_POWER, GPIO_OFF);
    }
    else
    {
        gpio_write(GPIO_USB_POWER, GPIO_ON); 
    }
}




// int main(void){
//     printf("Turn on Relays in C\n");
//     int fd;
//     int i = 0;
    
//     for(;;){
//        for (i=1; i<=4; i++){
//           printf("turn on relay No.%d\n", i);
//           wiringPiI2CWriteReg8(fd, i, ON);
//           sleep(2);
//           printf("turn off relay No.%d\n", i);
//           wiringPiI2CWriteReg8(fd, i, OFF);
//           sleep(2);
//        }
//     }
//     return 0;
// } 