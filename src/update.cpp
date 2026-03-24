#include <cstring>
#include <vector>
#include <stdlib.h>
#include "update.h"
#include "log.h"
#include "gpio.h"
// #include "can.h"

char car_type[256] = {0};
char contry[256] = {0};
char version_header[256] = {0};


update_t::~update_t()
{
    power_gpio_set(1, 1, 1, 1);
}
mcu_update_t::mcu_update_t()
{
    is_update_success = 0;
}

int mcu_update_t::download_update_package()
{
    FILE *fp = NULL;
    char name[64] = {0};
    int i = 0;
    std::vector<std::string>bin;
    //download
    popen("/home/cx/auto_test/temp/project_t/shell/download_mcu_package.sh", "r");

    fp = popen("ls /home/cx/auto_test/project/update_package/mcu_package/|grep .bin", "r");
    if (fp == NULL)
    {
        perror("no mcu bin or popen err\n");
        return -1;
    }
    while (fgets(name, 64, fp) > (char*)0)
    {
        bin.push_back(name);
        memset(name, 0, sizeof(name));
    }
       
    pclose(fp);
    if (false == bin.empty())
    {
        update_package_name = bin.back();
    }
    else
    {
        log_error("No valid bin file available");
    }
    
    return 0;
}

int mcu_update_t::get_ver_log_file_name()
{
    FILE * fp =NULL;
    char out[128] = {0};
    std::vector<std::string> ota_mcu_file_names;
    system("rm /home/cx/auto_test/project/tsu_version_info/unsynctime/ -rf");
    if (system("adb pull /usrdata/cx_log/log/unsynctime /home/cx/auto_test/project/tsu_version_info") == 0)
    {
        // sleep(1);
        fp = popen("ls /home/cx/auto_test/project/tsu_version_info/unsynctime |grep ota_mcu", "r");
        if (fp == NULL)
        {
            perror("popen err");
            return -1;
        }
        else
        {
            while (fgets(out, 128, fp) > (char*)0)
            {
                ota_mcu_file_names.push_back(out);
                memset(out, 0, 128);
            }
        }
    }
    else
    {
        log_error("Failed to download the MCU log file");
        pclose(fp);
        return -1;
    }

    pclose(fp);
    ver_log_file_name = ota_mcu_file_names.back();
    return 0;
}

int mcu_update_t::get_cur_version()
{
    int _timer = 0;
    FILE * mcu_fp = NULL;
    char cmd[256];
    char mcu_v[256];
    // log_debug("get mcu version");
    while (1)
    {
        if (_timer > 15)
        {
            log_error("Timeout was not started when getting the MCU version");
            return -1;
        }
        if (!system("adb root"))
        {
            break;
        }
        std::cout << "Wait for the mod to be online" << std::endl;
        ++_timer;
        sleep(3);
    }
    sprintf(cmd, "adb shell \"cat /data/run_info/mcu_ver.txt\"| awk -F' : ' '{print $2}'");
    
    mcu_fp = popen(cmd, "r");
    if (fgets(mcu_v, 256, mcu_fp)>(char*)0)
    {
        cur_version = mcu_v;
    }
    // log_info("mcu_ver : %s", cur_version.c_str());
    pclose(mcu_fp);

    return 0;
}
int mcu_update_t::update()
{
    int time = 20;
	int timer_ = 0;
	std::string cmd;
    
	cmd = "/home/cx/auto_test/temp/project_t/shell/program_bin.sh /home/cx/auto_test/project/update_package/mcu_package/" + update_package_name;
	std::cout<<cmd<<std::endl;
	if (system(cmd.c_str()) == 0)
	{
		log_debug("Wait for the TSU restart");
		sleep(3);
	}
	else
	{
		log_debug("The MCU version update failed");
		return -1;
	}
	while (1)
    {
        if (!system("adb root"))
        {
            break;
        }
		++timer_;
        std::cout<<"Wait for the mod to be online"<<std::endl;
        sleep(1);
		if (timer_ >= 120)
        {
            log_error("The timeout is not started, The MCU version update failed");
            return -1;
        }
    }
	timer_ = 0;
	std::cout<<"等待"<<time<<"秒读取MCU版本。。。"<<std::endl;
	sleep(time);
    get_cur_version();
	log_debug("The MCU version number after the update is:%s", cur_version.c_str());

	return 0;
}
int mcu_update_t::clear_mcu_ver_file()
{
    // int ret;
    if (system("rm /home/cx/auto_test_test/mcu_ver_file -rf") != 0)
    {
        return -1;
    }
    // if (system("mkdir mcu_ver_file") != 0)
    // {
    //     return -1;
    // }
    return 0;
}
std::string mcu_update_t::check_mcu_new_ver()
{
    FILE *f = nullptr;
    // int ver_num[512] = {0};
    std::vector<std::string>ver_num;
    int i = 0;
    char out[512] = {0};
    char local_ver[128];
    char cmd[512] = {0};
    

    snprintf(cmd, sizeof(cmd), 
            "svn list --username xxxxxx --password xxxxxxxx \"%s%s/\"|grep %s", 
            mcu_svn_path.c_str(), 
            contry.c_str(), 
            version_header.c_str());
    
    std::cout << cmd <<std::endl;
    f = popen(cmd, "r");
    while (fgets(out, 128, f) > (char*)0)
    {
        ver_num.push_back(out);
        memset(out, 0, sizeof(out));
    }
    
    pclose(f);
    
    FILE* f1 = popen("ls /home/cx/auto_test_test/mcu_ver_file/ |awk -F'_' '{print $1}'", "r");
    
    if (fgets(local_ver, sizeof(local_ver), f1) > (char*)0)
    {
        std::cout << local_ver << "  " << ver_num.back() << std::endl;
        if (strncmp(local_ver, ver_num.back().c_str(), strlen(local_ver)-1) != 0)
        {
            std::cout << ver_num.back() << "||||"<<local_ver<<std::endl;
            clear_mcu_ver_file();
        }       
    }
    else
    {
        clear_mcu_ver_file();
    }
    pclose(f1);
    
    return ver_num.back();
}
int mcu_update_t::download_mcu_new_ver()
{
    char cmd[256] = {0};
    is_update_success = 0;
    std::string mcu_ver_num = check_mcu_new_ver();
    mcu_ver_num.pop_back();

    sprintf(cmd, "svn checkout --username xxxxxx --password xxxxxxxx \"%s%s/%s\" /home/cx/auto_test_test/mcu_ver_file/", mcu_svn_path.c_str(), contry.c_str(), mcu_ver_num.c_str());
    // std::cout <<cmd<<std::endl;
    int status = system(cmd);
    if (status)
    {
        log_debug("download_mcu_new_ver ERROR");
    }
    return status;
}

int mcu_update_t::push_bin_update()
{
    char cmd[256] = {0};
    char out[128] = {0};
    FILE *f = nullptr;
    f = popen("ls /home/cx/auto_test_test/mcu_ver_file", "r");
    if (fgets(out, 128, f) > (char*)0)
    {
        for(int i = 0;i < 128;++i)
        {
            if (out[i] == '\n')
            {
                out[i] = 0;
                break;
            }
        }
        sprintf(cmd, "adb push /home/cx/auto_test_test/mcu_ver_file/%s /usrdata/ota", out);
    }
    pclose(f);
    
    std::cout << cmd <<std::endl;
    int ret = system(cmd);
    log_debug("%s, status = %d", cmd, ret);
    if (ret == 0)
    {
        log_debug("Successfully pushed MCU upgrade package");
        
    }
    return ret;
}
int mcu_update_t::get_update_package_name()
{
    char out[128] = {0};
    FILE *f = popen("ls /home/cx/auto_test_test/mcu_ver_file/", "r");
    if (fgets(out, 128, f) > (char*)0)
    {
        update_package_name = out;
        return 0;
    }
    return -1;
}
int mcu_update_t::update_sertification()
{
    char out[128] = {0};
    const char *ret = nullptr;
    is_update_success = 0;
    get_update_package_name();
    FILE *f = popen("adb shell \"cat /data/run_info/mcu_ver.txt \"| awk -F' : ' '{print $2}'", "r");
    if (fgets(out, 128, f) > (char*)0)
    {
        for(int i=0;i<128;++i)
        {
            if (out[i] == '\n')
                out[i] = 0;
        }
        printf("%s\n", out);
        printf("%s\n", update_package_name.c_str());
        ret = strstr(update_package_name.c_str(), out);
        if (ret != nullptr)
        {
            fclose(f);
            log_debug("MCU upgrade successful\n");
            is_update_success = 1;
            return 0;
        }
    }
    fclose(f);
    return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
std::string soc_update_t::get_file_name(const std::string& str)
{
    int i = 0;
    int len = str.length();
    std::string ret;

    for(;i<len;i++)
    {
        if (str[len-i] == '/')
        {
            break;
        }
    }
    return str.substr(len - i + 1, i);
}
int soc_update_t::download_update_package()
{
    char cmd[512] = {0};

    char shell_output[1024] = {0};

    std::string file_name = get_file_name(local_address);
    
    std::cout<<file_name<<std::endl;

    sprintf(cmd, "curl -u \"develop:Develop.1234\" -X GET %s>./update_package/mode5g_package/%s\n", remote_address.c_str(), file_name.c_str());

    return system(cmd);
    
}
int soc_update_t::get_update_package_path()
{
    update_package_path = "/home/cx/DailyTest/" + car_type + "/" +soc_hard_type;
    log_debug("car_type = %s", car_type.c_str());
    return 0;
}
int soc_update_t::get_ver_log_file_name()
{

    return 0;
}

int soc_update_t::get_cur_version()
{
    int timer_ = 0;
    FILE * soc_fp = NULL;
    std::string ret;
    char soc_v[256] = {0};
    while (1)
    {
        if (timer_ > 30)
        {
            log_error("Timeout was not started when getting the SOC version");
            return -1;
        }
        if (!system("adb root"))
        {
            break;
        }
        std::cout<<"Wait for the mod to be online"<<std::endl;
        sleep(3);
        ++timer_;
    }

    soc_fp = popen("adb shell \"cat /etc/cx_version\" |grep version|awk -F'=' '{print $2}'", "r");
    if (fgets(soc_v, 128, soc_fp)>(char*)0)
    {
        cur_version = soc_v;
    }

    pclose(soc_fp);
    return 0;
}
int soc_update_t::update()
{

    std::string cmd ;
    std::string path;
    std::string pre_tsu_version;
    std::string cur_tsu_version;


    get_update_package_path();
    if (false == update_package_path.empty())
    {
        cmd = "/home/cx/auto_test_test/shell/QFirehose -f " + update_package_path;
        log_debug("update_package_path = %s", update_package_path.c_str());
    }


    std::cout<<cmd<<std::endl;


    if (system(cmd.c_str()) == 0)
    {
        log_debug("The 5G module was updated successfully");
    }
    else
    {
        log_error("The 5G module update failed");
        return -1;
    }

    // pclose(fp1);
    return 0;
}

int arg_init(int argc, const char* argv[], soc_update_t *soc, mcu_update_t *mcu)
{
    if (argc < 4)
    {
        printf("Wrong number of parameters\n");
        return -1;
    }
    if (argv[1] != NULL && (strcmp(argv[1], "3A0V") == 0))
    {
        mcu->mcu_svn_path = A0V_SVN_PATH;
        mcu->version_header = A0V_HEADER;
        soc->car_type = "3A0V";
        mcu->contry = "PHEV";
        
    }
    else if (argv[1] != NULL && (strcmp(argv[1], "3DPA") == 0))
    {
        mcu->mcu_svn_path = DPA_SVN_PATH;
        mcu->version_header = DPA_HEADER;
        soc->car_type = "3DPA";
        if (argv[2] != NULL && (strcmp(argv[2], "JP") == 0))
        {
            mcu->contry = "JP";
        }
        else
        {
            mcu->contry = "Other";
        }
    }
    else
    {
        mcu->mcu_svn_path = BWT_SVN_PATH;
        mcu->version_header = BWT_HEADER;
        soc->car_type = "3BWT";
        if (argv[2] != NULL && (strcmp(argv[2], "EU") == 0))
        {
            mcu->contry = "EU";
        }
        else
        {
            mcu->contry = "JP";
        }
    }

    if (argv[3] != NULL && (strcmp(argv[3], "EEU") == 0))
    {
        soc->soc_hard_type = "EEU";
    }
    else
    {
        soc->soc_hard_type = "EROW";
    }
    return 0;
}