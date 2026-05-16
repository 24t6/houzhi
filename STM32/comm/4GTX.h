/**
 * @file    4GTX.h
 * @brief   4G 模块驱动头文件
 * @details 定义 4G 模块 AT 指令相关函数声明
 * @note    4G 模块功能:
 *          - 短信发送
 *          - 电话拨打
 *          - 紧急求救
 * @version 1.0
 * @date    2024
 */

#ifndef __4GTX_H
#define __4GTX_H

#include "stm32f4xx.h"
#include <stdio.h>

/**
 * @brief 初始化 4G 模块
 * @details 发送 AT 指令进行配置，包括波特率、短信模式等
 * @retval None
 */
void TX_4G_Init(void);

/**
 * @brief 拨打电话
 * @details 拨打预设的电话号码 (17688605321)
 * @retval None
 */
void TX_Call_phone(void);

/**
 * @brief 发送短信
 * @details 发送预设的求助短信内容
 * @retval None
 * @note 短信内容为 Unicode 编码的求救信息
 */
void TX_SendMssage(void);

/**
 * @brief 触发求救
 * @details 依次执行: 初始化4G -> 发送短信 -> 拨打 电话
 * @retval None
 * @note 这是摔倒检测后的最终响应动作
 */
void qiujiu(void);

#endif
