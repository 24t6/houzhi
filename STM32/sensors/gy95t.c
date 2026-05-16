/**
 * @file    gy95t.c
 * @brief   GY95T UART driver implementation
 */

#include "gy95t.h"

#include <string.h>

#include "delay.h"
#include "usart4.h"

#define GY95T_ADDR                 0xA4U
#define GY95T_FUNC_READ            0x03U
#define GY95T_FUNC_WRITE           0x06U
#define GY95T_START_REG_DEFAULT    0x08U
#define GY95T_REG_COUNT_DEFAULT    0x1BU
#define GY95T_FRAME_DATA_BYTES     27U
#define GY95T_FRAME_TOTAL_BYTES    32U

static GY95T_Data sensor_data;
static volatile uint8_t data_ready = 0;
static int16_t last_reliable_roll = 0;
static int16_t last_reliable_pitch = 0;
static int16_t last_reliable_yaw = 0;

static void GY95T_SendCommand(uint8_t function_code, uint8_t reg_addr, uint8_t reg_value)
{
    uint8_t send_data[5] = {GY95T_ADDR, function_code, reg_addr, reg_value, 0};
    uint8_t i;

    send_data[4] = (uint8_t)(send_data[0] + send_data[1] + send_data[2] + send_data[3]);

    for (i = 0; i < sizeof(send_data); i++)
    {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
        {
        }
        USART_SendData(UART4, send_data[i]);
    }
}

void GY95T_Init(void)
{
    delay_ms(300);

    GY95T_SendCommand(GY95T_FUNC_WRITE, 0x02U, 0x02U);
    delay_ms(50);

    GY95T_SendCommand(GY95T_FUNC_READ, GY95T_START_REG_DEFAULT, GY95T_REG_COUNT_DEFAULT);
    delay_ms(50);

    GY95T_SendCommand(GY95T_FUNC_WRITE, 0x03U, 0x00U);
    delay_ms(50);
}

void GY95T_ProcessData(void)
{
    uint8_t sum = 0;
    uint8_t i;

    if (Receive_ok == 0U)
    {
        return;
    }

    if ((usart_rx_data[0] != GY95T_ADDR) ||
        (usart_rx_data[1] != GY95T_FUNC_READ) ||
        (usart_rx_data[2] != GY95T_START_REG_DEFAULT) ||
        (usart_rx_data[3] != GY95T_REG_COUNT_DEFAULT))
    {
        Receive_ok = 0;
        return;
    }

    for (i = 0; i < (uint8_t)(usart_rx_data[3] + 4U); i++)
    {
        sum = (uint8_t)(sum + usart_rx_data[i]);
    }

    if (sum == usart_rx_data[usart_rx_data[3] + 4U])
    {
        memcpy(&sensor_data, &usart_rx_data[4], sizeof(sensor_data));
        data_ready = 1;
    }

    Receive_ok = 0;
}

uint8_t GY95T_DataReady(void)
{
    return data_ready;
}

GY95T_Data GY95T_GetSensorData(void)
{
    return sensor_data;
}

uint8_t GY95T_ReadLatest(GY95T_Data *out_data)
{
    if (out_data == 0)
    {
        return 0;
    }

    GY95T_ProcessData();
    if (data_ready == 0U)
    {
        return 0;
    }

    *out_data = sensor_data;
    last_reliable_roll = out_data->roll;
    last_reliable_pitch = out_data->pitch;
    last_reliable_yaw = out_data->yaw;
    data_ready = 0;

    return 1;
}

void GY95T_RequestData(void)
{
}

int16_t GY95T_GetRoll(void)
{
    GY95T_Data latest_data;

    if (GY95T_ReadLatest(&latest_data) != 0U)
    {
        return latest_data.roll;
    }

    return last_reliable_roll;
}

int16_t GY95T_GetPitch(void)
{
    return last_reliable_pitch;
}

int16_t GY95T_GetYaw(void)
{
    return last_reliable_yaw;
}
