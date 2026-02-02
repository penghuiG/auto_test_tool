#include "tsu_info.h"
#include "log.h"

#define POPEN_READ_LEN   128
#define NAME_LEN    20


int tsu_info_entry::info_get()
{
    char out[POPEN_READ_LEN] = {0};
    fp = popen(info_get_cmd.c_str(),"r");
    while (fgets(out,POPEN_READ_LEN,fp) > (char*)0)
    {
        
        data = out;
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return -1;
}

std::vector<tsu_info_entry> info;

int tsu_info_entry_init()
{

    info.resize(TSU_INFO_TYPE_MAX);
    info[SOC_VER] = tsu_info_entry("adb shell \" cat /etc/cx_version\" |grep version|awk -F'=' '{print $2}' ","SOC_VER");
    info[MCU_VER] = tsu_info_entry("adb shell \" cat /data/run_info/mcu_ver.txt\"| awk -F' : ' '{print $2}'","MCU_VER");
    info[TSU_VIN] = tsu_info_entry("adb shell \" cat /data/run_info/vininfo.txt\" ","TSU_VIN");
    info[TSU_CAR_NAME] = tsu_info_entry("adb shell \" cat /data/run_info/car_name.txt \" ","CAR_NAME");
    info[TSU_ICCID] = tsu_info_entry("adb shell \" cat /data/run_info/iccid.txt \" ","ICCID");
    info[TSU_IMEI] = tsu_info_entry("adb shell \" cat /data/run_info/imei.txt \" ","IMEI");
    info[TSU_IMSI] = tsu_info_entry("adb shell \" cat /data/run_info/imsi.txt \" ","IMSI");
    info[TSU_PART_NUMBER] = tsu_info_entry("adb shell \" cat /usrdata/conf/tsu_part_num.txt \" ","TSU_PART_NUM");
    return 0;
}

int get_tsu_info()
{
    for(int i = 0;i < TSU_INFO_TYPE_MAX; ++i)
    {
        info[i].info_get();
        // info[i].data_printf();
        log_info("%-*s = %s",NAME_LEN,info[i].name.c_str(),info[i].data.c_str());

    }
    return 0;
}

