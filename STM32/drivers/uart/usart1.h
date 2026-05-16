/**
 **************************************************************************************************
 * @file        usart1.h
 * @brief       USART1串口驱动 - 头文件(4G模块通信)
 * @description 定义USART1初始化和数据发送函数声明
 * @note        引脚: PA9(TX), PA10(RX)
 * @version     V1.0
 * @date        2024
 **************************************************************************************************
 */

#ifndef __USART1_H
#define __USART1_H

#include "stm32f4xx.h"

/**
 * @brief 初始化USART1串口
 * @param baudrate 波特率
 */
void USART1_Init(uint32_t baudrate);

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节
 */
void Serial1_SendByte(uint8_t Byte);

/**
 * @brief 发送字符串
 * @param str 字符串指针
 */
void Serial1_SendString(char *str);

/**
 * @brief 发送数组
 * @param array 数组指针
 * @param length 数组长度
 */
void Serial1_SendArray(uint8_t *array, uint16_t length);

#endif
