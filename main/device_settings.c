#include "device_settings.h"

static const char *TAG = "device_settings";
__unused static const uint8_t broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
__unused static const uint8_t placeholder_mac[ESP_NOW_ETH_ALEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
__unused static const uint8_t test_mac[ESP_NOW_ETH_ALEN] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
static eeprom_handle_t eeprom_handle;

bool is_same(const void *a, const void *b, const size_t size)
{
        return (memcmp(a, b, size) == 0);
}

void device_settings_default(device_settings_t *device_settings)
{
        memset(device_settings, 0, sizeof(device_settings_t));
        eeprom_default_config(&eeprom_handle);
        memcpy(device_settings->time, __TIME__, 8);
        memcpy(device_settings->date, __DATE__, 11);
        memcpy(device_settings->remote_conn_mac, broadcast_mac, ESP_NOW_ETH_ALEN);
        device_settings->salt = esp_random();
}

void device_settings_print(device_settings_t *device_settings)
{
        LOG_INFO("      Built time : %s %s", __DATE__, __TIME__);
        LOG_INFO("Config save time : %s %s", device_settings->date, device_settings->time);
        LOG_INFO("  Peer MAC addr. : " MACSTR, MAC2STR(device_settings->remote_conn_mac));
        LOG_INFO("            salt : %lu", device_settings->salt);
}

void device_settings_init(device_settings_t *device_settings)
{
        esp_err_t err;
        // LOG_ERROR("INITIAL:");
        // device_settings_print(device_settings);

        device_settings_default(device_settings);
        // LOG_INFO("namespace: %s", eeprom_handle.namespace_name);

        // LOG_ERROR("DEFAULT:");
        // device_settings_print(device_settings);

        // device_settings_default(device_settings);
        // LOG_ERROR("DEFAULT2:");
        // device_settings_print(device_settings);

        // memcpy(device_settings->remote_conn_mac, test_mac, ESP_NOW_ETH_ALEN);
        // err = eeprom_set_entry(&device_settings->eeprom_handle, device_settings, sizeof(device_settings_t));
        // ESP_ERROR_CHECK_WITHOUT_ABORT(err);

        // LOG_ERROR("TEST SAVING:");
        // device_settings_print(device_settings);

        // memcpy(device_settings->remote_conn_mac, placeholder_mac, ESP_NOW_ETH_ALEN);
        err = eeprom_get_entry(&eeprom_handle, device_settings, sizeof(device_settings_t));
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);

        // LOG_ERROR("TEST RETRIEVING:");
        device_settings_print(device_settings);

        if (is_same(device_settings->time, __TIME__, 8) && is_same(device_settings->date, __DATE__, 11))
        {
                // LOG_WARNING("SKIPPED CHECK 1");
                return;
        }

        if (is_same(device_settings->remote_conn_mac, broadcast_mac, ESP_NOW_ETH_ALEN))
        {
                // LOG_WARNING("SKIPPED CHECK 2");
                return;
        }
        if (CLEAR_PAIRED_PEER_ON_NEW_UPLOAD != 1)
        {
                // LOG_WARNING("SKIPPED CHECK 3");
                return;
        }

        LOG_WARNING("Cleared paired peer due to new program installed.");

        device_settings_default(device_settings);
        err = eeprom_set_entry(&eeprom_handle, device_settings, sizeof(device_settings_t));
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        device_settings_print(device_settings);
}

void device_settings_set_mac(device_settings_t *device_settings, uint8_t *mac)
{
        esp_err_t err;

        // LOG_ERROR("BEFORE:");
        // device_settings_print(device_settings);

        memcpy(device_settings->remote_conn_mac, mac, ESP_NOW_ETH_ALEN);

        // LOG_ERROR("WRITING:");
        // device_settings_print(device_settings);

        err = eeprom_set_entry(&eeprom_handle, device_settings, sizeof(device_settings_t));
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);

        device_settings_default(device_settings);

        // LOG_ERROR("CLEARED:");
        // device_settings_print(device_settings);

        err = eeprom_get_entry(&eeprom_handle, device_settings, sizeof(device_settings_t));
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);

        // LOG_ERROR("AFTER:");
        // device_settings_print(device_settings);
}
