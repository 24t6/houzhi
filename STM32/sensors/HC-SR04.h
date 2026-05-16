/**
 * @file    HC-SR04.h
 * @brief   HC-SR04 dual ultrasonic ranging driver
 */

#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "stm32f4xx.h"

#define SPEED_OF_SOUND            340U
#define AVERAGE_READS             2U
#define AVERAGE_READS_FAST        1U
#define ULTRASONIC_WAIT_MS        60U
#define ULTRASONIC_WAIT_MS_FAST   35U
#define INVALID_HOLD_LIMIT        2U
#define INVALID_HOLD_LIMIT_FAST   1U
#define TIM_TIMEOUT_VALUE         65535U

#define TRIGGER_PORT1             GPIOC
#define TRIGGER_PIN1              GPIO_Pin_9
#define ECHO_PORT1                GPIOC
#define ECHO_PIN1                 GPIO_Pin_14
#define ECHO_EXTI_LINE1           EXTI_Line14

#define TRIGGER_PORT2             GPIOC
#define TRIGGER_PIN2              GPIO_Pin_15
#define ECHO_PORT2                GPIOE
#define ECHO_PIN2                 GPIO_Pin_12
#define ECHO_EXTI_LINE2           EXTI_Line12

void HC_SR04_Init(void);
uint32_t HC_SR04_Compute(void);

void HC_SR04_Init1(void);
uint32_t HC_SR04_Compute1(void);

void HC_SR04_TIM_Init(void);

extern volatile uint32_t Distance;
extern volatile uint32_t Distance1;
extern volatile uint16_t num;
extern volatile uint16_t num1;

#endif
