
#include "power.h"

__unused static const char *TAG = "power";

void keep_power(void)
{
        gpio_set_level(GPIO_WAKE, 1);
}

void kill_power(void)
{
        gpio_set_level(GPIO_WAKE, 0);
}

void config_wake_gpio(void)
{
        gpio_config_t wake_io_conf = {
            .pin_bit_mask = 1 << GPIO_WAKE,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&wake_io_conf));
        keep_power();
}
