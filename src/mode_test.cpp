
#include "mode_test.h"
#include "adb.h"
#include "log.h"
#include "string.h"
#include "gpio.h"
#include <vector>
#include <cstdio>
/** */
#define POPEN_READ_LEN   128

static gl_status_t popen_data_get(std::string cmd)
{
    char buffer[512];
    FILE* fp = nullptr;
    fp = popen(cmd.c_str(), "r");
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
         log_debug("%s", buffer);
    }
    pclose(fp);
    return GL_STATE_OK;
}
/*time sync*/
gl_status_t timing_test()
{
    char buffer[256];
    FILE* fp = nullptr;
    log_debug("-> Time synchronization test\n");
    std::string cmd("adb shell \"cat /tmp/ql_time_sync_record_flag\"");
    if (system(cmd.c_str()))
    {
        log_error("/tmp/ql_time_sync_record_flag not exit");
        return GL_STATE_FAILED;
    }

    popen_data_get(cmd);
    return GL_STATE_OK;
}
/*REFLASH*/
gl_status_t reflash_test()
{
    log_debug("-> reflash test\n");
    if (system("adb shell \"ls /proc/recoveryinfo\"") != 0)
    {
        log_error("tsu_reflash err");
        return GL_STATE_FAILED;
    }
    log_debug("Reflash test successful");
    return GL_STATE_OK;
}

/*DAQ*/
gl_status_t daq_test()
{
    log_debug("-> daq test\n");
    if (system("adb shell \"ps | grep oemapp/bin/daq | grep -v grep\""))
    {
        log_error("DAQ No startup");
        return GL_STATE_FAILED;
    }
    if (system("adb shell \"ls -l /usrdata/sqlite/daq_applied.db\""))
    {
        log_error("The daq applied.db file is missing");
    }
    if (system("adb shell \"rm -rf /usrdata/sqlite/upload/* \"") == 0)
    {
        
        gpio_write(GPIO_IG, 1);
        sleep(2);
        gpio_write(GPIO_IG, 0);
        sleep(30);
        if (system("adb shell \"ls -l /usrdata/sqlite/upload/ |grep .EOF\""))
        {
            log_error("DAQ is not working properly");
            return GL_STATE_FAILED;
        }
    }

    log_debug("DAQ test successful");
    return GL_STATE_OK;
}
/*ecall*/
gl_status_t ecall_test()
{
    char out[128] = {0};
    log_debug("-> ecall test\n");
    FILE *f = popen("adb shell \"cat /etc/ecall_version_*\"", "r");
    if (fgets(out, 128, f) > (char*)0)
    {
        log_debug("%s", out);
    }
    else
    {
        pclose(f);
        log_error("Unable to obtain ecall version");
        return GL_STATE_FAILED;
    }
    pclose(f);
    return GL_STATE_OK;
}
/*cellular*/
gl_status_t cellular_test()
{
    FILE *fp = NULL;
    char buf[128] = {0};
    log_debug("-> cellular test\n");
    if (system("adb shell \"ps | grep oemapp/bin/cellular | grep -v grep\""))
    {
        log_error("cellular No startup");
        return GL_STATE_FAILED;
    }

    fp = popen("adb shell \"cat /usrdata/conf/tsu_part_num.txt\"", "r");
    if (fgets(buf, sizeof(buf), fp)>(char*)0)
    {
        log_debug("tsu_part_num: %s", buf);
        if (strncmp("8B100-3BWT-COMM-M1", buf, sizeof("8B100-3BWT-COMM-M1")-1))
        {
            log_error("cat tsu_part_num.txt err != 8B100-3BWT-COMM-M1");
            return GL_STATE_FAILED;
        }
    }

    if (system("adb shell \"ifconfig | grep rmnet_data\""))
    {
        log_error("The dial status is wrong");
        return GL_STATE_FAILED;
    }

    memset(buf, 0, sizeof(buf));
    FILE *f1 = popen("adb shell \"cat /data/run_info/cert_cn.txt\"", "r");
    if (fgets(buf, sizeof(buf), f1))
    {
        log_debug("cert_cn: %s", buf);
    }
    pclose(f1);
    memset(buf, 0, sizeof(buf));
    FILE *f2 = popen("adb shell \"cat /data/run_info/iccid.txt\"", "r");
    if (fgets(buf, sizeof(buf), f2))
    {
        log_debug("iccid: %s", buf);
    }
    pclose(f2);
    memset(buf, 0, sizeof(buf));
    FILE *f3 = popen("adb shell \"cat /data/run_info/imei.txt\"", "r");
    if (fgets(buf, sizeof(buf), f3))
    {
        log_debug("imei: %s", buf);
    }
    pclose(f3);

    pclose(fp);
    log_debug("cellular test successful");
    return GL_STATE_OK;
}