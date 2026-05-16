#include "usart5.h"
#include "stm32f4xx.h"
#include <stdio.h>

#define UART5_RX_BUFFER_SIZE 512
static uint8_t uart5_rx_buffer[UART5_RX_BUFFER_SIZE];
static volatile uint16_t uart5_rx_len = 0;

void UART5_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
    
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(UART5, &USART_InitStructure);
    USART_Cmd(UART5, ENABLE);
    
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void UART5_IRQHandler(void)
{
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        uint8_t c = USART_ReceiveData(UART5);
        if (uart5_rx_len < UART5_RX_BUFFER_SIZE - 1)
            uart5_rx_buffer[uart5_rx_len++] = c;
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
    }
}

void UART5_SendByte(uint8_t data)
{
    USART_SendData(UART5, data);
    while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
}

void UART5_SendString(char* str)
{
    while (*str)
    {
        UART5_SendByte(*str++);
    }
}

void UART5_SendBytes(uint8_t* data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        UART5_SendByte(data[i]);
    }
}

uint8_t UART5_ReadByte(void)
{
    if (uart5_rx_len > 0) {
        uint8_t c = uart5_rx_buffer[0];
        for (uint16_t i = 0; i < uart5_rx_len - 1; i++) {
            uart5_rx_buffer[i] = uart5_rx_buffer[i + 1];
        }
        uart5_rx_len--;
        return c;
    }
    return 0;
}

uint8_t UART5_GetRxFlag(void)
{
    return uart5_rx_len > 0 ? 1 : 0;
}

void UART5_ClearRxFlag(void)
{
    uart5_rx_len = 0;
}

int fputc(int ch, FILE *f)
{
    UART5_SendByte((uint8_t)ch);
    return ch;
}
