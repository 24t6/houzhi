/**
 * @file    usart1.c
 * @brief   USART1驱动 - 4G通信模块
 * @details 实现USART1初始化和数据发送,用于4G模块AT指令通信
 * @note    引脚: PA9(TX) - 4G模块RX, PA10(RX) - 4G模块TX
 */

#include "usart1.h"
#include <stdio.h>

/**
 * @brief USART1初始化
 * @param baudrate 波特率(默认115200)
 * @note 使用PA9(复用功能TX)、PA10(复用功能RX)
 */
void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    /* 开启GPIOA和USART1时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* 配置PA9、PA10为复用功能 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    /* GPIO初始化 */
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1配置: 8N1, 115200 */
    USART_InitStruct.USART_BaudRate            = baudrate;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);

    /* 使能USART1 */
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节
 */
void Serial1_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * @brief 发送字符串
 * @param str 字符串指针
 */
void Serial1_SendString(char *str)
{
    while (*str)
    {
        Serial1_SendByte(*str++);
    }
}

/**
 * @brief 发送数组
 * @param array 数组指针
 * @param length 数组长度
 */
void Serial1_SendArray(uint8_t *array, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        Serial1_SendByte(array[i]);
    }
}

/**
 * @brief printf重定向 - 将printf输出重定向到USART1
 * @param ch 要输出的字符
 * @param f 文件指针(未使用)
 * @return 输出的字符
 */
//int fputc(int ch, FILE *f)
//{
//    USART_SendData(USART1, (uint8_t)ch);
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    return ch;
//}
