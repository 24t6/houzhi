/**
 **************************************************************************************************
 * @file        4GTX.c
 * @brief       4G通信模块驱动实现
 * @description 实现4G模块初始化、短信发送、电话拨打等AT指令操作
 * @author      
 * @date        2024
 * @version     V1.0
 **************************************************************************************************
 */

#include "4GTX.h"
#include "usart1.h"
#include "delay.h"
#include <stdio.h>

//设定紧急求救人
#define TARGET_PHONE_NUM  "15118882091"

/**
 * @brief 初始化4G模块
 * @details 发送一系列AT指令配置4G模块，包括波特率、短信模式等
 * @note 初始化完成后打印"4G INIT OK!"
 */
void TX_4G_Init(void)
{
    Serial1_SendString("AT\r\n");
    delay_ms(500); 
    Serial1_SendString("ATE0\r\n");          
    delay_ms(500);
    Serial1_SendString("AT+ICCID\r\n");      
    delay_ms(1000);                          
    Serial1_SendString("AT+CGATT?\r\n");     
    delay_ms(1000);                          
    Serial1_SendString("AT+SETVOLTE=1\r\n"); 
    delay_ms(500);
    Serial1_SendString("AT+CLIP=1\r\n");     
    delay_ms(500);

    Serial1_SendString("AT+CMGF=1\r\n");           //  文本模式
    delay_ms(500);
    Serial1_SendString("AT+CSCS=\"UCS2\"\r\n");    
    delay_ms(500);
    Serial1_SendString("AT+CSMP=17,167,0,8\r\n");  // 短信参数设为中文格式(8)
    delay_ms(500);
}

/**
 * @brief 拨打电话
 * @details 拨打预设电话号码 17688605321
 */
void TX_Call_phone(void)
{
    Serial1_SendString("ATD" TARGET_PHONE_NUM ";\r\n");
    printf("Calling: %s\n", TARGET_PHONE_NUM);
}

/**
 * @brief 发送短信
 * @details 向预设号码发送Unicode编码的求救短信
 * @note 短信内容为预定义的二进制数据
 */

void TX_SendMssage(void)
{
    char cmd[64];
    char ucs2_phone[64] = {0}; // 存放转换后的 UCS2 号码
    
    // 1. 核心魔法：把普通手机号 "151..." 自动转换成 "003100350031..."
    for(int i = 0; TARGET_PHONE_NUM[i] != '\0'; i++) {
        sprintf(&ucs2_phone[i * 4], "003%c", TARGET_PHONE_NUM[i]);
    }
    
    // 2. 发送带有 UCS2 格式号码的 CMGS 指令
    sprintf(cmd, "AT+CMGS=\"%s\"\r\n", ucs2_phone);
    Serial1_SendString(cmd);
    
    // 3. 等待 4G 模块返回 '>' 符号 (极其重要！)
    delay_ms(500); 
    
    // 4. 发送中文: "发生危险，请救援！"
    Serial1_SendString("53D1751F53719669FF0C8BF7655163F4FF01");
    
    delay_ms(100);
    
    // 5. 发送 Ctrl+Z 结束并发送
    Serial1_SendByte(0x1A);  
    
    // 6. 留出时间给基站发射信号
    delay_ms(3000); 
}

//void TX_SendMssage(void)
//{
//    uint8_t Message[] = {
//        0x7F, 0xFB, 0x8F, 0x66, 0x4E, 0x86, 0xFF, 0x0C,
//        0x62, 0x11, 0x97, 0x00, 0x89, 0x81, 0x5E, 0x2E, 0x52, 0xA9
//    };
//    Serial1_SendString("AT+CMGS=\"17688605321\"\r");
//    delay_ms(100);
//    Serial1_SendArray(Message, 18);
//    Serial1_SendByte(0x1A);  /* Ctrl+Z 结束短信 */
//}

/**
 * @brief 触发紧急求救
 * @details 依次执行: 初始化4G模块 -> 发送短信 -> 等待1秒 -> 拨打电话
 * @note 用于摔倒检测后的紧急响应
 */
void qiujiu(void)
{
    TX_4G_Init();
    TX_SendMssage();
    delay_ms(1000);
    TX_Call_phone();
}
