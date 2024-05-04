
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

// Configuration for the ESP-NOW
typedef struct
{
        wifi_phy_rate_t wifi_phy_rate;   // WiFi PHY data rate
        wifi_interface_t wifi_interface; // WiFi IF interface
        wifi_mode_t mode;                // WiFi mode
        esp_interface_t esp_interface;   // ESP IF interface
        bool long_range;                 // Enable long-range mode
        uint8_t channel;                 // WiFi channel, currently only supported channel is: `1`
        char *pmk;                       // Public master key
        char *lmk;                       // Local master key
} espnow_wifi_config_t;

// ESP-NOW queue date event types
typedef enum
{
        ESPNOW_SEND_CB, // Result of sending an ESP-NOW packet
        ESPNOW_RECV_CB, // Received valid ESP-NOW packet
} espnow_event_id_t;

// Result of sending an ESP-NOW packet
typedef struct
{
        uint8_t mac_addr[ESP_NOW_ETH_ALEN]; // Peer MAC address
        esp_now_send_status_t status : 8;   // Status of sending ESPNOW data
} __packed espnow_event_send_cb_t;

// Received valid ESP-NOW packet
typedef struct
{
        uint8_t mac_addr[ESP_NOW_ETH_ALEN]; // Peer MAC address
        size_t data_len;                    // Received data length, in bytes
        uint8_t *data;                      // Received data, in heap memory
} __packed espnow_event_recv_cb_t;

// ESP-NOW queue date event info
typedef union
{
        espnow_event_send_cb_t send_cb; // The event is a send callback event
        espnow_event_recv_cb_t recv_cb; // The event is a receive callback event
} __packed espnow_event_info_t;

// When ESPNOW sending or receiving callback function is called, post event to ESPNOW task
typedef struct
{
        espnow_event_id_t id : 8; // ESP-NOW queue date event types
        espnow_event_info_t info; // ESP-NOW queue date event info
} __packed espnow_event_t;

// ESP-NOW data packet types, as indicates in the header of the packet
typedef enum
{
        ESPNOW_PACKET_TYPE_TEXT,              // Text
        ESPNOW_PACKET_TYPE_BATTERY_VOLTAGE,   // Battery voltage
        ESPNOW_PACKET_TYPE_MOTOR_STAT,        // Motor + PID status
        ESPNOW_PACKET_TYPE_SYNC_RGB,          // `NOT IMPLEMENTED`
        ESPNOW_PACKET_TYPE_CAR_MOVEMENT,      // `NOT IMPLEMENTED`
        ESPNOW_PACKET_TYPE_CATAPULT_MOVEMENT, // `NOT IMPLEMENTED`
        ESPNOW_PACKET_TYPE_KEEPER_MOVEMENT,   // `NOT IMPLEMENTED`
        ESPNOW_PACKET_TYPE_PING,              // `NOT IMPLEMENTED`
        ESPNOW_PACKET_TYPE_ACK,               // Acknowledgment
        ESPNOW_PACKET_TYPE_NACK,              // `NOT IMPLEMENTED`
        ESPNOW_PACKET_TYPE_CONNECT,           // Request for connection
        ESPNOW_PACKET_TYPE_MAX,
} espnow_packet_type_t;

static const char __attribute__((unused)) * ESPNOW_PACKET_TYPE_STRING[] = {
    "ESPNOW_PACKET_TYPE_TEXT",
    "ESPNOW_PACKET_TYPE_BATTERY_VOLTAGE",
    "ESPNOW_PACKET_TYPE_MOTOR_STAT",
    "ESPNOW_PACKET_TYPE_SYNC_RGB",
    "ESPNOW_PACKET_TYPE_CAR_MOVEMENT",
    "ESPNOW_PACKET_TYPE_CATAPULT_MOVEMENT",
    "ESPNOW_PACKET_TYPE_KEEPER_MOVEMENT",
    "ESPNOW_PACKET_TYPE_PING",
    "ESPNOW_PACKET_TYPE_ACK",
    "ESPNOW_PACKET_TYPE_NACK",
    "ESPNOW_PACKET_TYPE_CONNECT",
    "ESPNOW_PACKET_TYPE_MAX"};

// ESP-NOW data packet sequence number
typedef enum
{
        ESPNOW_PARAM_SEQ_TX, // Transmit
        ESPNOW_PARAM_SEQ_RX, // Receive
} espnow_param_seq_t;

// ESP-NOW data packet sending method
typedef enum
{
        ESPNOW_DATA_BROADCAST, // Use broadcast, use address FF:FF:FF:FF:FF:FF
        ESPNOW_DATA_UNICAST,   // Use unicast, use peer MAC address
} espnow_packet_sending_method_t;

// ESP-NOW data packet content definition
typedef struct
{
        uint16_t seq_num;                         // Sequence number of ESP-NOW data.
        uint16_t crc;                             // CRC16 value of ESPNOW data.
        espnow_packet_sending_method_t broadcast; // 0: broadcast, 1: unicast
        espnow_packet_type_t type;                // Data packet types
        uint8_t salt;                             // Random bits
        uint8_t len;                              // Length of payload, unit: byte.
        uint8_t payload[0];                       // Payload of ESP-NOW data.
} espnow_packet_t;

// Parameters of sending ESPNOW data
typedef struct
{
        espnow_packet_sending_method_t broadcast; // Broadcast or unicast of ESP-NOW data.
        espnow_packet_type_t type;                // Data packet types
        uint16_t seq_num;                         // Sequence number of ESP-NOW data.
        int len;                                  // Length of ESPNOW data to be sent, unit: byte.
        uint8_t *buffer;                          // Buffer pointing to ESPNOW data.
        uint8_t dest_mac[ESP_NOW_ETH_ALEN];       // MAC address of destination device.
} espnow_send_param_t;

// ESP-NOW peer connection status
typedef enum
{
        ESP_PEER_STATUS_UNKNOWN,        // Unknown status
        ESP_PEER_STATUS_LOST,           // Not in range of Wi-Fi for an extended period of time
        ESP_PEER_STATUS_PROTOCOL_ERROR, // `NOT IMPLEMENTED`, all mismatched packet is discarded on the upstream
        ESP_PEER_STATUS_NOREPLY,        // Peer does not reply for ping packet
        ESP_PEER_STATUS_IN_RANGE,       // In range of Wi-Fi
        ESP_PEER_STATUS_AVAILABLE,      // Can establish connection
        ESP_PEER_STATUS_CONNECTING,     // Connection request is sent, waiting for reply
        ESP_PEER_STATUS_CONNECTED,      // Connected
        ESP_PEER_STATUS_REJECTED,       // Peer rejected the connection
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

// ESP-NOW peer handle
typedef struct
{
        uint8_t mac[ESP_NOW_ETH_ALEN]; // Peer MAC address
        int64_t lastseen_broadcast_us; // Timestamp of last received broadcast packet
        int64_t lastseen_unicast_us;   // Timestamp of last received unicast packet
        int64_t lastsent_unicast_us;   // Timestamp of last transmitted unicast packet
        int64_t connect_time_us;       // Timestamp of last send connection request packet
        int64_t last_ping_us;          // Timestamp of last ping packet
        size_t conn_retry;             // Number of time of retrying the connection request packet
        size_t seq_rx;                 // Total number of packet received
        size_t seq_tx;                 // Total number of packet transmitted
        esp_peer_status_t status;      // Peer connection status
        int rssi;                      // Last seen RSSI
        bool registered;               // Is registered on the connection table
        bool is_unique;                // Is set to unique peer, will disconnect all other peer
        bool saved_to_rom;             // Peer MAC address is saved to EEPROM
} esp_peer_handle_t;

// ESP-NOW peer connection handle, create/remove connection as requested
typedef struct
{
        device_settings_t *device_settings; // EEPROM handle
        esp_peer_handle_t *entries;         // List of all peers
        int8_t size;                        // Number of peers in the list
        int8_t limit;                       // Max active number of peers
        int8_t remote_connected;            // Number of connected peers
} esp_connection_handle_t;

/* ESP-NOW */

// Initialize Wi-Fi for ESP-NOW
void espnow_wifi_init(espnow_wifi_config_t *espnow_config);
// Creates the task for handling ESP-NOW data packets and returns a queue for ESP-NOW events
QueueHandle_t espnow_init(espnow_wifi_config_t *espnow_config, esp_connection_handle_t *conn_handle);

// Loads default settings of the ESP-NOW Wi-Fi
espnow_wifi_config_t *espnow_wifi_default_config(espnow_wifi_config_t *config);

// Configure parameter for sending packet to default peer
espnow_send_param_t *espnow_get_default_send_param(espnow_send_param_t *send_param);
// Configure parameter for sending packet to broadcast address
espnow_send_param_t *espnow_get_send_param_broadcast(espnow_send_param_t *send_param);
// Configure parameter for sending packet to unicast address
espnow_send_param_t *espnow_get_send_param_unicast(espnow_send_param_t *send_param, const uint8_t *mac);
// Configure parameter for sending packet to peer MAC address
espnow_send_param_t *espnow_get_send_param(espnow_send_param_t *send_param, esp_peer_handle_t *peer);

void espnow_deinit(espnow_send_param_t *send_param);

// Validates the packet structure from the header and extract the payload
espnow_packet_t *espnow_data_parse(espnow_packet_t *recv_data, espnow_event_recv_cb_t *recv_cb);

// Send ESP-NOW data packet to peer
esp_err_t espnow_send_data(espnow_send_param_t *send_param, espnow_packet_type_t type, void *data, size_t len);
// Send ESP-NOW data packet type of `TEXT` to peer
esp_err_t espnow_send_text(espnow_send_param_t *send_param, char *text);
// Send ESP-NOW data packet type of `REPLY` to peer
esp_err_t espnow_send_reply(espnow_send_param_t *send_param);

/* ESP-NOW peer connection */

// Initialize ESP-NOW peer connection handle
void esp_connection_handle_init(esp_connection_handle_t *handle);
// Load paired peer as the unique peer, disconnecting all other peer
void esp_connection_handle_connect_to_device_settings(esp_connection_handle_t *handle, device_settings_t *device_settings);
// `DO NOT USE`
void esp_connection_handle_clear(esp_connection_handle_t *handle);
// Updates all peer status
void esp_connection_handle_update(esp_connection_handle_t *handle);
// Updates rssi
void esp_connection_update_rssi(esp_connection_handle_t *handle, const rssi_event_t *rssi_event);

// Enable broadcast
void esp_connection_enable_broadcast(esp_connection_handle_t *handle);
// Disable broadcast
void esp_connection_disable_broadcast(esp_connection_handle_t *handle);

// Count the total number of connected peer
size_t esp_connection_count_connected(esp_connection_handle_t *handle);
// Count the total number of unique peer
size_t esp_connection_count_unique_peer(esp_connection_handle_t *handle);

// Get peer handle from peer MAC address, return NULL if not found in list
esp_peer_handle_t *esp_connection_mac_lookup(esp_connection_handle_t *handle, const uint8_t *mac);

// Create new peer handle with provided MAC if not in list, returns peer handle
esp_peer_handle_t *esp_connection_mac_add_to_entry(esp_connection_handle_t *handle, const uint8_t *mac);

// Print peer list and status
void esp_connection_show_entries(esp_connection_handle_t *handle);

// Pinging peer to keep connection valid
void esp_connection_send_heartbeat(esp_connection_handle_t *handle);

// Sets the maximum peer that can be active
void esp_connection_set_peer_limit(esp_connection_handle_t *handle, int8_t new_limit);

// Sets the unique peer
void esp_connection_set_unique_peer_mac(esp_connection_handle_t *handle, const uint8_t *mac);

/* ESP-NOW peer */

// Updates peer status
void esp_peer_set_status(esp_peer_handle_t *peer, esp_peer_status_t new_status);

// Process received packet
void esp_peer_process_received(esp_peer_handle_t *peer, espnow_packet_t *recv_data);

// Kill all ono-unique peer
void esp_connection_purge_non_unique_peers(esp_connection_handle_t *handle);
