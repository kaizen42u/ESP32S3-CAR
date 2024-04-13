
#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_crc.h"
#include "nvs_flash.h"
#include "esp_timer.h"

#include "mem_probe.h"
#include "logging.h"
#include "rssi.h"
#include "device_settings.h"
#include "info.h"

#define ONE_SECOND_IN_US (1 * 1e6)

#define ESPNOW_QUEUE_SIZE (64)

typedef struct
{
        wifi_phy_rate_t wifi_phy_rate;
        wifi_interface_t wifi_interface;
        wifi_mode_t mode;
        esp_interface_t esp_interface;
        bool long_range;
        uint8_t channel;
        char *pmk;
        char *lmk;
} espnow_config_t;

typedef enum
{
        ESPNOW_SEND_CB,
        ESPNOW_RECV_CB,
} espnow_event_id_t;

typedef struct
{
        uint8_t mac_addr[ESP_NOW_ETH_ALEN];
        esp_now_send_status_t status : 8;
} __packed espnow_event_send_cb_t;

typedef struct
{
        uint8_t mac_addr[ESP_NOW_ETH_ALEN];
        size_t data_len;
        uint8_t *data;
} __packed espnow_event_recv_cb_t;

typedef union
{
        espnow_event_send_cb_t send_cb;
        espnow_event_recv_cb_t recv_cb;
} __packed espnow_event_info_t;

/* When ESPNOW sending or receiving callback function is called, post event to ESPNOW task. */
typedef struct
{
        espnow_event_id_t id : 8;
        espnow_event_info_t info;
} __packed espnow_event_t;

typedef enum
{
        ESPNOW_PARAM_TYPE_TEXT,
        ESPNOW_PARAM_TYPE_BATTERY_VOLTAGE,
        ESPNOW_PARAM_TYPE_MOTOR_STAT,
        ESPNOW_PARAM_TYPE_SYNC_RGB,
        ESPNOW_PARAM_TYPE_CAR_MOVEMENT,
        ESPNOW_PARAM_TYPE_CATAPULT_MOVEMENT,
        ESPNOW_PARAM_TYPE_KEEPER_MOVEMENT,
        ESPNOW_PARAM_TYPE_PING,
        ESPNOW_PARAM_TYPE_ACK,
        ESPNOW_PARAM_TYPE_NACK,
        ESPNOW_PARAM_TYPE_MAX,
} espnow_param_type_t;

static const char __attribute__((unused)) * ESPNOW_PARAM_TYPE_STRING[] = {
    "ESPNOW_PARAM_TYPE_TEXT",
    "ESPNOW_PARAM_TYPE_BATTERY_VOLTAGE",
    "ESPNOW_PARAM_TYPE_MOTOR_STAT",
    "ESPNOW_PARAM_TYPE_SYNC_RGB",
    "ESPNOW_PARAM_TYPE_CAR_MOVEMENT",
    "ESPNOW_PARAM_TYPE_CATAPULT_MOVEMENT",
    "ESPNOW_PARAM_TYPE_KEEPER_MOVEMENT",
    "ESPNOW_PARAM_TYPE_PING",
    "ESPNOW_PARAM_TYPE_ACK",
    "ESPNOW_PARAM_TYPE_NACK",
    "ESPNOW_PARAM_TYPE_MAX"};

typedef enum
{
        ESPNOW_PARAM_SEQ_TX,
        ESPNOW_PARAM_SEQ_RX,
} espnow_param_seq_t;

typedef enum
{
        ESPNOW_DATA_BROADCAST,
        ESPNOW_DATA_UNICAST,
} espnow_data_type_t;

/* User defined field of ESPNOW data. */
typedef struct
{
        uint16_t seq_num;             // Sequence number of ESPNOW data.
        uint16_t crc;                 // CRC16 value of ESPNOW data.
        espnow_data_type_t broadcast; // 0: broadcast, 1: unicast
        espnow_param_type_t type;     //
        uint8_t salt;                 // random bits
        uint8_t len;                  // Length of payload, unit: byte.
        uint8_t payload[0];           // Real payload of ESPNOW data.
} espnow_data_t;

/* Parameters of sending ESPNOW data. */
typedef struct
{
        espnow_data_type_t broadcast;       // Broadcast or unicast ESPNOW data.
        espnow_param_type_t type;           //
        uint16_t seq_num;                   // Sequence number of ESPNOW data.
        int len;                            // Length of ESPNOW data to be sent, unit: byte.
        uint8_t *buffer;                    // Buffer pointing to ESPNOW data.
        uint8_t dest_mac[ESP_NOW_ETH_ALEN]; // MAC address of destination device.
} espnow_send_param_t;

typedef enum
{
        ESP_PEER_STATUS_UNKNOWN,
        ESP_PEER_STATUS_LOST,
        ESP_PEER_STATUS_PROTOCOL_ERROR,
        ESP_PEER_STATUS_NOREPLY,
        ESP_PEER_STATUS_IN_RANGE,
        ESP_PEER_STATUS_AVAILABLE,
        ESP_PEER_STATUS_CONNECTING,
        ESP_PEER_STATUS_CONNECTED,
        ESP_PEER_STATUS_REJECTED,
        ESP_PEER_STATUS_MAX,
} esp_peer_status_t;

static const char __attribute__((unused)) * ESP_PEER_STATUS_STRING[] = {
    "ESP_PEER_STATUS_UNKNOWN",
    "ESP_PEER_STATUS_LOST",
    "ESP_PEER_STATUS_PROTOCOL_ERROR",
    "ESP_PEER_STATUS_NOREPLY",
    "ESP_PEER_STATUS_IN_RANGE",
    "ESP_PEER_STATUS_AVAILABLE",
    "ESP_PEER_STATUS_CONNECTING",
    "ESP_PEER_STATUS_CONNECTED",
    "ESP_PEER_STATUS_REJECTED",
    "ESP_PEER_STATUS_MAX"};

typedef enum
{
        ESP_PEER_PACKET_TEXT,
        ESP_PEER_PACKET_BATTERY_VOLTAGE,
        ESP_PEER_PACKET_SYNC_RGB,
        ESP_PEER_PACKET_CAR_MOVEMENT,
        ESP_PEER_PACKET_CATAPULT_MOVEMENT,
        ESP_PEER_PACKET_KEEPER_MOVEMENT,
        ESP_PEER_PACKET_PING,
        ESP_PEER_PACKET_CONNECT,
        ESP_PEER_PACKET_ACK,
        ESP_PEER_PACKET_NACK,
        ESP_PEER_PACKET_MAX,
} esp_peer_packet_type_t;

typedef struct
{
        uint8_t mac[ESP_NOW_ETH_ALEN];
        int64_t lastseen_broadcast_us;
        int64_t lastseen_unicast_us;
        int64_t lastsent_unicast_us;
        int64_t connect_time_us;
        int64_t last_ping_us;
        size_t conn_retry;
        size_t seq_rx;
        size_t seq_tx;
        esp_peer_status_t status;
        int rssi;
        bool registered;
        bool is_unique;
        bool saved_to_rom;
} esp_peer_t;

typedef struct
{
        device_settings_t *device_settings;
        esp_peer_t *entries;
        int8_t size;
        int8_t limit;
        int8_t remote_connected;
} esp_connection_handle_t;

espnow_config_t *espnow_wifi_default_config(espnow_config_t *config);
espnow_send_param_t *espnow_default_send_param(espnow_send_param_t *send_param);

void espnow_wifi_init(espnow_config_t *espnow_config);
QueueHandle_t espnow_init(espnow_config_t *espnow_config, esp_connection_handle_t *conn_handle);
void espnow_deinit(espnow_send_param_t *send_param);

espnow_data_t *espnow_data_parse(espnow_data_t *recv_data, espnow_event_recv_cb_t *recv_cb);

espnow_send_param_t *espnow_get_send_param_broadcast(espnow_send_param_t *send_param);
espnow_send_param_t *espnow_get_send_param_unicast(espnow_send_param_t *send_param, const uint8_t *mac);
espnow_send_param_t *espnow_get_send_param(espnow_send_param_t *send_param, esp_peer_t *peer);

esp_err_t espnow_send_data(espnow_send_param_t *send_param, espnow_param_type_t type, void *data, size_t len);
esp_err_t espnow_send_text(espnow_send_param_t *send_param, char *text);
esp_err_t espnow_reply(espnow_send_param_t *send_param);

void esp_connection_enable_broadcast(esp_connection_handle_t *handle);
void esp_connection_disable_broadcast(esp_connection_handle_t *handle);

void esp_connection_handle_init(esp_connection_handle_t *handle);
void esp_connection_handle_connect_to_device_settings(esp_connection_handle_t *handle, device_settings_t *device_settings);
void esp_connection_handle_clear(esp_connection_handle_t *handle);
void esp_connection_handle_update(esp_connection_handle_t *handle);
void esp_connection_update_rssi(esp_connection_handle_t *handle, const rssi_event_t *rssi_event);

size_t esp_connection_count_connected(esp_connection_handle_t *handle);
size_t esp_connection_count_unique_peer(esp_connection_handle_t *handle);
esp_peer_t *esp_connection_mac_lookup(esp_connection_handle_t *handle, const uint8_t *mac);
esp_peer_t *esp_connection_mac_add_to_entry(esp_connection_handle_t *handle, const uint8_t *mac);

void esp_connection_show_entries(esp_connection_handle_t *handle);
void esp_connection_send_heartbeat(esp_connection_handle_t *handle);

void esp_connection_set_peer_limit(esp_connection_handle_t *handle, int8_t new_limit);
void esp_connection_set_unique_peer_mac(esp_connection_handle_t *handle, const uint8_t *mac);
void esp_peer_set_status(esp_peer_t *peer, esp_peer_status_t new_status);
void esp_peer_process_received(esp_peer_t *peer, espnow_data_t *recv_data);
void esp_connection_purge_non_unique_peers(esp_connection_handle_t *handle);
