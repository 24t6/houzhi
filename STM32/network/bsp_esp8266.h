#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

#include "stm32f4xx.h"
#include <stdint.h>

#define ESP8266_RX_BUF_SIZE 512
extern uint8_t esp8266_rx_buf[ESP8266_RX_BUF_SIZE];
extern volatile uint16_t esp8266_rx_len;

void USART3_Init(uint32_t baudrate);
void USART3_SendString(char *str);
void USART3_SendByte(uint8_t data);
void ESP8266_ClearBuffer(void);
uint8_t ESP8266_WaitFor(const char *keyword, uint16_t timeout);
void ESP8266_SendCmd(const char *cmd);

#endif