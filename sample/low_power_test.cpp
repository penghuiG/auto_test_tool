#include "power_manage.h"

int main()
{
    power_manage_init();
    power_gpio_set(1, 1, 1, 1);
    return 0;
}