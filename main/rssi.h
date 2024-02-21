
#pragma once

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_now.h"

#include "logging.h"

#define RSSI_QUEUE_SIZE (64)

typedef struct
{
        uint16_t frame_ctrl : 16;
        uint16_t duration_id : 16;
        uint8_t addr1[6]; /* receiver address */
        uint8_t addr2[6]; /* sender address */
        uint8_t addr3[6]; /* filtering address */
        uint16_t sequence_ctrl : 16;
        uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct
{
        wifi_ieee80211_mac_hdr_t hdr;
        uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

typedef struct
{
        uint8_t recv_mac[6];
        int rssi;
        int64_t time_us;
} __packed rssi_event_t;

QueueHandle_t rssi_init(void);
void print_rssi_event(rssi_event_t *event);