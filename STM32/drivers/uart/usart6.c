/**
 **************************************************************************************************
 * @file        usart6.c
 * @brief       USART6串口驱动实现(前置通信)
 * @description 实现USART6初始化和数据发送，用于与前置设备通信
 * @note        引脚: PC6(TX), PC7(RX)
 **************************************************************************************************
 */

#include "usart6.h"
#include <stdio.h>

/**
 * @brief 初始化USART6串口
 * @param bound 波特率
 * @note 配置发送和接收模式
 */
void USART6_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* 使能时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    /* 配置TX引脚 PC6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

    /* 配置RX引脚 PC7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    /* 配置USART6 */
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART6, &USART_InitStructure);

    /* 使能USART6 */
    USART_Cmd(USART6, ENABLE);
}

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节
 */
void Serial6_SendByte(uint8_t Byte)
{
    USART_SendData(USART6, Byte);
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
}

/**
 * @brief 发送字符串
 * @param str 字符串指针
 */
void Serial6_SendString(char *str)
{
    while (*str)
    {
        Serial6_SendByte(*str++);
    }
}

/**
 * @brief 发送数组
 * @param array 数组指针
 * @param length 数组长度
 */
void Serial6_SendArray(uint8_t *array, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        Serial6_SendByte(array[i]);
    }
}

/**
 * @brief 发送数据包
 * @param array 数据数组
 * @param size 数据长度
 * @note 自动添加帧头0xFF和帧尾0xFE
 */
void Serial6_SendPacket(uint8_t *array, uint8_t size)
{
    Serial6_SendByte(0xFF);
    Serial6_SendArray(array, size);
    Serial6_SendByte(0xFE);
}

/**
 * @brief 发送雷达与环境信息 (前后车识别版)
 * @param distance_left 左超声波距离(cm)
 * @param distance_right 右超声波距离(cm)
 * @param direction 方向码: 0=无来车, 1=后前, 2=后左, 3=后右
 * @param radar_speed 雷达探测目标速度(km/h)
 * @note 数据格式: [帧头0xFF][超声波左][超声波右][方向码][雷达速度][帧尾0xFE]
 */
void Serial6_SendRadarInfo(uint16_t distance_left, uint16_t distance_right, uint8_t direction, uint8_t radar_speed)
{
    uint8_t data[4];
    
    if (distance_left > 250) distance_left = 250;
    if (distance_right > 250) distance_right = 250;
    
    data[0] = (uint8_t)distance_left;
    data[1] = (uint8_t)distance_right;
    data[2] = direction;
    data[3] = radar_speed;
    
    Serial6_SendPacket(data, 4);
}
