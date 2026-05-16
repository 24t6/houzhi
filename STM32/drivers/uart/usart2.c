/**
 **************************************************************************************************
 * @file        usart2.h
 * @brief       USART2串口驱动 - 头文件(毫米波雷达)
 * @description 定义USART2初始化和中断处理函数声明
 * @note        用于接收毫米波雷达数据
								引脚：PA2（TX）,PA3(RX)
 * @version     V1.0
 * @date        2024
 **************************************************************************************************
 */

#include "usart2.h"
#include <string.h>

/* 环形缓冲区 */
#define RADAR_RX_BUF_SIZE 1024
static volatile uint8_t Radar_rx_buffer[RADAR_RX_BUF_SIZE];
static volatile uint16_t Radar_rx_index = 0;
static volatile uint16_t Radar_rx_start_index = 0;

/* 获取可用字节数 */
uint16_t Radar_GetAvailableBytes(void)
{
    if (Radar_rx_start_index <= Radar_rx_index) {
        return Radar_rx_index - Radar_rx_start_index;
    } else {
        return (RADAR_RX_BUF_SIZE - Radar_rx_start_index) + Radar_rx_index;
    }
}

/* 从缓冲区读取数据 */
uint16_t Radar_GetBufferData(uint8_t *buffer, uint16_t max_len)
{
    uint16_t available = Radar_GetAvailableBytes();
    if (available == 0) return 0;
    
    if (available > max_len) available = max_len;
    
    uint16_t bytes_read = 0;
    while (bytes_read < available) {
        uint16_t idx = (Radar_rx_start_index + bytes_read) % RADAR_RX_BUF_SIZE;
        buffer[bytes_read] = Radar_rx_buffer[idx];
        bytes_read++;
    }
    
    Radar_rx_start_index = (Radar_rx_start_index + bytes_read) % RADAR_RX_BUF_SIZE;
    return bytes_read;
}

/* 初始化USART2 */
void USART2_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    /* 配置PA2为USART2_TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    
    /* 配置PA3为USART2_RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
    /* 配置USART2参数 */
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
    
    /* 使能接收中断 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    /* 配置中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* USART2中断处理 */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART2);
        
        uint16_t next_index = (Radar_rx_index + 1) % RADAR_RX_BUF_SIZE;
        if (next_index != Radar_rx_start_index) {
            Radar_rx_buffer[Radar_rx_index] = data;
            Radar_rx_index = next_index;
        }
    }
}
