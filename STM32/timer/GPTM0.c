///**
// **************************************************************************************************
// * @file        GPTM0.c
// * @brief       通用定时器驱动实现
// * @description 实现TIM4和TIM5定时器配置，用于超声波测距计时
// * @note        TIM5用于左超声波，TIM4用于右超声波
// * @author      
// * @date        2024
// * @version     V1.0
// **************************************************************************************************
// */

//#include "GPTM0.h"

//volatile uint32_t num1 = 0;   ///< TIM4计数器(右超声波)
//volatile uint32_t num = 0;    ///< TIM5计数器(左超声波)

///**
// * @brief 配置TIM5定时器
// * @note 用于左超声波测距计时，1ms中断一次
// */
//void GPTM0_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

//    TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;
//    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;
//    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);

//    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
//    TIM_Cmd(TIM5, DISABLE);

//    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

///**
// * @brief TIM5中断服务函数
// * @note 定时器溢出中断，每1ms触发一次
// */
//void TIM5_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
//    {
//        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
//        num++;
//    }
//}

///**
// * @brief 配置TIM4定时器
// * @note 用于右超声波测距计时，1ms中断一次
// */
//void GPTM1_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

//    TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;
//    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;
//    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

//    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
//    TIM_Cmd(TIM4, DISABLE);

//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

///**
// * @brief TIM4中断服务函数
// * @note 定时器溢出中断，每1ms触发一次
// */
//void TIM4_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
//    {
//        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//        num1++;
//    }
//}
