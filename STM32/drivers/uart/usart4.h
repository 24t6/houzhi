/**
 **************************************************************************************************
 * @file        usart4.h
 * @brief       UART4串口驱动 - 头文件(GY95T陀螺仪)
 * @description 定义UART4初始化和函数声明
 * @note        用于与GY95T陀螺仪通信
 * @version     V1.0
 * @date        2024
 **************************************************************************************************
 */

#ifndef __USART4_H
#define __USART4_H

#include "stm32f4xx.h"

extern volatile uint8_t Receive_ok;       ///< 接收完成标志
extern uint8_t usart_rx_data[100];        ///< 接收数据缓冲区

/**
 * @brief 初始化UART4串口
 * @param bound 波特率
 */
void UART4_Init(u32 bound);

/**
 * @brief UART4中断服务函数
 */
void UART4_IRQHandler(void);

/**
 * @brief 发送单个字节
 * @param byte 要发送的字节
 */
void UART4_SendByte(uint8_t byte);

/**
 * @brief 发送数据(带校验)
 * @param Buffer 数据缓冲区
 * @param Length 数据长度
 */
void UART4_Send(uint8_t *Buffer, uint8_t Length);

#endif
