#include "bsp_esp8266.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>

uint8_t esp8266_rx_buf[ESP8266_RX_BUF_SIZE];
volatile uint16_t esp8266_rx_len = 0;

void USART3_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART3_SendByte(uint8_t data)
{
    USART_SendData(USART3, data);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void USART3_SendString(char *str)
{
    while (*str)
    {
        USART3_SendByte(*str++);
    }
}

void ESP8266_ClearBuffer(void)
{
    memset(esp8266_rx_buf, 0, sizeof(esp8266_rx_buf));
    esp8266_rx_len = 0;
}

uint8_t ESP8266_WaitFor(const char *keyword, uint16_t timeout)
{
    uint16_t t = 0;
    while (t < timeout)
    {
        if (strstr((char*)esp8266_rx_buf, keyword)) return 1;
        delay_ms(10);
        t += 10;
    }
    return 0;
}

void ESP8266_SendCmd(const char *cmd)
{
    USART3_SendString((char*)cmd);
    USART3_SendString((char*)"\r\n");
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        uint8_t c = USART_ReceiveData(USART3);
        if (esp8266_rx_len < ESP8266_RX_BUF_SIZE - 1)
            esp8266_rx_buf[esp8266_rx_len++] = c;
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}