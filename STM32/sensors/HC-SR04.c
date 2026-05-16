/**
 * @file    HC-SR04.c
 * @brief   HC-SR04 dual ultrasonic ranging driver
 */

#include "HC-SR04.h"
#include "delay.h"
#include "stm32f4xx_tim.h"

volatile uint32_t Distance = 0;
volatile uint32_t Distance1 = 0;
volatile uint16_t num = 0;
volatile uint16_t num1 = 0;
static volatile uint8_t echo_active = 0;
static volatile uint8_t echo_active1 = 0;
static uint32_t last_distance = 2500U;
static uint32_t last_distance1 = 2500U;
static uint8_t invalid_count = 0;
static uint8_t invalid_count1 = 0;

static uint32_t HC_SR04_FinalizeDistance(uint32_t valid_reads,
                                         uint32_t average_distance,
                                         uint32_t *last_valid_distance,
                                         uint8_t *invalid_reads,
                                         uint8_t invalid_hold_limit)
{
    if (valid_reads == 0U)
    {
        if (*invalid_reads < invalid_hold_limit)
        {
            (*invalid_reads)++;
            return *last_valid_distance;
        }

        *last_valid_distance = 2500U;
        return 2500U;
    }

    *invalid_reads = 0U;
    average_distance /= valid_reads;
    if (average_distance > 2500U)
    {
        average_distance = 2500U;
    }

    *last_valid_distance = average_distance;
    return average_distance;
}

void HC_SR04_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef tim_time_base_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE);

    tim_time_base_structure.TIM_Period = TIM_TIMEOUT_VALUE;
    tim_time_base_structure.TIM_Prescaler = 83;
    tim_time_base_structure.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_time_base_structure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &tim_time_base_structure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    TIM_TimeBaseInit(TIM4, &tim_time_base_structure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    nvic_init_structure.NVIC_IRQChannel = TIM5_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 7;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 1;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

    nvic_init_structure.NVIC_IRQChannel = TIM4_IRQn;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvic_init_structure);
}

void HC_SR04_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure;
    EXTI_InitTypeDef exti_init_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    gpio_init_structure.GPIO_Pin = TRIGGER_PIN1;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(TRIGGER_PORT1, &gpio_init_structure);

    gpio_init_structure.GPIO_Pin = ECHO_PIN1;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(ECHO_PORT1, &gpio_init_structure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);

    exti_init_structure.EXTI_Line = EXTI_Line14;
    exti_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exti_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init_structure);
    EXTI_ClearITPendingBit(EXTI_Line14);

    nvic_init_structure.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 6;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 1;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

    HC_SR04_TIM_Init();
}

void HC_SR04_Init1(void)
{
    GPIO_InitTypeDef gpio_init_structure;
    EXTI_InitTypeDef exti_init_structure;
    NVIC_InitTypeDef nvic_init_structure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    gpio_init_structure.GPIO_Pin = TRIGGER_PIN2;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(TRIGGER_PORT2, &gpio_init_structure);

    gpio_init_structure.GPIO_Pin = ECHO_PIN2;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(ECHO_PORT2, &gpio_init_structure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource12);

    exti_init_structure.EXTI_Line = EXTI_Line12;
    exti_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exti_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init_structure);
    EXTI_ClearITPendingBit(EXTI_Line12);

    nvic_init_structure.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 6;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);
}

uint32_t HC_SR04_Compute(void)
{
    uint32_t valid_reads = 0;
    uint32_t average_distance = 0;
    uint8_t i;

    for (i = 0; i < AVERAGE_READS; i++)
    {
        TIM_Cmd(TIM5, DISABLE);
        TIM_SetCounter(TIM5, 0);
        Distance = 0;
        echo_active = 0;
        GPIO_SetBits(TRIGGER_PORT1, TRIGGER_PIN1);
        delay_us(20);
        GPIO_ResetBits(TRIGGER_PORT1, TRIGGER_PIN1);
        delay_ms(ULTRASONIC_WAIT_MS);

        if (Distance >= 20U && Distance <= 4000U)
        {
            average_distance += Distance;
            valid_reads++;
        }

    }
    return HC_SR04_FinalizeDistance(valid_reads,
                                    average_distance,
                                    &last_distance,
                                    &invalid_count,
                                    INVALID_HOLD_LIMIT);
}

uint32_t HC_SR04_Compute1(void)
{
    uint32_t valid_reads = 0;
    uint32_t average_distance = 0;
    uint8_t i;

    for (i = 0; i < AVERAGE_READS_FAST; i++)
    {
        TIM_Cmd(TIM4, DISABLE);
        TIM_SetCounter(TIM4, 0);
        Distance1 = 0;
        echo_active1 = 0;
        GPIO_SetBits(TRIGGER_PORT2, TRIGGER_PIN2);
        delay_us(20);
        GPIO_ResetBits(TRIGGER_PORT2, TRIGGER_PIN2);
        delay_ms(ULTRASONIC_WAIT_MS_FAST);

        if (Distance1 >= 20U && Distance1 <= 4000U)
        {
            average_distance += Distance1;
            valid_reads++;
        }

    }
    return HC_SR04_FinalizeDistance(valid_reads,
                                    average_distance,
                                    &last_distance1,
                                    &invalid_count1,
                                    INVALID_HOLD_LIMIT_FAST);
}

void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        num++;
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        num1++;
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

void EXTI15_10_IRQHandler(void)
{
    uint32_t time_ticks;

    if (EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        if (GPIO_ReadInputDataBit(ECHO_PORT1, ECHO_PIN1) == Bit_SET)
        {
            TIM_SetCounter(TIM5, 0);
            TIM_Cmd(TIM5, ENABLE);
            echo_active = 1;
        }
        else if (echo_active != 0U)
        {
            TIM_Cmd(TIM5, DISABLE);
            time_ticks = TIM_GetCounter(TIM5);
            if (time_ticks > 60000U)
            {
                time_ticks = 60000U;
            }

            Distance = time_ticks * 34U / 200U;
            if (Distance > 4000U)
            {
                Distance = 4000U;
            }
            if (Distance < 20U)
            {
                Distance = 20U;
            }

            echo_active = 0;
        }

        EXTI_ClearITPendingBit(EXTI_Line14);
    }

    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        if (GPIO_ReadInputDataBit(ECHO_PORT2, ECHO_PIN2) == Bit_SET)
        {
            TIM_SetCounter(TIM4, 0);
            TIM_Cmd(TIM4, ENABLE);
            echo_active1 = 1;
        }
        else if (echo_active1 != 0U)
        {
            TIM_Cmd(TIM4, DISABLE);
            time_ticks = TIM_GetCounter(TIM4);
            if (time_ticks > 60000U)
            {
                time_ticks = 60000U;
            }

            Distance1 = time_ticks * 34U / 200U;
            if (Distance1 > 4000U)
            {
                Distance1 = 4000U;
            }
            if (Distance1 < 20U)
            {
                Distance1 = 20U;
            }

            echo_active1 = 0;
        }

        EXTI_ClearITPendingBit(EXTI_Line12);
    }
}
