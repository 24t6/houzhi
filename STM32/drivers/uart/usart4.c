/**
 * @file    usart4.c
 * @brief   UART4 driver for GY95T
 */

#include "usart4.h"

#include <string.h>

#define GY95T_ADDR              0xA4U
#define GY95T_FUNC_READ         0x03U
#define GY95T_MAX_FRAME_LEN     40U
#define GY95T_HEADER_BYTES      4U

volatile uint8_t Receive_ok = 0;
uint8_t usart_rx_data[100];

void UART4_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4, &USART_InitStructure);

    USART_Cmd(UART4, ENABLE);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void UART4_IRQHandler(void)
{
    static uint8_t frame_index = 0;
    static uint8_t frame_length = 0;
    static uint8_t frame_buffer[GY95T_MAX_FRAME_LEN] = {0};
    uint8_t rx_byte;

    if (USART_GetITStatus(UART4, USART_IT_RXNE) == RESET)
    {
        return;
    }

    rx_byte = (uint8_t)USART_ReceiveData(UART4);

    if (frame_index == 0U)
    {
        if (rx_byte != GY95T_ADDR)
        {
            return;
        }
    }
    else if (frame_index == 1U)
    {
        if (rx_byte != GY95T_FUNC_READ)
        {
            frame_index = 0U;
            frame_length = 0U;
            return;
        }
    }

    if (frame_index >= GY95T_MAX_FRAME_LEN)
    {
        frame_index = 0U;
        frame_length = 0U;
        return;
    }

    frame_buffer[frame_index++] = rx_byte;

    if (frame_index == GY95T_HEADER_BYTES)
    {
        frame_length = (uint8_t)(frame_buffer[3] + 5U);
        if ((frame_length < GY95T_HEADER_BYTES + 1U) || (frame_length > GY95T_MAX_FRAME_LEN))
        {
            frame_index = 0U;
            frame_length = 0U;
            return;
        }
    }

    if ((frame_length != 0U) && (frame_index >= frame_length))
    {
        if (Receive_ok == 0U)
        {
            memcpy(usart_rx_data, frame_buffer, frame_length);
            Receive_ok = 1;
        }

        frame_index = 0U;
        frame_length = 0U;
    }
}

void UART4_SendByte(uint8_t byte)
{
    USART_SendData(UART4, byte);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
    {
    }
}

void UART4_Send(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    while (i < Length)
    {
        if (i < (Length - 1U))
        {
            Buffer[Length - 1U] = (uint8_t)(Buffer[Length - 1U] + Buffer[i]);
        }
        UART4_SendByte(Buffer[i++]);
    }
}
