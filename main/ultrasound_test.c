
#include "ultrasound_test.h"

static const char *TAG = "ultrasound_test";

float constrain(float value, float min, float max)
{
        if (value < min)
                return min;
        else if (value > max)
                return max;
        else
                return value;
}

float ultrasound_duration_to_distance(float duration_us)
{
        return 0.034 * duration_us / 2;
}

void display_ultrasound_draw(u8g2_t *u8g2, tof_sensor_event_t tof_sensor_event)
{
        float distance_cm = ultrasound_duration_to_distance(tof_sensor_event.duration_us);

        const u8g2_uint_t min_width = 4, max_width = 122;
        const float min_dist = 0.0, max_dist = 300.00;
        const float slope = (max_width - min_width) / (max_dist - min_dist);

        u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
        u8g2_DrawRFrame(u8g2, 0, 10, 128, 20, 3);

        if ((tof_sensor_event.state != TOF_OK) || (distance_cm < 0))
        {
                u8g2_DrawStr(u8g2, 10, 50, "Distance:  NAN  cm");
                u8g2_DrawStr(u8g2, 10, 58, TOF_SENSOR_STATE_STRING[tof_sensor_event.state]);
                return;
        }

        char print_buffer[20];
        snprintf(print_buffer, 20, "Distance: %6.2fcm", distance_cm);
        u8g2_DrawStr(u8g2, 10, 50, print_buffer);

        static float prev_time = 0;
        float fps = 1000000 / (esp_timer_get_time() - prev_time);
        prev_time = esp_timer_get_time();
        snprintf(print_buffer, 20, "     fps: %6.0f", fps);
        u8g2_DrawStr(u8g2, 10, 58, print_buffer);

        if (distance_cm > 0)
        {
                float width = distance_cm * slope + min_width;
                width = constrain(width, min_width, max_width);
                u8g2_DrawRBox(u8g2, 3, 13, width, 14, 2);
        }
}

void drawLogo(u8g2_t *u8g2)
{
        u8g2_SetFontMode(u8g2, 1); // Transparent
        u8g2_SetDrawColor(u8g2, 1);

        u8g2_SetFontDirection(u8g2, 0);
        u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
        u8g2_DrawStr(u8g2, 0, 30, "U");

        u8g2_SetFontDirection(u8g2, 1);
        u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
        u8g2_DrawStr(u8g2, 21, 8, "8");

        u8g2_SetFontDirection(u8g2, 0);
        u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
        u8g2_DrawStr(u8g2, 51, 30, "g");
        u8g2_DrawStr(u8g2, 67, 30, "\xb2");

        u8g2_DrawHLine(u8g2, 2, 35, 47);
        u8g2_DrawHLine(u8g2, 3, 36, 47);
        u8g2_DrawVLine(u8g2, 45, 32, 12);
        u8g2_DrawVLine(u8g2, 46, 33, 12);
}

void drawURL(u8g2_t *u8g2)
{
        u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
        u8g2_DrawStr(u8g2, 1, 54, "github.com/olikraus/u8g2");
}

u8g2_t display_init(void)
{
        // initialize the u8g2 hal
        u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
        u8g2_esp32_hal.bus.spi.clk = GPIO_SSD1306_SPI_CLOCK;
        u8g2_esp32_hal.bus.spi.mosi = GPIO_SSD1306_SPI_DATA;
        u8g2_esp32_hal.bus.spi.cs = GPIO_SSD1306_SPI_CS;
        u8g2_esp32_hal.dc = GPIO_SSD1306_SPI_DC;
        u8g2_esp32_hal.reset = GPIO_SSD1306_SPI_RESET;
        u8g2_esp32_hal_init(u8g2_esp32_hal);

        // initialize the u8g2 library
        u8g2_t u8g2;
        u8g2_Setup_ssd1306_128x64_noname_f(
            &u8g2,
            U8G2_R0,
            u8g2_esp32_spi_byte_cb,
            u8g2_esp32_gpio_and_delay_cb);

        u8g2_InitDisplay(&u8g2);
        u8g2_SetPowerSave(&u8g2, 0);

        u8g2_ClearBuffer(&u8g2);
        drawLogo(&u8g2);
        drawURL(&u8g2);
        u8g2_SendBuffer(&u8g2);
        vTaskDelay(pdMS_TO_TICKS(500));

        return u8g2;
}

void display_ultrasound_loop(void)
{
        u8g2_t u8g2 = display_init();
        tof_sensor_event_t tof_sensor_event;
        QueueHandle_t tof_sensor_event_queue = tof_sensor_init(GPIO_ULTRASOUND_TRIG, GPIO_ULTRASOUND_ECHO);
        while (true)
        {
                bool draw_screen = false;
                if (xQueueReceive(tof_sensor_event_queue, &tof_sensor_event, 1))
                {
                        draw_screen = true;
                        if (tof_sensor_event.state != TOF_OK)
                        {
                                ESP_LOGW(TAG, "%s", TOF_SENSOR_STATE_STRING[tof_sensor_event.state]);
                                // print_mem(&tof_sensor_event, sizeof(tof_sensor_event_t));
                        }

                        ESP_LOGI(TAG, "TOF event: echo %d, trig %d, duration_us %8llu, dist: %6.2fcm",
                                 tof_sensor_event.echo_pin, tof_sensor_event.trig_pin, tof_sensor_event.duration_us,
                                 ultrasound_duration_to_distance(tof_sensor_event.duration_us));
                }

                if (draw_screen)
                {
                        u8g2_ClearBuffer(&u8g2);
                        display_ultrasound_draw(&u8g2, tof_sensor_event);
                        u8g2_SendBuffer(&u8g2);
                }
        }
}
