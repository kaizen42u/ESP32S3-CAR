
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"

#include "button.h"
#include "espnow.h"
#include "pindef.h"
#include "motor_controller.h"
#include "catapult_controller.h"
#include "goalkeeper_controller.h"
#include "ws2812.h"
#include "rssi.h"
#include "logging.h"
#include "mathop.h"
#include "eeprom.h"
#include "device_settings.h"
#include "power.h"

static const char __attribute__((unused)) *TAG = "app_main";

static espnow_send_param_t espnow_send_param;
static esp_connection_handle_t esp_connection_handle;
static motor_controller_handle_t motor_controller_handle;
static device_settings_t device_settings;

void rssi_task()
{
        ws2812_hsv_t hsv = {.h = RGB_LED_HUE, .s = RGB_LED_SATURATION, .v = 0};
        ws2812_handle_t ws2812_handle;
        ws2812_default_config(&ws2812_handle);
        ws2812_init(&ws2812_handle);
        ws2812_set_hsv(&ws2812_handle, &hsv);
        ws2812_update(&ws2812_handle);

        QueueHandle_t rssi_event_queue = rssi_init();
        uint8_t countdown = 0;
        for (;;)
        {
                rssi_event_t rssi_event;
                while (xQueueReceive(rssi_event_queue, &rssi_event, 0))
                {
                        // print_rssi_event(&rssi_event);
                        esp_connection_update_rssi(&esp_connection_handle, &rssi_event);

                        const int rssi_min = MIN_RSSI_TO_INITIATE_CONNECTION;
                        if (rssi_event.rssi > rssi_min)
                        {
                                countdown = 100;
                                float led_volume = map(rssi_event.rssi, 0, rssi_min, 50, 0);
                                led_volume = constrain(led_volume, 0, 100);
                                hsv.v = led_volume;
                                ws2812_set_hsv(&ws2812_handle, &hsv);
                                ws2812_update(&ws2812_handle);
                        }
                }

                if (countdown)
                        countdown--;
                else
                {
                        if (esp_connection_handle.remote_connected)
                                hsv.v = RGB_LED_VALUE * esp_connection_handle.remote_connected;
                        else
                                hsv.v = 0;
                        ws2812_set_hsv(&ws2812_handle, &hsv);
                        ws2812_update(&ws2812_handle);
                }
                vTaskDelay(pdMS_TO_TICKS(10));
        }
}

void info_task()
{
        for (;;)
        {
                // motor_controller_print_stat();
                if (esp_connection_handle.remote_connected && DEBUG_TRANSMIT_PID_STATUS_TO_REMOTE)
                {
                        // esp_connection_disable_broadcast(&esp_connection_handle);
                        __unused motor_group_stat_pkt_t *stat = motor_controller_get_stat();
                        espnow_send_data(&espnow_send_param, ESPNOW_PARAM_TYPE_MOTOR_STAT, stat, sizeof(motor_group_stat_pkt_t)); // DEBUG ONLY
                }
                vTaskDelay(pdMS_TO_TICKS(100));
        }
}

void ping_task()
{
        for (;;)
        {
                esp_connection_send_heartbeat(&esp_connection_handle);
                vTaskDelay(pdMS_TO_TICKS(300));
        }
}

void power_switch_task()
{
        static int64_t time_us = 0;
        for (;;)
        {
                if (esp_connection_handle.remote_connected)
                        time_us = esp_timer_get_time();
                int64_t duration = esp_timer_get_time() - time_us;
                (duration >= constrain(IDLE_SHUTDOWN_SECONDS, 10, 100) * ONE_SECOND_IN_US) ? kill_power() : keep_power();

                if (SHOW_CONNECTION_STATUS)
                        esp_connection_show_entries(&esp_connection_handle);
                vTaskDelay(pdMS_TO_TICKS(3000));
        }
}

void app_main(void)
{
        config_wake_gpio();

        // Initialize NVS
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
                LOG_WARNING("Resetting EEPROM data!");
                // NVS partition was truncated and needs to be erased
                // Retry nvs_flash_init
                ESP_ERROR_CHECK(nvs_flash_erase());
                err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);

        device_settings_init(&device_settings);

        espnow_config_t espnow_config;
        espnow_wifi_default_config(&espnow_config);
        espnow_wifi_init(&espnow_config);
        espnow_default_send_param(&espnow_send_param);
        esp_connection_handle_init(&esp_connection_handle);
        // LOG_ERROR("&device_settings = %p", &device_settings);
        esp_connection_handle_connect_to_device_settings(&esp_connection_handle, &device_settings);
        esp_connection_set_peer_limit(&esp_connection_handle, 2);
        QueueHandle_t espnow_event_queue = espnow_init(&espnow_config, &esp_connection_handle);
        esp_connection_enable_broadcast(&esp_connection_handle);

        esp_connection_mac_add_to_entry(&esp_connection_handle, device_settings.remote_conn_mac);
        espnow_default_send_param(&espnow_send_param);

        err = espnow_send_text(&espnow_send_param, "device init");
        if (err != ESP_OK)
        {
                ESP_ERROR_CHECK_WITHOUT_ABORT(err);
                espnow_deinit(&espnow_send_param);
                vTaskDelete(NULL);
        }

        motor_controller_default_config(&motor_controller_handle);
        motor_controller_init(&motor_controller_handle);
        motor_controller_set_mcpwm_enable(&motor_controller_handle);

#if (HAS_GOALKEEPER_MODULE == true)
        goalkeeper_controller_handle_t goalkeeper_controller_handle;
        goalkeeper_controller_default_config(&goalkeeper_controller_handle);
        goalkeeper_controller_init(&goalkeeper_controller_handle);
#endif

#if (HAS_CATAPULT_MODULE == true)
        catapult_controller_handle_t catapult_controller_handle;
        catapult_controller_default_config(&catapult_controller_handle);
        catapult_controller_init(&catapult_controller_handle);
#endif

        xTaskCreate(rssi_task, "rssi_task", 4096, NULL, 4, NULL);
        xTaskCreate(ping_task, "ping_task", 4096, NULL, 4, NULL);
        xTaskCreate(info_task, "info_task", 4096, NULL, 4, NULL);
        xTaskCreate(power_switch_task, "power_switch_task", 4096, NULL, 4, NULL);

        motor_controller_stop_all(&motor_controller_handle);
        esp_connection_set_unique_peer_mac(&esp_connection_handle, device_settings.remote_conn_mac);

        while (true)
        {
                // read the events and write to console
                button_event_t remote_button_event = {0};

                espnow_event_t espnow_evt;
                while (xQueueReceive(espnow_event_queue, &espnow_evt, 0))
                {
                        espnow_data_t *recv_data = NULL;
                        switch (espnow_evt.id)
                        {
                        case ESPNOW_SEND_CB:
                                espnow_event_send_cb_t *send_cb = &espnow_evt.info.send_cb;
                                if (send_cb->status != ESP_NOW_SEND_SUCCESS)
                                {
                                        LOG_WARNING("Send data to peer " MACSTR " failed", MAC2STR(send_cb->mac_addr));
                                }
                                else
                                {
                                        LOG_VERBOSE("Send data to peer " MACSTR " success", MAC2STR(send_cb->mac_addr));
                                }
                                // ESP_LOGV(TAG, "Send data to " MACSTR ", status1: %4s", MAC2STR(send_cb->mac_addr), send_cb->status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
                                break;
                        case ESPNOW_RECV_CB:
                                espnow_event_recv_cb_t *recv_cb = &espnow_evt.info.recv_cb;
                                if (!(recv_data = espnow_data_parse(recv_data, recv_cb)))
                                {
                                        LOG_WARNING("bad data packet from peer " MACSTR, MAC2STR(recv_cb->mac_addr));
                                        free(recv_cb->data);
                                        break;
                                }

                                esp_peer_t *peer = esp_connection_mac_add_to_entry(&esp_connection_handle, recv_cb->mac_addr);
                                espnow_get_send_param(&espnow_send_param, peer);
                                esp_peer_process_received(peer, recv_data);

                                LOG_VERBOSE("peer " MACSTR " is %s", MAC2STR(recv_cb->mac_addr), recv_data->broadcast ? "BROADCAST" : "UNICAST");

                                if (recv_data->broadcast == ESPNOW_DATA_UNICAST)
                                {
                                        if (recv_data->len == sizeof(button_event_t))
                                                memcpy(&remote_button_event, recv_data->payload, recv_data->len);
                                        // print_mem(recv_data->payload, recv_data->len);
                                }

                                // if (recv_data->type != ESPNOW_PARAM_TYPE_ACK)
                                // espnow_reply(&espnow_send_param);

                                free(recv_cb->data);

                                break;
                        default:
                                LOG_ERROR("Callback type error: %d", espnow_evt.id);
                                break;
                        }
                }

                if (esp_connection_handle.remote_connected == 0)
                {
                        motor_controller_stop_all(&motor_controller_handle);
                }
                else
                {
#if (CAR_USE_PID_CONTROL == true)
                        motor_controller(&motor_controller_handle, &remote_button_event);
#else
                        motor_controller_openloop(&motor_controller_handle, &remote_button_event);
#endif

#if (HAS_GOALKEEPER_MODULE == true)
                        goalkeeper_controller(&goalkeeper_controller_handle, &remote_button_event);
#endif

#if (HAS_CATAPULT_MODULE == true)
                        catapult_controller(&catapult_controller_handle, &remote_button_event);
#endif
                }
                esp_connection_handle_update(&esp_connection_handle);
                heap_caps_check_integrity_all(true);
                vTaskDelay(pdMS_TO_TICKS(10));
        }
}
