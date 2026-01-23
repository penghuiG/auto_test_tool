#include "uart.h"
#include "log.h"
#include <algorithm>

extern test_info_t test_info;
// float get_avg_cur()
// {
//     int count = 0;
//     int timer_ = 0;
//     float sum = 0;
//     while(1)
//     {
//         ++count;
//         sum += test_info.cur_val;
//         ++timer_;
//         usleep(200000);
//         if(timer_ == 50)
//         {
//             sum /=count;
//             log_debug(0,"count = %d,avg = %.2f\n",count,sum);
//             timer_ = 0;
//             break;
//         }
//     }
//     return sum;
// }
float moving_average(float xn)
{
  static int index = -1;
  static float buffer[MOVE_AVERAGE_LENGTH];
  static float sum = 0;
  float yn = 0;
  int i = 0;
  if(index == -1)
  {

    for(i = 0; i <MOVE_AVERAGE_LENGTH; i++)
    {
      buffer[i] = xn;
    }
    sum = xn*MOVE_AVERAGE_LENGTH;
    index = 0;
  }
  else
  {
    sum -= buffer[index];
    buffer[index] = xn;
    sum += xn;
    index++;
    if(index >= MOVE_AVERAGE_LENGTH)
    {
      index = 0;
    }
  }
  yn = sum/MOVE_AVERAGE_LENGTH;
  return yn;
}

int uart_init(char * dev)
{
    int uart_fd = -1;
    if((uart_fd=serialOpen(dev,38400))<0)
    { 
      log_debug("Serial port initialization failed\n");
      return -1;
    }
    log_debug("uart_fd = %d\n",uart_fd);
    // serialPrintf(uart_fd, "uart OK.........");
    return uart_fd;
}

float get_current(char *current_char)
{
  float z = 0;
  float x = 0;
  float t = 0;
  
  for(int i = 0;i < 4;i++)
  {
    if(current_char[i] < '0' || current_char[i] > '9')
    {
      return -1;
    }
    else
    {
      t = current_char[i] - 48;
      for(int j = 1;j < 4 - i;j++)
      {
        t = t  * 10;
      }
    }
    z += t;
  }
  for(int i = 4;i < 7;i++)
  {
    if(current_char[i] < '0' || current_char[i] > '9')
    {
      return -1;
    }
    else
    {
      t = current_char[i] - 48;
      for(int j = 0;j < i - 3;j++)
      {
        t *= 0.1;
      }
    }
    x += t;
  }
  return z + x;
  
}

void uartHandler(int uart_fd)
{
    // int uart_fd  = *(int*)data;
    char buff[32] = {0};
    float current = 0;
    while(1)
    {
       int sz = serialDataAvail(uart_fd); 
       
       if(sz > 0)
       {
          for(int i = 0; i < sz; i++)
          {
              int c = serialGetchar(uart_fd);
              if(c != -1)
                  buff[i] = c;  
          }
          // std::cout<<buff;
          current = get_current(buff);
          if(current >= 0)
          {
            test_info.cur_val = current;
            test_info.avg_val = moving_average(current);
            // log_debug("test_info.avg_val = %.2f\n",test_info.avg_val);
          }
          memset(buff,0,32);
       }
       else
       {
         usleep(50000); // 必要的延时50ms
       }
    }

    serialClose(uart_fd);
    log_debug("close serial.\n");
}
uart_dev::uart_dev()
{
  baud_rate = 115200;
  fd = -1;
}

uart_dev::~uart_dev()
{
  std::cout<< "uart is closed"<<std::endl;
}
int uart_dev::get_dev_name_by_id()
{
  char out[128] = {0};
  FILE *f = popen("ls /dev/serial/by-id/","r");
  if(fgets(out,sizeof(out),f) > (char*)0)
  {
    for(int i = 0;i < sizeof(out);++i)
    {
      if(out[i] == '\n')
        out[i] = 0;
    }
    dev_name = out;
    dev_name = "/dev/serial/by-id/" + dev_name;
  }
  else
  {
    std::cout << "Please insert USB serial port tool" <<std::endl;
    pclose(f);
    return -1;
  }
  
  pclose(f);
  return 0;
}

int uart_dev::uart_init()
{
  int uart_fd = -1;
  std::cout << "uart name: " << dev_name << std::endl;
  if((uart_fd=serialOpen(dev_name.c_str(),baud_rate))<0)
  { 
    log_debug("Serial port initialization failed\n");
    return -1;
  }
  log_debug("uart_fd = %d\n",uart_fd);
  // serialPrintf(uart_fd, "uart OK.........");
  return uart_fd;
  
}

int uart_dev::uart_init(std::string dev)
{

  return fd;
}
int uart_dev::uart_init(char* dev)
{
  
  return fd;
}