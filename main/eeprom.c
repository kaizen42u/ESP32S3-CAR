
#include "eeprom.h"

eeprom_handle_t *eeprom_default_config(eeprom_handle_t *eeprom_handle)
{
        eeprom_handle->namespace_name = "storage";
        eeprom_handle->key = "default";
        eeprom_handle->open_mode = NVS_READWRITE;
        return eeprom_handle;
}

esp_err_t eeprom_get_entry(eeprom_handle_t *eeprom_handle, void *out_value, size_t size)
{
        nvs_handle_t nvs_handle;
        esp_err_t err;

        // Open
        err = nvs_open(eeprom_handle->namespace_name, eeprom_handle->open_mode, &nvs_handle);
        // err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
        if (err != ESP_OK)
                return err;

        // Read the size of memory space required for blob
        size_t required_size = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_blob(nvs_handle, eeprom_handle->key, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        {
                nvs_close(nvs_handle);
                return err;
        }

        // Read previously saved blob if available
        if (size < required_size)
        {
                nvs_close(nvs_handle);
                return ESP_ERR_INVALID_SIZE;
        }

        err = nvs_get_blob(nvs_handle, eeprom_handle->key, out_value, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        {
                nvs_close(nvs_handle);
                return err;
        }

        // Close
        nvs_close(nvs_handle);
        return ESP_OK;
}

esp_err_t eeprom_set_entry(eeprom_handle_t *eeprom_handle, void *in_value, size_t size)
{
        nvs_handle_t nvs_handle;
        esp_err_t err;

        // Open
        err = nvs_open(eeprom_handle->namespace_name, eeprom_handle->open_mode, &nvs_handle);
        // err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
        if (err != ESP_OK)
                return err;

        // Read the size of memory space required for blob
        size_t required_size = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_blob(nvs_handle, eeprom_handle->key, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        {
                nvs_close(nvs_handle);
                return err;
        }

        // Read previously saved blob if available
        if (size < required_size)
        {
                nvs_close(nvs_handle);
                return ESP_ERR_INVALID_SIZE;
        }

        void *out_value = malloc(required_size + size);
        err = nvs_get_blob(nvs_handle, eeprom_handle->key, out_value, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        {
                nvs_close(nvs_handle);
                return err;
        }

        // Skip if nothing has changed
        if (strncmp(out_value, in_value, size) == 0)
        {
                nvs_close(nvs_handle);
                free(out_value);
                return ESP_OK;
        }
        free(out_value);

        // Save blob
        err = nvs_set_blob(nvs_handle, eeprom_handle->key, in_value, size);
        if (err != ESP_OK)
        {
                nvs_close(nvs_handle);
                return err;
        }

        // Commit
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK)
        {
                nvs_close(nvs_handle);
                return err;
        }

        // Close
        nvs_close(nvs_handle);
        return ESP_OK;
}