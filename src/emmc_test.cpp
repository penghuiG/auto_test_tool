#include "emmc_test.h"
#include "log.h"
#include "power_manage.h"
#include "gpio.h"

gl_status_t emmc_witting_sleep(int timeout)
{
    gpio_write(GPIO_USB_POWER,GPIO_ON);
    gpio_write(GPIO_IG,GPIO_ON);
    gpio_write(GPIO_ACC,GPIO_ON);
    while(timeout--)
    {
        if(is_lpm_status() == GL_STATE_OK)
        {
            log_debug("tsu is in low-power mode");
            return GL_STATE_OK;
        }
        sleep(1);
    }
    log_error("tsu is not in low-power mode");
    return GL_STATE_FAILED;
}

gl_status_t emmc_witting_wakeup(int timeout)
{
    gpio_write(GPIO_USB_POWER,GPIO_OFF);
    gpio_write(GPIO_IG,GPIO_OFF);
    gpio_write(GPIO_ACC,GPIO_OFF);
    while(timeout--)
    {
        if(system("adb root") == 0)
        {
            log_debug("tsu is in wakeup mode");

            return GL_STATE_OK;
        }
        sleep(1);
    }
    log_error("tsu is not in wakeup mode");
    return GL_STATE_FAILED;
}
//向emc写入1M数据
gl_status_t emmc_write()
{
    if(system("adb shell \"dd if=/dev/zero of=/usrdata/conf/test1.img bs=1M count=1\"") == 0)//写入1M数据
    {
        log_debug("EMMC write success");
        sleep(10);
        return GL_STATE_OK;
    }
    log_error("EMMC write fail");
    return GL_STATE_FAILED;
}



void run_command_and_print(const char *cmd) {
    FILE *fp;
    char buf[1024];

    // 打开管道执行命令
    printf("cmd: %s\n",cmd);
    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen failed");
        return;
    }

    // 逐行读取并打印输出
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        log_error("%s", buf);
    }

    // 关闭管道
    pclose(fp);
}

gl_status_t emmc_check_log()
{
    if(system("adb shell \"dmesg|grep 'error -110'\"") == 0)
    {
        log_error("EMMC check log fail");
        run_command_and_print("adb shell \"dmesg|grep 'error -110'\"");
        return GL_STATE_FAILED;
    }
    log_debug("EMMC check log success");
    return GL_STATE_OK;
}

/**
1、断电(b+/acc/ig/vbus OFF)
2、上电(b+/acc/ig/vbus ON)
3、使板子进入低功耗acc/ig/vbus OFF)
4、sleep 两分钟
5、唤醒(acc/ig/vbus ON)
6、等tsu起来，立马写emmc，   adb shell "dd if=/dev/zero of=/data/test1.img bs=1M count=1"
7、sleep 两分钟
8、获取关键日志，   adb shell "dmesg|grep  'error -110'"
9、到第二步重复
 */
gl_status_t emmc_test()
{
    int i = 0;
    
    while(i < 200)
    {
        ++i;
        log_debug("********************EMMC test %04d********************\n",i);
        power_restart();
        sleep(10);
        emmc_witting_wakeup(100);
        sleep(30);
        // log_debug("冷启动");
        // emmc_write();
        // emmc_check_log();

        emmc_witting_sleep(400);
        sleep(30);
        emmc_witting_wakeup(10);
        log_debug("休眠唤醒");
        emmc_write();
        emmc_check_log();
    }
    


    return GL_STATE_OK;
}