#if 0
#include "uart.h"
#include "update.h"
#include <iostream>
#include <string>
#include <string.h>
#include <filesystem>
#include <stdlib.h>
#include "power_manage.h"
#include "log.h"
#include "gpio.h"
#include "adb.h"
#include "can_operation.h"
#include "mode_test.h"
#include "tsu_info.h"
#include "thread_pool.h"
// #define UART_FLAG
#define UPDATE_5G_FLAG
#define UPDATE_MCU_FLAG
#define LOG_FLAG
// #define TEST_FLAG

volatile int exit_code = 0;

#if 0
int main(int argc, const char* argv[])
{
    int sec = 0, ser_cnt = 0;
    int can_tx_flag = true;
    soc_update_t *soc = new soc_update_t();
    mcu_update_t *mcu = new mcu_update_t();
    // uart_dev *uart  = new uart_dev();
    adb_dev *adb = new adb_dev();
 
    log_clear();
    log_init("/home/cx/auto_test_test/auto_test_test.log");
    info_printf("**********test start**********\n");

    if (arg_init(argc, argv, soc, mcu) != 0)
    {
        log_error("Input parameter processing error");
        return -1;
    }

    power_manage_init();
    power_gpio_set(1, 1, 1, 1);
#ifdef TEST_FLAG
    float haha = get_current((char *)"1000159");
    std::cout<<haha<<std::endl;
#endif

#ifdef UPDATE_5G_FLAG

    info_printf("\n-> Upgrade SOC\n");
    if (system("lsusb | grep 9008") != 0)
    {
        adb->edl_enter();
    }
    system("ifconfig can0 txqueuelen 65536");
    std::thread curetn_thread(get_curent_hander);
    curetn_thread.detach();
    std::thread can_thread(tsu_program_mode, can_tx_flag);
    can_thread.detach();
    log_debug("Start upgrading SOC\n");
    int cnt = 3;
    while (cnt--)
    {
        if (soc->update() ==0 )
        {
            break;
        } 
        power_restart();
        sleep(20);
    }
    can_tx_flag = false;
    // if (system("lsusb | grep 9008") == 0)
    // {
    //     adb->edl_out();
    // }
#endif
    
#ifdef UPDATE_MCU_FLAG

    info_printf("\n-> Upgrade MCU\n");
    //120秒钟连不上adb, 直接退出
    if (adb->connect(120) < 0)
    {
        log_error("Adb connection timeout");
        return -1;
    }
    wait_in_place(30);
    //下载最新mcu版本
    mcu->download_mcu_new_ver();
    //推包用ota升级
    mcu->push_bin_update();
    log_debug("Waiting for MCU to complete the upgrade\n");
    log_debug("Start upgrading MCU\n");
    wait_in_place(120);
    adb->connect(120);
    
    while (ser_cnt < 30)
    {
        if (mcu->update_sertification() == 0)
        {
            goto exit_mcu_update;
        }
        sleep(5);
        ++ser_cnt;
    }
    log_error("MCU upgrade failed");

exit_mcu_update:
#endif
    tsu_info_entry_init();
    wait_in_place(30);
    get_tsu_info();

#ifdef UART_FLAG
    // int uart_fd = uart_init("/dev/ttyAMA10");
    // int uart_fd = uart_init((char*)"/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0");
    uart->get_dev_name_by_id();
    int uart_fd = uart->uart_init();
    
    if (uart_fd > 0)
    {
        std::thread uartThread(uartHandler, uart_fd);
        uartThread.detach();
    } 
#endif
#if 1
    if (adb->is_connect)
    {
        
        // if (soc_update_flag||mcu_update_flag)
        //     wait_in_place(30);
        soc->get_cur_version();
        mcu->get_cur_version();

        timing_test();
        
        reflash_test();

        daq_test();

        ecall_test();

        // cellular_test();
        info_printf("\n-> program startup status test\n");
        adb_shell_cmd((char *)"ls /oemapp/bin", PROCESS_QUERY);

        low_power_test();
        
    }
    
#endif
    delete soc;
    delete mcu;
    delete adb;
    // delete uart;
    log_close();
    power_gpio_set(1, 1, 1, 1);

    exit_code = 1;
    for(;;)
    {
        if (exit_code != 0)
        {
            break;
        }
    }
    return 0;
}
#endif

#include "xlxs_opt.h"
int main()
{
    // std::thread curent_thread(get_curent_hander);
    // curent_thread.detach();
    // power_manage_init();
    // ReadWithOpenXLSX();
    tread_pool_sample();
}
#endif