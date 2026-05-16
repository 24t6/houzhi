/**
 **************************************************************************************************
 * @file        usart3.h
 * @brief       USART3 driver for the ESP8266 transport layer
 **************************************************************************************************
 */

#ifndef __USART3_H
#define __USART3_H

#include "stm32f4xx.h"

void USART3_Init(uint32_t baud_rate);
void USART3_SendByte(uint8_t byte);
void USART3_SendArray(uint8_t *array, uint16_t length);
void USART3_SendString(char *string);
void USART3_SendCmd(const char *cmd);
void USART3_ClearBuffer(void);
uint16_t USART3_GetData(uint8_t *buffer, uint16_t max_len);
uint8_t USART3_WaitFor(const char *keyword, uint16_t timeout);

#endif /* __USART3_H */
