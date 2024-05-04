
#pragma once

#include <nvs_flash.h>
#include <nvs.h>
#include <esp_err.h>
#include <string.h>

#include "logging.h"

// Handle to the emulated EEPROM storage
typedef struct
{
        const char *namespace_name; // Partition name
        const char *key;            // Key of blob storage, like file name
        nvs_open_mode_t open_mode;  // Read only or read-write
} eeprom_handle_t;

// Loads default settings of the emulated EEPROM storage
eeprom_handle_t *eeprom_default_config(eeprom_handle_t *eeprom_handle);

// `Reads` byte data from the emulated EEPROM storage
// Call only after `eeprom_default_config()`
esp_err_t eeprom_get_entry(eeprom_handle_t *eeprom_handle, void *out_value, size_t size);

// `Writes` byte data from the emulated EEPROM storage
// Call only after `eeprom_default_config()`
esp_err_t eeprom_set_entry(eeprom_handle_t *eeprom_handle, void *in_value, size_t size);
