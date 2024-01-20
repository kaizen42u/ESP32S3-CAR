
#include "mem_probe.h"

void print_mem(const void *ptr, size_t len)
{
        const uint8_t step = 16;
        static uint8_t buffer[16];
        uint8_t pos = 0;

        // printf("--- START MEM DUMP [%d]\r\n", len);
        while (pos < len)
        {
                memcpy(buffer, ptr + pos, step);
                printf("[%p]: ", ptr + pos);
                for (uint8_t i = 0; i < step; i++)
                {
                        if (pos + i >= len)
                                printf("   ");
                        else
                                printf("%02X ", buffer[i]);
                }
                printf("| ");
                for (uint8_t i = 0; i < step; i++)
                        if (pos + i >= len)
                                printf(" ");
                        else
                                printf("%c", isprint(buffer[i]) ? buffer[i] : '.');
                printf("\n");
                pos += step;
        }
        // printf("---   END MEM DUMP\r\n");
}