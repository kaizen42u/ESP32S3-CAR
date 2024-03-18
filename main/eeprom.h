
#pragma once

#include <nvs_flash.h>
#include <nvs.h>
#include <esp_err.h>
#include <string.h>

#include "logging.h"

typedef struct
{
        const char *namespace_name;
        const char *key;
        nvs_open_mode_t open_mode;
} eeprom_handle_t;

eeprom_handle_t *eeprom_default_config(eeprom_handle_t *eeprom_handle);
esp_err_t eeprom_get_entry(eeprom_handle_t *eeprom_handle, void *out_value, size_t size);
esp_err_t eeprom_set_entry(eeprom_handle_t *eeprom_handle, void *in_value, size_t size);
