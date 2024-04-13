#pragma once

#include <inttypes.h>
#include <esp_now.h>
#include <esp_mac.h>
#include <esp_random.h>

#include "info.h"
#include "eeprom.h"
#include "logging.h"

typedef struct
{
        char time[9];
        char date[12];
        uint32_t salt;
        uint8_t remote_conn_mac[6];
} device_settings_t;

void device_settings_init(device_settings_t *device_settings);
void device_settings_set_mac(device_settings_t *device_settings, uint8_t *mac);