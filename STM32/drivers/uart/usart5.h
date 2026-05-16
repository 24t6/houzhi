#ifndef __UART5_H
#define __UART5_H

#include "stm32f4xx.h"
#include <stdint.h>

void UART5_Init(uint32_t baudrate);
void UART5_SendByte(uint8_t data);
void UART5_SendString(char* str);
void UART5_SendBytes(uint8_t* data, uint16_t len);
uint8_t UART5_ReadByte(void);
uint8_t UART5_GetRxFlag(void);
void UART5_ClearRxFlag(void);

#endif


