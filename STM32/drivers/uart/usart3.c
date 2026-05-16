/**
 **************************************************************************************************
 * @file        usart3.c
 * @brief       USART3 driver for the ESP8266 transport layer
 **************************************************************************************************
 */

#include "usart3.h"

#include "delay.h"
#include <string.h>

#define USART3_RX_BUF_SIZE 512

static uint8_t usart3_rx_buf[USART3_RX_BUF_SIZE];
static uint16_t usart3_rx_cnt = 0;

void USART3_Init(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio_init_structure;
    USART_InitTypeDef usart_init_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    gpio_init_structure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init_structure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    usart_init_structure.USART_BaudRate = baud_rate;
    usart_init_structure.USART_WordLength = USART_WordLength_8b;
    usart_init_structure.USART_StopBits = USART_StopBits_1;
    usart_init_structure.USART_Parity = USART_Parity_No;
    usart_init_structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &usart_init_structure);

    USART_Cmd(USART3, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    nvic_init_structure.NVIC_IRQChannel = USART3_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);
}

void USART3_SendByte(uint8_t byte)
{
    USART_SendData(USART3, byte);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
    {
    }
}

void USART3_SendArray(uint8_t *array, uint16_t length)
{
    uint16_t i;

    for (i = 0; i < length; i++)
    {
        USART3_SendByte(array[i]);
    }
}

void USART3_SendString(char *string)
{
    while (*string)
    {
        USART3_SendByte((uint8_t)*string++);
    }
}

void USART3_ClearBuffer(void)
{
    memset(usart3_rx_buf, 0, sizeof(usart3_rx_buf));
    usart3_rx_cnt = 0;
}

uint16_t USART3_GetData(uint8_t *buffer, uint16_t max_len)
{
    uint16_t i;
    uint16_t len = usart3_rx_cnt > max_len ? max_len : usart3_rx_cnt;

    for (i = 0; i < len; i++)
    {
        buffer[i] = usart3_rx_buf[i];
    }

    usart3_rx_cnt = 0;
    usart3_rx_buf[0] = '\0';
    return len;
}

uint8_t USART3_WaitFor(const char *keyword, uint16_t timeout)
{
    uint16_t elapsed = 0;

    while (elapsed < timeout)
    {
        if (strstr((char *)usart3_rx_buf, keyword) != 0)
        {
            return 1;
        }

        delay_ms(10);
        elapsed += 10;
    }

    return 0;
}

void USART3_SendCmd(const char *cmd)
{
    USART3_SendString((char *)cmd);
    USART3_SendString("\r\n");
}

void USART3_IRQHandler(void)
{
    uint8_t ch;

    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        ch = (uint8_t)USART_ReceiveData(USART3);

        if (usart3_rx_cnt < (USART3_RX_BUF_SIZE - 1))
        {
            usart3_rx_buf[usart3_rx_cnt++] = ch;
            usart3_rx_buf[usart3_rx_cnt] = '\0';
        }

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
