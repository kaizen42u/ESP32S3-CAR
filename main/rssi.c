
#include "rssi.h"

static const char *TAG = "rssi";

QueueHandle_t rssi_queue;

static void wifi_promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{

        // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
        if (type != WIFI_PKT_MGMT)
                return;

        static const uint8_t ACTION_SUBTYPE = 0xd0;
        // static const uint8_t ESPRESSIF_OUI[] = {0x18, 0xfe, 0x34};

        const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
        const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
        const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

        // Only continue processing if this is an action frame containing the Espressif OUI.
        if ((ACTION_SUBTYPE == (hdr->frame_ctrl & 0xFF)))
        {
                // print_mem(hdr, sizeof(wifi_ieee80211_mac_hdr_t));
                rssi_event_t event = {
                    .rssi = ppkt->rx_ctrl.rssi,
                    .time_us = esp_timer_get_time()};
                memcpy(event.recv_mac, hdr->addr2, 6);

                if (xQueueSend(rssi_queue, &event, 0) != pdTRUE)
                {
                        ESP_LOGW(TAG, "Send send queue fail");
                }
        }
}

// call after `esp_wifi_init`
QueueHandle_t rssi_init(void)
{
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous_rx_cb));
        rssi_queue = xQueueCreate(RSSI_QUEUE_SIZE, sizeof(rssi_event_t));
        if (rssi_queue == NULL)
        {
                ESP_LOGE(TAG, "Create mutex fail");
                return NULL;
        }
        return rssi_queue;
}

void print_rssi_event(rssi_event_t *event)
{
        ESP_LOGI(TAG, "address " MACSTR " RSSI: %d", MAC2STR(event->recv_mac), event->rssi);
}