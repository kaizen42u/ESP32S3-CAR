
#include "espnow.h"

static const char *TAG = "espnow";

QueueHandle_t espnow_queue;
static const uint8_t broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint16_t espnow_seq[2] = {0, 0}; // [0] for Tx, [1] for Rx
static esp_connection_handle_t *esp_connection_handle;
static espnow_wifi_config_t *espnow_config;

espnow_wifi_config_t *espnow_wifi_default_config(espnow_wifi_config_t *config)
{
        if (config == NULL)
        {
                LOG_ERROR("NULL pointer, config=0x%X", (uintptr_t)config);
                return NULL;
        }
        char pmk[] = "pmk1234567890123";
        char lmk[] = "lmk1234567890123";
        config->mode = WIFI_MODE_AP;
        config->wifi_interface = WIFI_IF_AP;
        config->wifi_phy_rate = WIFI_PHY_RATE_LORA_250K;
        config->esp_interface = ESP_IF_WIFI_AP;
        config->channel = 1;
        config->long_range = true;
        config->lmk = lmk;
        config->pmk = pmk;
        espnow_config = config;
        return config;
}

espnow_send_param_t *espnow_get_default_send_param(espnow_send_param_t *send_param)
{
        if (send_param == NULL)
        {
                LOG_ERROR("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return NULL;
        }
        memset(send_param, 0, sizeof(espnow_send_param_t));
        send_param->broadcast = ESPNOW_DATA_BROADCAST;
        memcpy(send_param->dest_mac, broadcast_mac, ESP_NOW_ETH_ALEN);
        return send_param;
}

/* WiFi should start before using ESPNOW */
void espnow_wifi_init(espnow_wifi_config_t *espnow_config)
{
        if (espnow_config == NULL)
        {
                LOG_ERROR("NULL pointer, config=0x%X", (uintptr_t)espnow_config);
                return;
        }
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_set_mode(espnow_config->mode));
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_set_channel(espnow_config->channel, WIFI_SECOND_CHAN_NONE));

        if (espnow_config->long_range)
                ESP_ERROR_CHECK(esp_wifi_set_protocol(espnow_config->esp_interface, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
}

void espnow_deinit(espnow_send_param_t *send_param)
{
        if (send_param != NULL)
        {
                if (send_param->buffer != NULL)
                {
                        free(send_param->buffer);
                }
                else
                {
                        LOG_WARNING("NULL pointer, send_param->buffer=0x%X", (uintptr_t)send_param->buffer);
                }
                free(send_param);
        }
        else
        {
                LOG_WARNING("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
        }

        if (espnow_queue != NULL)
        {
                vQueueDelete(espnow_queue);
                espnow_queue = NULL;
        }
        else
        {
                LOG_WARNING("NULL queue, espnow_queue=0x%X", (uintptr_t)espnow_queue);
        }
        esp_now_deinit();
}

/* ESPNOW sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */
static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
        espnow_event_t evt;
        espnow_event_send_cb_t *send_cb = &evt.info.send_cb;

        if (mac_addr == NULL)
        {
                LOG_ERROR("Send callback argument error, mac_addr=0x%X", (uintptr_t)mac_addr);
                return;
        }

        evt.id = ESPNOW_SEND_CB;
        memcpy(send_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
        send_cb->status = status;
        if (xQueueSend(espnow_queue, &evt, 0) != pdTRUE)
        {
                LOG_WARNING("Send callback failed to send queue");
        }
}

static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
        espnow_event_t evt;
        espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;
        uint8_t *mac_addr = recv_info->src_addr;

        if (mac_addr == NULL || data == NULL || len <= 0)
        {
                LOG_ERROR("Receive callback argument error, mac_addr=0x%X, data=0x%X, len=%d", (uintptr_t)mac_addr, (uintptr_t)data, len);
                return;
        }

        evt.id = ESPNOW_RECV_CB;
        memcpy(recv_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
        recv_cb->data = malloc(len + 1);
        if (recv_cb->data == NULL)
        {
                LOG_WARNING("malloc failed");
                return;
        }
        memcpy(recv_cb->data, data, len);
        recv_cb->data[len] = '\0';
        recv_cb->data_len = len;
        if (xQueueSend(espnow_queue, &evt, 0) != pdTRUE)
        {
                LOG_WARNING("Receive callback failed to send queue");
                free(recv_cb->data);
        }
}

/* Parse received ESPNOW data. */
espnow_packet_t *espnow_data_parse(espnow_packet_t *recv_data, espnow_event_recv_cb_t *recv_cb)
{
        if (recv_cb == NULL)
        {
                LOG_ERROR("NULL pointer, recv_cb=0x%X", (uintptr_t)recv_cb);
                return NULL;
        }

        size_t recv_data_min_len = sizeof(espnow_packet_t);
        if (recv_cb->data_len < recv_data_min_len)
        {
                LOG_WARNING("Received ESP-NOW data too short, len:%d<min:%d", recv_cb->data_len, recv_data_min_len);
                return NULL;
        }

        // if (recv_cb->data) print_mem(recv_cb->data, recv_cb->data_len);

        recv_data = (espnow_packet_t *)recv_cb->data;
        if (recv_data == NULL)
        {
                LOG_ERROR("NULL pointer, recv_data=0x%X", (uintptr_t)recv_data);
                return NULL;
        }

        if (recv_data->len > (recv_cb->data_len - recv_data_min_len))
        {
                LOG_WARNING("Received ESP-NOW data length mismatch, len:%d!=header:%d", recv_data->len, recv_cb->data_len - recv_data_min_len);
                print_mem(recv_data->payload, recv_data->len);
                return NULL;
        }

        uint16_t crc, crc_cal = 0;
        crc = recv_data->crc;
        recv_data->crc = 0;
        crc_cal = esp_crc16_le(UINT16_MAX, (uint8_t const *)recv_cb->data, recv_cb->data_len);
        if (crc_cal != crc)
        {
                LOG_WARNING("Received ESP-NOW data CRC error, crc:%04X!=crc_cal:%04X", crc, crc_cal);
                return NULL;
        }
        recv_data->crc = crc;

        // print_mem(recv_data->payload, recv_data->len);
        return recv_data;
}

espnow_send_param_t *espnow_payload_create(espnow_send_param_t *send_param, void *data, size_t len)
{
        if (send_param == NULL)
        {
                LOG_ERROR("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return NULL;
        }

        send_param->len = sizeof(espnow_packet_t) + len;
        send_param->buffer = malloc(send_param->len);
        if (send_param->buffer == NULL)
        {
                LOG_WARNING("malloc failed");
                return NULL;
        }

        espnow_packet_t *packet = (espnow_packet_t *)send_param->buffer;
        if (packet == NULL)
        {
                LOG_ERROR("NULL pointer, packet=0x%X", (uintptr_t)packet);
                return NULL;
        }

        packet->salt = esp_random();
        packet->type = send_param->type;
        packet->broadcast = send_param->broadcast;
        packet->seq_num = send_param->seq_num;
        packet->len = len;
        memcpy(packet->payload, data, len);
        packet->crc = 0;
        packet->crc = esp_crc16_le(UINT16_MAX, (uint8_t const *)send_param->buffer, send_param->len);
        return send_param;
}

espnow_send_param_t *espnow_payload_cleanup(espnow_send_param_t *send_param)
{
        if (send_param == NULL)
        {
                LOG_WARNING("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return NULL;
        }
        if (send_param->buffer == NULL)
        {
                LOG_WARNING("NULL pointer, send_param->buffer=0x%X", (uintptr_t)send_param->buffer);
                return NULL;
        }

        free(send_param->buffer);
        send_param->buffer = NULL;
        send_param->len = 0;
        return send_param;
}

esp_err_t espnow_send_data(espnow_send_param_t *send_param, espnow_packet_type_t type, void *data, size_t len)
{
        esp_err_t err;
        if (send_param == NULL)
        {
                LOG_WARNING("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return ESP_ERR_INVALID_ARG;
        }

        esp_peer_handle_t *peer = esp_connection_mac_lookup(esp_connection_handle, send_param->dest_mac);

        if (esp_connection_count_unique_peer(esp_connection_handle) > 0 && !peer->is_unique)
                return ESP_OK;

        if (peer == NULL)
        {
                send_param->seq_num = espnow_seq[ESPNOW_PARAM_SEQ_TX]++;
        }
        else
        {
                send_param->seq_num = peer->seq_tx;
                peer->seq_tx++;
                peer->lastsent_unicast_us = esp_timer_get_time();
        }
        esp_err_t ret;
        send_param->type = type;
        espnow_payload_create(send_param, data, len);
        espnow_packet_t *packet = (espnow_packet_t *)send_param->buffer;
        if (packet == NULL)
        {
                LOG_WARNING("NULL pointer, packet=0x%X", (uintptr_t)packet);
                return ESP_ERR_INVALID_STATE;
        }

        if (peer->registered == false)
        {
                esp_now_peer_info_t peer_info = {
                    .channel = espnow_config->channel,
                    .encrypt = false,
                    .ifidx = espnow_config->esp_interface,
                };
                memcpy(peer_info.peer_addr, peer->mac, ESP_NOW_ETH_ALEN);
                err = esp_now_add_peer(&peer_info);
                if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST)
                        ESP_ERROR_CHECK(err);
                peer->registered = true;
        }

        LOG_VERBOSE("Send %s to " MACSTR " , seq:%d, len:%d", ESPNOW_PACKET_TYPE_STRING[send_param->type], MAC2STR(send_param->dest_mac), packet->seq_num, packet->len);
        ret = esp_now_send(send_param->dest_mac, send_param->buffer, send_param->len);
        espnow_payload_cleanup(send_param);
        return ret;
}

esp_err_t espnow_send_text(espnow_send_param_t *send_param, char *text)
{
        return espnow_send_data(send_param, ESPNOW_PACKET_TYPE_TEXT, text, strlen(text));
}

esp_err_t espnow_send_reply(espnow_send_param_t *send_param)
{
        return ESP_OK;
        // return espnow_send_data(send_param, ESPNOW_PACKET_TYPE_ACK, 0, 0);
}

QueueHandle_t espnow_init(espnow_wifi_config_t *espnow_config, esp_connection_handle_t *conn_handle)
{
        if ((espnow_config == NULL) || (conn_handle == NULL))
        {
                LOG_ERROR("NULL pointer, espnow_config=0x%X, conn_handle=0x%X", (uintptr_t)espnow_config, (uintptr_t)conn_handle);
                return NULL;
        }

        esp_connection_handle = conn_handle;
        espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));
        if (espnow_queue == NULL)
        {
                LOG_ERROR("Create queue failed");
                return NULL;
        }

        /* Initialize ESPNOW and register sending and receiving callback function. */
        ESP_ERROR_CHECK(esp_now_init());
        ESP_ERROR_CHECK(esp_wifi_config_espnow_rate(espnow_config->wifi_interface, espnow_config->wifi_phy_rate));
        ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
        ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
#if CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE
        ESP_ERROR_CHECK(esp_now_set_wake_window(65535));
#endif
        /* Set primary master key. */
        ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *)espnow_config->pmk));

        /* Add broadcast peer information to peer list. */
        esp_now_peer_info_t peer_info = {
            .channel = espnow_config->channel,
            .encrypt = false,
            .ifidx = espnow_config->esp_interface,
        };
        memcpy(peer_info.peer_addr, broadcast_mac, ESP_NOW_ETH_ALEN);
        ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
        esp_peer_handle_t *peer = esp_connection_mac_add_to_entry(esp_connection_handle, peer_info.peer_addr);
        peer->registered = true;
        return espnow_queue;
}

espnow_send_param_t *espnow_get_send_param_broadcast(espnow_send_param_t *send_param)
{
        if (send_param == NULL)
        {
                LOG_ERROR("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return NULL;
        }

        send_param->broadcast = ESPNOW_DATA_BROADCAST;
        memcpy(send_param->dest_mac, broadcast_mac, ESP_NOW_ETH_ALEN);
        return send_param;
}

espnow_send_param_t *espnow_get_send_param_unicast(espnow_send_param_t *send_param, const uint8_t *mac)
{
        if (send_param == NULL)
        {
                LOG_ERROR("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return NULL;
        }
        send_param->broadcast = ESPNOW_DATA_UNICAST;
        memcpy(send_param->dest_mac, mac, ESP_NOW_ETH_ALEN);
        return send_param;
}

espnow_send_param_t *espnow_get_send_param(espnow_send_param_t *send_param, esp_peer_handle_t *peer)
{
        if (send_param == NULL)
        {
                LOG_ERROR("NULL pointer, send_param=0x%X", (uintptr_t)send_param);
                return NULL;
        }

        if (peer == NULL)
                return espnow_get_default_send_param(send_param);

        if (memcmp(peer->mac, broadcast_mac, ESP_NOW_ETH_ALEN) == 0)
                return espnow_get_send_param_broadcast(send_param);

        return espnow_get_send_param_unicast(send_param, peer->mac);
}

void esp_connection_handle_init(esp_connection_handle_t *handle)
{
        if (handle == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X", (uintptr_t)handle);
                return;
        }

        handle->size = 0;
        handle->limit = -1;
        handle->remote_connected = false;
        handle->entries = malloc(sizeof(esp_peer_handle_t));
        if (handle->entries == NULL)
        {
                LOG_ERROR("malloc failed, cannot create node entries");
                return;
        }
}

void esp_connection_handle_connect_to_device_settings(esp_connection_handle_t *handle, device_settings_t *device_settings)
{
        if ((handle == NULL) || device_settings == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X, device_settings=0x%X", (uintptr_t)handle, (uintptr_t)device_settings);
                return;
        }
        handle->device_settings = device_settings;
}

void esp_connection_handle_clear(esp_connection_handle_t *handle)
{
        if (handle == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X", (uintptr_t)handle);
                return;
        }
        if (handle->entries == NULL)
        {
                LOG_WARNING("NULL pointer, handle->entries=0x%X", (uintptr_t)handle->entries);
                return;
        }
        free(handle->entries);
}

void esp_connection_handle_update(esp_connection_handle_t *handle)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return;
        }

        bool have_unique_peer = esp_connection_count_unique_peer(handle);
        if (have_unique_peer)
                esp_connection_purge_non_unique_peers(handle);

        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X, index=%d", (uintptr_t)peer, i);
                        continue;
                }

                espnow_send_param_t send_param;
                switch (peer->status)
                {
                case ESP_PEER_STATUS_UNKNOWN:
                case ESP_PEER_STATUS_PROTOCOL_ERROR:
                case ESP_PEER_STATUS_NOREPLY:
                case ESP_PEER_STATUS_IN_RANGE:
                        if (esp_timer_get_time() - peer->lastseen_broadcast_us > ONE_SECOND_IN_US)
                                if (esp_timer_get_time() - peer->lastseen_unicast_us > ONE_SECOND_IN_US)
                                        esp_peer_set_status(peer, ESP_PEER_STATUS_LOST);
                        break;
                case ESP_PEER_STATUS_CONNECTED:
                        if (!peer->saved_to_rom)
                        {
                                device_settings_set_mac(handle->device_settings, peer->mac);
                                esp_connection_set_unique_peer_mac(handle, peer->mac);
                                peer->saved_to_rom = true;
                        }
                        if (esp_timer_get_time() - peer->lastseen_unicast_us > ONE_SECOND_IN_US)
                                esp_peer_set_status(peer, ESP_PEER_STATUS_LOST);
                        break;
                case ESP_PEER_STATUS_CONNECTING:
                        if (esp_timer_get_time() - peer->last_ping_us > 3 * 1e5)
                        {
                                peer->last_ping_us = esp_timer_get_time();
                                espnow_get_default_send_param(&send_param);
                                espnow_get_send_param_unicast(&send_param, peer->mac);
                                peer->lastseen_unicast_us = esp_timer_get_time();
                                espnow_send_data(&send_param, ESPNOW_PACKET_TYPE_CONNECT, NULL, 0);
                        }

                        if (esp_timer_get_time() - peer->connect_time_us > ONE_SECOND_IN_US)
                                esp_peer_set_status(peer, ESP_PEER_STATUS_NOREPLY);
                        break;
                case ESP_PEER_STATUS_AVAILABLE:
                        if ((handle->limit != -1) && (handle->remote_connected >= handle->limit))
                        {
                                esp_peer_set_status(peer, ESP_PEER_STATUS_REJECTED);
                                break;
                        }
                        peer->connect_time_us = esp_timer_get_time();
                        peer->lastseen_unicast_us = esp_timer_get_time();
                        espnow_get_default_send_param(&send_param);
                        espnow_get_send_param_unicast(&send_param, peer->mac);
                        espnow_send_data(&send_param, ESPNOW_PACKET_TYPE_CONNECT, NULL, 0);
                        esp_peer_set_status(peer, ESP_PEER_STATUS_CONNECTING);
                        break;
                case ESP_PEER_STATUS_LOST:
                case ESP_PEER_STATUS_REJECTED:
                case ESP_PEER_STATUS_MAX:
                        if (peer->is_unique)
                                esp_peer_set_status(peer, ESP_PEER_STATUS_AVAILABLE);
                        break;
                }
        }
        handle->remote_connected = esp_connection_count_connected(handle);
}

void esp_connection_update_rssi(esp_connection_handle_t *handle, const rssi_event_t *rssi_event)
{
        if ((handle == NULL) || (handle->entries == NULL) || (rssi_event == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X, rssi_event=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries, (uintptr_t)rssi_event);
                return;
        }

        esp_peer_handle_t *peer = esp_connection_mac_add_to_entry(handle, rssi_event->recv_mac);
        peer->rssi = rssi_event->rssi;

        const int rssi_min = MIN_RSSI_TO_INITIATE_CONNECTION;
        if (rssi_event->rssi > rssi_min)
        {
                if (peer->status == ESP_PEER_STATUS_CONNECTED)
                        peer->lastseen_unicast_us = esp_timer_get_time();

                if (peer != NULL && peer->status == ESP_PEER_STATUS_IN_RANGE)
                {
                        peer->lastseen_broadcast_us = esp_timer_get_time();
                        esp_peer_set_status(peer, ESP_PEER_STATUS_AVAILABLE);

                        /* Add unicast peer information to peer list. */
                        if (!esp_now_is_peer_exist(peer->mac))
                        {
                                esp_now_peer_info_t peer_info = {
                                    .channel = espnow_config->channel,
                                    .encrypt = false,
                                    .ifidx = espnow_config->esp_interface,
                                };
                                memcpy(peer_info.peer_addr, peer->mac, ESP_NOW_ETH_ALEN);
                                ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
                                peer->registered = true;
                        }
                }
        }

        if (peer != NULL && peer->is_unique && peer->status == ESP_PEER_STATUS_IN_RANGE)
                esp_peer_set_status(peer, ESP_PEER_STATUS_AVAILABLE);
}

bool esp_mac_check_equals(const uint8_t *mac1, const uint8_t *mac2)
{
        if ((mac1 == NULL) || (mac2 == NULL))
        {
                LOG_ERROR("NULL pointer, mac1=0x%X, mac2=0x%X", (uintptr_t)mac1, (uintptr_t)mac2);
                return false;
        }

        for (uint8_t i = 0; i < ESP_NOW_ETH_ALEN; i++)
                if (mac1[i] != mac2[i])
                        return false;
        return true;
}

size_t esp_connection_count_connected(esp_connection_handle_t *handle)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return 0;
        }

        size_t count = 0;
        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X, index=%d", (uintptr_t)peer, i);
                        return 0;
                }

                if (peer->status == ESP_PEER_STATUS_CONNECTED)
                        count++;
        }
        return count;
}

size_t esp_connection_count_unique_peer(esp_connection_handle_t *handle)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return 0;
        }

        size_t count = 0;
        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X, index=%d", (uintptr_t)peer, i);
                        return 0;
                }

                if (peer->is_unique)
                        count++;
        }
        return count;
}

esp_peer_handle_t *esp_connection_mac_lookup(esp_connection_handle_t *handle, const uint8_t *mac)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer");
                return NULL;
        }
        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X, index=%d", (uintptr_t)peer, i);
                        return NULL;
                }

                if (esp_mac_check_equals(peer->mac, mac))
                        return peer;
        }
        return NULL;
}

void esp_connection_peer_init(esp_peer_handle_t *peer, const uint8_t *mac)
{
        if (peer == NULL)
        {
                LOG_ERROR("NULL pointer, peer=0x%X", (uintptr_t)peer);
                return;
        }
        memset(peer, 0, sizeof(esp_peer_handle_t));
        memcpy(peer->mac, mac, ESP_NOW_ETH_ALEN);
        peer->conn_retry = 0;
        peer->lastseen_broadcast_us = esp_timer_get_time();
        peer->lastseen_unicast_us = esp_timer_get_time();
        peer->seq_rx = 0;
        peer->seq_tx = 0;
        peer->rssi = -200;
        peer->status = ESP_PEER_STATUS_UNKNOWN;
        peer->registered = false;
        peer->is_unique = false;
        peer->saved_to_rom = false;
}

esp_peer_handle_t *esp_connection_mac_add_to_entry(esp_connection_handle_t *handle, const uint8_t *mac)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return NULL;
        }

        esp_peer_handle_t *peer = esp_connection_mac_lookup(handle, mac);
        if (peer != NULL)
        {
                LOG_VERBOSE("Peer " MACSTR " already logged", MAC2STR(mac));
                // print_mem(peer, sizeof(esp_peer_handle_t));
                return peer;
        }

        size_t new_capacity = handle->size + 1;
        esp_peer_handle_t *new_addr = realloc(handle->entries, sizeof(esp_peer_handle_t) * new_capacity);
        if (new_addr == NULL)
        {
                LOG_ERROR("realloc failed, cannot add peer " MACSTR "to node list", MAC2STR(mac));
                return NULL;
        }

        handle->entries = new_addr;
        esp_peer_handle_t *new_peer = handle->entries + handle->size;
        if (peer != NULL)
        {
                LOG_ERROR("NULL pointer, new_peer=0x%X", (uintptr_t)new_peer);
                // print_mem(peer, sizeof(esp_peer_handle_t));
                return peer;
        }

        esp_connection_peer_init(new_peer, mac);
        handle->size = new_capacity;
        LOG_INFO("Added " MACSTR " to known node, total: %d", MAC2STR(mac), handle->size);
        esp_connection_show_entries(handle);
        // print_mem(new_peer, sizeof(esp_peer_handle_t));
        return new_peer;
}

void esp_connection_show_entries(esp_connection_handle_t *handle)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return;
        }

        LOG_INFO("Listing available ESP-NOW nodes, %d total", handle->size);
        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X", (uintptr_t)peer);
                        return;
                }
                LOG_INFO("    id: %d, addr: " MACSTR ", rssi: %4d, status: %s, unique: %d", i, MAC2STR(peer->mac), peer->rssi, ESP_PEER_STATUS_STRING[peer->status], peer->is_unique);
        }
        if (handle->size == 0)
        {
                LOG_INFO("    <Empty>");
        }
}

void esp_connection_set_peer_limit(esp_connection_handle_t *handle, int8_t new_limit)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return;
        }
        handle->limit = new_limit;
}

void esp_connection_set_unique_peer_mac(esp_connection_handle_t *handle, const uint8_t *mac)
{
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return;
        }
        if (memcmp(mac, broadcast_mac, ESP_NOW_ETH_ALEN) == 0)
                return;

        LOG_WARNING("Setting peer MAC " MACSTR " as unique peer", MAC2STR(mac));
        esp_peer_handle_t *peer = esp_connection_mac_add_to_entry(handle, mac);
        peer->is_unique = true;
}

void esp_peer_set_status(esp_peer_handle_t *peer, esp_peer_status_t new_status)
{
        if (peer == NULL)
        {
                LOG_ERROR("NULL pointer, peer=0x%X", (uintptr_t)peer);
                return;
        }
        if (new_status == ESP_PEER_STATUS_CONNECTED)
                LOG_INFO("peer " MACSTR " connected!", MAC2STR(peer->mac));
        if (peer->status == ESP_PEER_STATUS_CONNECTED && new_status == ESP_PEER_STATUS_LOST)
                LOG_WARNING("peer " MACSTR " disconnected!", MAC2STR(peer->mac));
        LOG_INFO("peer " MACSTR " status [%s --> %s]", MAC2STR(peer->mac), ESP_PEER_STATUS_STRING[peer->status], ESP_PEER_STATUS_STRING[new_status]);
        peer->status = new_status;
}

void esp_peer_process_received(esp_peer_handle_t *peer, espnow_packet_t *recv_data)
{
        if ((peer == NULL) || (recv_data == NULL))
        {
                LOG_ERROR("NULL pointer, peer=0x%X, recv_data=0x%X", (uintptr_t)peer, (uintptr_t)recv_data);
                return;
        }

        espnow_send_param_t send_param;
        espnow_get_send_param(&send_param, peer);

        if (recv_data->type == ESPNOW_PACKET_TYPE_ACK)
        {
                LOG_VERBOSE("packet id:[%04d] acknowledged from peer " MACSTR, recv_data->seq_num, MAC2STR(peer->mac));
                return;
        }

        if (peer->status < ESP_PEER_STATUS_IN_RANGE)
                esp_peer_set_status(peer, ESP_PEER_STATUS_IN_RANGE);

        if (recv_data->broadcast == ESPNOW_DATA_BROADCAST)
        {
                peer->lastseen_broadcast_us = esp_timer_get_time();
                espnow_send_reply(&send_param);
                LOG_VERBOSE("Receive %dth broadcast data from: " MACSTR ", len: %d",
                            recv_data->seq_num,
                            MAC2STR(peer->mac),
                            recv_data->len);
                // print_mem(recv_data->payload, recv_data->len);
        }
        else if (recv_data->broadcast == ESPNOW_DATA_UNICAST)
        {
                peer->lastseen_unicast_us = esp_timer_get_time();
                espnow_send_reply(&send_param);
                peer->lastsent_unicast_us = esp_timer_get_time();
                if (peer->status == ESP_PEER_STATUS_CONNECTING)
                {
                        esp_peer_set_status(peer, ESP_PEER_STATUS_CONNECTED);
                }
                LOG_VERBOSE("Receive %dth unicast data from: " MACSTR ", len: %d",
                            recv_data->seq_num,
                            MAC2STR(peer->mac),
                            recv_data->len);
                // print_mem(recv_data->payload, recv_data->len);
        }
        else
        {
                LOG_WARNING("Receive error data from: " MACSTR "", MAC2STR(peer->mac));
        }
}

void esp_connection_send_heartbeat(esp_connection_handle_t *handle)
{
        static espnow_send_param_t send_param;

        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return;
        }

        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X", (uintptr_t)peer);
                        return;
                }

                // if (peer->registered == true)
                if (peer->status >= ESP_PEER_STATUS_CONNECTING || peer->is_unique)
                {
                        LOG_VERBOSE("Sending heartbeat to peer " MACSTR, MAC2STR(peer->mac));
                        espnow_get_send_param(&send_param, peer);
                        send_param.broadcast = ESPNOW_DATA_UNICAST;
                        espnow_send_text(&send_param, "ping");
                }
        }
}

void esp_connection_enable_broadcast(esp_connection_handle_t *handle)
{
        esp_err_t err;
        esp_peer_handle_t *peer = esp_connection_mac_lookup(handle, broadcast_mac);
        esp_now_peer_info_t peer_info = {
            .channel = espnow_config->channel,
            .encrypt = false,
            .ifidx = espnow_config->esp_interface,
        };
        memcpy(peer_info.peer_addr, peer->mac, ESP_NOW_ETH_ALEN);
        err = esp_now_add_peer(&peer_info);
        if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST)
                ESP_ERROR_CHECK(err);
        peer->registered = true;
        peer->status = ESP_PEER_STATUS_UNKNOWN;
}

void esp_connection_disable_broadcast(esp_connection_handle_t *handle)
{
        LOG_WARNING("DISABLED BROADCAST MAC ADDRESS");
        esp_err_t err;
        esp_peer_handle_t *peer = esp_connection_mac_lookup(handle, broadcast_mac);
        err = esp_now_del_peer(peer->mac);
        if (err != ESP_OK && err != ESP_ERR_ESPNOW_NOT_FOUND)
                ESP_ERROR_CHECK(err);
        peer->registered = false;
        peer->status = ESP_PEER_STATUS_REJECTED;
}

void esp_connection_purge_non_unique_peers(esp_connection_handle_t *handle)
{
        esp_err_t err;
        if ((handle == NULL) || (handle->entries == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->entries=0x%X", (uintptr_t)handle, (uintptr_t)handle->entries);
                return;
        }

        for (size_t i = 0; i < handle->size; i++)
        {
                esp_peer_handle_t *peer = handle->entries + i;
                if (peer == NULL)
                {
                        LOG_ERROR("NULL pointer, peer=0x%X", (uintptr_t)peer);
                        continue;
                }
                if (!esp_now_is_peer_exist(peer->mac))
                        continue;

                if (peer->is_unique)
                        continue;

                err = esp_now_del_peer(peer->mac);
                if (err != ESP_OK && err != ESP_ERR_ESPNOW_NOT_FOUND)
                        ESP_ERROR_CHECK(err);
                peer->registered = false;
                peer->status = ESP_PEER_STATUS_REJECTED;
        }
}