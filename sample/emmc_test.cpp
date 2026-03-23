#include "emmc.h"
#include "power_manage.h"
#include "log.h"
#include <thread>

int main()
{

    // log_clear();
    log_init("/home/cx/auto_test_test/auto_test_test.log");

    power_manage_init();
    power_gpio_set(1, 1, 1, 1);

    std::thread curetn_thread(get_curent_hander);
    curetn_thread.detach();

    emmc_test();
    return 0;
}