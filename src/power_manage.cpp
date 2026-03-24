#include "power_manage.h"

#include <stdio.h>
#include <wiringPi.h>
#include "log.h"
#include "curent.h"

#define CURENT_REG_ADDR 0x43   //用的地址是0x43
#define MOVE_LENGTH  100   //滑动窗口长度

static float moving_curent ;
float moving_average_get(float xn)
{
  static int index = -1;
  static float buffer[MOVE_LENGTH];
  static float sum = 0;
  float yn = 0;
  int i = 0;
  if (index == -1)
  {

    for(i = 0; i <MOVE_LENGTH; i++)
    {
      buffer[i] = xn;
    }
    sum = xn*MOVE_LENGTH;
    index = 0;
  }
  else
  {
    sum -= buffer[index];
    buffer[index] = xn;
    sum += xn;
    index++;
    if (index >= MOVE_LENGTH)
    {
      index = 0;
    }
  }
  yn = sum/MOVE_LENGTH;
  return yn;
}

void get_curent_hander()
{
	float current1 = 0;

    // wiringPiSetup();
  
    INA219* ina1 = new INA219(0x43);
	printf("INA219 TEST !!!\r\n");
	ina1->begin();
    while (1)
    {
        current1 = ina1->getCurrent_mA();               // current in mA

        // printf("Current:%6.2f ma\r\n", current1);  
        moving_curent =  moving_average_get(current1);
        usleep(100000);
    }
}
gl_status_t low_power_test()
{
    int timer = 0;

    /**1、root 成功
     * 
     * 2、断开acc|ig|usb
     * 3、看6分钟的时候电流是否在2ma左右
     * 4、链接acc|ig 唤醒
     * 5、再次进入低功耗
     */
    log_debug("-> low power test\n");
    while (1)
    {
        if (system("adb root") == 0)
        {
            break;
        }
        sleep(1);
        if (timer ++ > 40)
        {
            log_error("The tsu timeout is not started");
            return GL_STATE_FAILED;
            
        }
    }
    timer = 0;
    power_gpio_set(1, 0, 0, 0);
    log_debug("ACC|IG|USB_OPWER off\n");
    log_debug("Waiting to enter low-power mode......\n");
    while (1)
    {
        sleep(1);
        if (moving_curent < 5)
        {
            log_debug("Successfully entered low-power mode\n");
            break;
        }
        if (timer ++ > 600)
        {
            log_error("Enter the low-power timeout\n");

            return GL_SLEEP_FAIL;
        }
    }
    timer = 0;
    sleep(120);
    power_gpio_set(1, 1, 1, 1);
    log_debug("ACC|IG|USB_OPWER on\n");
    log_debug("Waiting to exit low-power mode......\n");
    while (1)
    {
        sleep(1);
        if (system("adb root") == 0)
        {
            log_debug("SOC successfully awakened\n");
            break;
        }
        if (timer ++ > 5)
        {
            log_error("Failed to exit the low-power consumption\n");
             return GL_WEAKUP_FAIL;
        }
        
    }
    log_debug("low power mode test success\n");
    return GL_STATE_OK;
}
// #define GPIO_B    25
// #define GPIO_IG   24
// #define GPIO_ACC  23
// #define GPIO_USB_POWER 22
void power_restart()
{
    gpio_write(GPIO_B, GPIO_ON);
    gpio_write(GPIO_IG, GPIO_ON);
    gpio_write(GPIO_ACC, GPIO_ON);
    gpio_write(GPIO_USB_POWER, GPIO_ON);
    sleep(2);
    gpio_write(GPIO_B, GPIO_OFF);
    gpio_write(GPIO_IG, GPIO_OFF);
    gpio_write(GPIO_ACC, GPIO_OFF);
    gpio_write(GPIO_USB_POWER, GPIO_OFF);
    log_debug("Power off and restart\n");
}
gl_status_t is_lpm_status()
{
    // std::cout << moving_curent << "ma" << std::endl;
    if (moving_curent <= 5.0)
    {
        return GL_STATE_OK;
    }
    return GL_STATE_FAILED;
}