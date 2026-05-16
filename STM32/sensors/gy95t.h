/**
 * @file    gy95t.h
 * @brief   GY95T UART driver
 */

#ifndef __GY95T_H
#define __GY95T_H

#include "stm32f4xx.h"

#pragma pack(push, 1)
typedef struct
{
    int16_t acc[3];
    int16_t gyro[3];
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    uint8_t level;
    int16_t temp;
    int16_t mag[3];
} GY95T_Data;
#pragma pack(pop)

void GY95T_Init(void);
void GY95T_ProcessData(void);
uint8_t GY95T_DataReady(void);
GY95T_Data GY95T_GetSensorData(void);
uint8_t GY95T_ReadLatest(GY95T_Data *out_data);
void GY95T_RequestData(void);
int16_t GY95T_GetRoll(void);
int16_t GY95T_GetPitch(void);
int16_t GY95T_GetYaw(void);

#endif
