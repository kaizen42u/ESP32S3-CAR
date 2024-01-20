
#include "button_espnow_test.h"

static const char *TAG = "button_espnow_test";

void button_espnow_loop(void)
{
        // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
                ESP_ERROR_CHECK(nvs_flash_erase());
                ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        espnow_config_t espnow_config;
        espnow_wifi_default_config(&espnow_config);
        espnow_wifi_init(&espnow_config);
        QueueHandle_t espnow_event_queue = espnow_init();

        espnow_send_param_t espnow_send_param;
        espnow_default_send_param(&espnow_send_param);

        if (espnow_send_text(&espnow_send_param, "device init") != ESP_OK)
        {
                ESP_LOGE(TAG, "Send error, quitting");
                espnow_deinit(&espnow_send_param);
                vTaskDelete(NULL);
        }

        QueueHandle_t button_event_queue = button_init(GPIO_INPUT_PIN_SEL);

        while (true)
        {
                // read the events and write to console
                button_event_t button_event;
                while (xQueueReceive(button_event_queue, &button_event, 0))
                {
                        ESP_LOGI(TAG, "GPIO event: pin %d, event = %s", button_event.pin, BUTTON_STATE_STRING[button_event.event]);

                        char temp[64];
                        switch (button_event.pin)
                        {
                        case GPIO_BUTTON_UP:
                                snprintf(temp, 64, "GPIO_BUTTON_UP, %s", BUTTON_STATE_STRING[button_event.event]);
                                break;
                        case GPIO_BUTTON_DOWN:
                                snprintf(temp, 64, "GPIO_BUTTON_DOWN, %s", BUTTON_STATE_STRING[button_event.event]);
                                break;
                        case GPIO_BUTTON_LEFT:
                                snprintf(temp, 64, "GPIO_BUTTON_LEFT, %s", BUTTON_STATE_STRING[button_event.event]);
                                break;
                        case GPIO_BUTTON_RIGHT:
                                snprintf(temp, 64, "GPIO_BUTTON_RIGHT, %s", BUTTON_STATE_STRING[button_event.event]);
                                break;
                        default:
                                break;
                        }
                        espnow_send_text(&espnow_send_param, temp);
                }

                espnow_event_t espnow_evt;
                while (xQueueReceive(espnow_event_queue, &espnow_evt, 0))
                {
                        espnow_data_t *recv_data = NULL;
                        switch (espnow_evt.id)
                        {
                        case ESPNOW_SEND_CB:
                                espnow_event_send_cb_t *send_cb = &espnow_evt.info.send_cb;
                                ESP_LOGV(TAG, "Send data to " MACSTR ", status1: %4s", MAC2STR(send_cb->mac_addr), send_cb->status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
                                break;
                        case ESPNOW_RECV_CB:
                                espnow_event_recv_cb_t *recv_cb = &espnow_evt.info.recv_cb;
                                if (!(recv_data = espnow_data_parse(recv_data, recv_cb)))
                                {
                                        free(recv_cb->data);
                                        break;
                                }
                                if (recv_data->flags.broadcast == ESPNOW_DATA_BROADCAST)
                                {
                                        if (recv_data->flags.ack == ESPNOW_PARAM_ACK_ACK)
                                                ESP_LOGI(TAG, "Packet id:[%4d] acknowleded by receiver", recv_data->seq_num);
                                        else
                                        {
                                                espnow_reply(&espnow_send_param, recv_data);
                                                if (recv_data->flags.type == ESPNOW_PARAM_TYPE_DATA)
                                                        ESP_LOGI(TAG, "Receive %dth broadcast data from: " MACSTR ", len: %d, %s", recv_data->seq_num, MAC2STR(recv_cb->mac_addr), recv_cb->data_len, recv_data->payload);
                                                else
                                                {
                                                        ESP_LOGI(TAG, "Receive %dth broadcast text from: " MACSTR ", len: %d", recv_data->seq_num, MAC2STR(recv_cb->mac_addr), recv_cb->data_len);
                                                        print_mem(recv_data->payload, recv_data->len);
                                                }
                                        }
                                }
                                else if (recv_data->flags.broadcast == ESPNOW_DATA_UNICAST)
                                        ESP_LOGI(TAG, "Receive %dth unicast data from: " MACSTR ", len: %d", recv_data->seq_num, MAC2STR(recv_cb->mac_addr), recv_cb->data_len);
                                else
                                        ESP_LOGW(TAG, "Receive error data from: " MACSTR "", MAC2STR(recv_cb->mac_addr));
                                free(recv_cb->data);
                                break;
                        default:
                                ESP_LOGE(TAG, "Callback type error: %d", espnow_evt.id);
                                break;
                        }
                }

                vTaskDelay(1);
        }
}
