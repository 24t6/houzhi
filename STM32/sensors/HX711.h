/**
 * @file    HX711.h
 * @brief   HX711 称重传感器驱动头文件
 * @details 定义三通道 HX711 称重传感器的函数声明和引脚定义
 * @note    引脚分配:
 *          - 通道1: SCK-PB1, DOUT-PB0
 *          - 通道2: SCK-PB8, DOUT-PB7
 *          - 通道3: SCK-PD2, DOUT-PD1
 * @version 1.0
 * @date    2024
 */

#ifndef __HX711_H
#define __HX711_H

#include "stm32f4xx.h"

/**
 * @brief 通道1引脚定义
 */
#define HX711_SCK_PIN    GPIO_Pin_1   ///< 通道1 SCK 引脚
#define HX711_SCK_PORT   GPIOB         ///< 通道1 SCK 端口
#define HX711_DOUT_PIN   GPIO_Pin_0   ///< 通道1 DOUT 引脚
#define HX711_DOUT_PORT  GPIOB         ///< 通道1 DOUT 端口

/**
 * @brief 通道2引脚定义
 */
#define HX711_SCK_PIN1   GPIO_Pin_8   ///< 通道2 SCK 引脚
#define HX711_SCK_PORT1  GPIOB         ///< 通道2 SCK 端口
#define HX711_DOUT_PIN1  GPIO_Pin_7   ///< 通道2 DOUT 引脚
#define HX711_DOUT_PORT1 GPIOB         ///< 通道2 DOUT 端口

/**
 * @brief 通道3引脚定义
 */
#define HX711_SCK_PIN2   GPIO_Pin_2   ///< 通道3 SCK 引脚
#define HX711_SCK_PORT2  GPIOD         ///< 通道3 SCK 端口
#define HX711_DOUT_PIN2  GPIO_Pin_1   ///< 通道3 DOUT 引脚
#define HX711_DOUT_PORT2 GPIOD         ///< 通道3 DOUT 端口

/**
 * @brief 初始化 HX711 引脚
 * @details 配置三路 SCK 为输出模式，DOUT 为输入上拉模式
 * @retval None
 */
void Init_HX711pin(void);

/**
 * @brief 读取通道1 HX711 数据
 * @return 24位 AD 原始值
 */
u32 HX711_Read(void);

/**
 * @brief 读取通道2 HX711 数据
 * @return 24位 AD 原始值
 */
u32 HX711_Read1(void);

/**
 * @brief 读取通道3 HX711 数据
 * @return 24位 AD 原始值
 */
u32 HX711_Read2(void);

/**
 * @brief 获取毛皮重量(去皮)
 * @details 读取三路传感器数据作为基准值，用于后续重量计算
 * @retval None
 */
void Get_Maopi(void);

/**
 * @brief 获取实际重量
 * @details 用当前值减去毛皮值，再除以换算系数得到实际重量
 * @retval None
 */
void Get_Weight(void);

/**
 * @brief 通道1原始数据缓冲区
 */
extern u32 HX711_Buffer;

/**
 * @brief 通道1毛皮重量
 */
extern u32 Weight_Maopi;

/**
 * @brief 通道1实际重量(克)
 */
extern s32 Weight_Shiwu;

/**
 * @brief 通道2原始数据缓冲区
 */
extern u32 HX711_Buffer1;

/**
 * @brief 通道2毛皮重量
 */
extern u32 Weight_Maopi1;

/**
 * @brief 通道2实际重量(克)
 */
extern s32 Weight_Shiwu1;

/**
 * @brief 通道3原始数据缓冲区
 */
extern u32 HX711_Buffer2;

/**
 * @brief 通道3毛皮重量
 */
extern u32 Weight_Maopi2;

/**
 * @brief 通道3实际重量(克)
 */
extern s32 Weight_Shiwu2;

#endif
