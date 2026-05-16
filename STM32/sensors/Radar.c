/**
 **************************************************************************************************
 * @file        Radar.c
 * @brief       Millimeter-wave radar parser and task handler
 **************************************************************************************************
 */

#include "Radar.h"

#include <string.h>

#include "usart2.h"

FrontRadar_Message targets[20];
uint8_t target_count = 0;
uint8_t radar_alarm = 0;

static uint8_t tmp_buffer[256];
static uint8_t parse_state = 0;
static uint16_t parse_idx = 0;
static uint32_t frame_counter = 0;

static void ParseRadarFrame(uint8_t *data, uint16_t length);

static void ParseRadarFrame(uint8_t *data, uint16_t length)
{
    uint8_t target_num;
    uint8_t i;

    if (length < 4)
    {
        target_count = 0;
        radar_alarm = 0;
        return;
    }

    target_num = data[2];
    radar_alarm = data[3];
    if (target_num > 20)
    {
        target_count = 0;
        radar_alarm = 0;
        return;
    }

    target_count = target_num;
    memset(targets, 0, sizeof(targets));

    for (i = 0; i < target_num; i++)
    {
        uint16_t offset = 4u + (uint16_t)i * 5u;
        if ((offset + 5u) <= length)
        {
            uint8_t *p = &data[offset];
            int8_t angle = (int8_t)(p[0] - 0x80);
            uint8_t distance_m = p[1];
            uint8_t speed_dir = p[2];
            uint8_t speed_val = p[3];
            int16_t speed_signed = (speed_dir == 0x01) ? speed_val : -(int16_t)speed_val;

            targets[i].angle = angle;
            targets[i].distance = distance_m * 10;
            targets[i].speed = speed_signed;
            targets[i].snr = p[4];
        }
    }

    frame_counter++;
}

void Radar_10msTask(void)
{
    uint8_t rx_buf[128];
    uint16_t bytes_read;

    do
    {
        uint16_t i;

        bytes_read = Radar_GetBufferData(rx_buf, sizeof(rx_buf));
        if (bytes_read == 0U)
        {
            break;
        }

        for (i = 0; i < bytes_read; i++)
        {
            uint8_t b = rx_buf[i];

            switch (parse_state)
            {
            case 0:
                if (b == 0xF4)
                {
                    parse_state = 1;
                }
                break;

            case 1:
                parse_state = (b == 0xF3) ? 2 : 0;
                break;

            case 2:
                parse_state = (b == 0xF2) ? 3 : 0;
                break;

            case 3:
                if (b == 0xF1)
                {
                    parse_idx = 0;
                    parse_state = 4;
                }
                else
                {
                    parse_state = 0;
                }
                break;

            case 4:
                tmp_buffer[parse_idx++] = b;

                if (parse_idx >= sizeof(tmp_buffer))
                {
                    parse_state = 0;
                    parse_idx = 0;
                    break;
                }

                if (parse_idx >= 2)
                {
                    uint16_t data_len = (uint16_t)tmp_buffer[0] | ((uint16_t)tmp_buffer[1] << 8);
                    uint16_t frame_size = (uint16_t)(data_len + 6u);

                    if (frame_size > sizeof(tmp_buffer))
                    {
                        parse_state = 0;
                        parse_idx = 0;
                        break;
                    }

                    if (parse_idx >= frame_size)
                    {
                        if (tmp_buffer[data_len + 2] == 0xF8 &&
                            tmp_buffer[data_len + 3] == 0xF7 &&
                            tmp_buffer[data_len + 4] == 0xF6 &&
                            tmp_buffer[data_len + 5] == 0xF5)
                        {
                            ParseRadarFrame(tmp_buffer, (uint16_t)(data_len + 2u));
                        }

                        parse_state = 0;
                        parse_idx = 0;
                    }
                }
                break;

            default:
                parse_state = 0;
                parse_idx = 0;
                break;
            }
        }
    } while (bytes_read > 0U);
}
