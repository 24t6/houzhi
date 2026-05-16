/**
 **************************************************************************************************
 * @file        usart6.h
 * @brief       USART6串口驱动 - 头文件(前置通信)
 * @description 定义USART6初始化和数据发送函数声明
 * @note        用于与前置设备通信
 * @version     V1.0
 * @date        2024
 **************************************************************************************************
 */

#ifndef __USART6_H
#define __USART6_H

#include "stm32f4xx.h"

/**
 * @brief 初始化USART6串口
 * @param bound 波特率
 */
void USART6_Init(u32 bound);

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节
 */
void Serial6_SendByte(uint8_t Byte);

/**
 * @brief 发送字符串
 * @param str 字符串指针
 */
void Serial6_SendString(char *str);

/**
 * @brief 发送数组
 * @param array 数组指针
 * @param length 数组长度
 */
void Serial6_SendArray(uint8_t *array, uint16_t length);

/**
 * @brief 发送数据包
 * @param array 数据数组
 * @param size 数据长度
 */
void Serial6_SendPacket(uint8_t *array, uint8_t size);

/**
 * @brief 发送雷达与环境信息
 * @param distance_left 左超声波距离
 * @param distance_right 右超声波距离
 * @param direction 方向码: 0=无来车, 1=后前, 2=后左, 3=后右
 * @param radar_speed 雷达速度
 */
void Serial6_SendRadarInfo(uint16_t distance_left, uint16_t distance_right, uint8_t direction, uint8_t radar_speed);

#endif
