#pragma once

#include <inttypes.h>
#include <esp_now.h>
#include <esp_mac.h>
#include <esp_random.h>

#include "info.h"
#include "eeprom.h"
#include "logging.h"

// Persistent settings that will be saved after a power loss
typedef struct
{
        char time[9];               // Time of code compile
        char date[12];              // Date of code compile
        uint32_t salt;              // Random bits
        uint8_t remote_conn_mac[6]; // Remote MAC address
} device_settings_t;

// Initialize the storage partition and loads settings from flash
void device_settings_init(device_settings_t *device_settings);

// Writes new MAC address into flash
void device_settings_set_mac(device_settings_t *device_settings, uint8_t *mac);