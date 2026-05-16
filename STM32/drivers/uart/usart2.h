#ifndef __USART2_H
#define __USART2_H

#include "stm32f4xx.h"

void USART2_Init(u32 bound);
uint16_t Radar_GetAvailableBytes(void);
uint16_t Radar_GetBufferData(uint8_t *buffer, uint16_t max_len);

#endif
