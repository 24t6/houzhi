#include "app_init.h"

#include <stdio.h>
#include <string.h>

#include "4GTX.h"
#include "HC-SR04.h"
#include "HX711.h"
#include "LatticeLed.h"
#include "app_context.h"
#include "app_esp8266.h"
#include "delay.h"
#include "gy95t.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "usart5.h"
#include "usart6.h"

void App_InitSystem(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_init(168);

    USART1_Init(115200);
    USART2_Init(115200);
    USART3_Init(115200);
    UART5_Init(115200);
    UART4_Init(115200);
    USART6_Init(115200);

    printf("\r\n======================================================\r\n");
    printf("     Smart Vehicle Safety System - Core Booting...      \r\n");
    printf("======================================================\r\n");

    printf("[INIT] Calibrating Load Cells (Please DO NOT TOUCH!)...\r\n");
    Init_HX711pin();
    delay_ms(2000);
    Get_Maopi();
    delay_ms(100);
    Get_Maopi();
    delay_ms(100);
    Get_Maopi();

    printf("[INIT] Starting High-Speed Gyroscope...\r\n");
    GY95T_Init();

    printf("[INIT] Starting Ultrasonics...\r\n");
    HC_SR04_Init();
    HC_SR04_Init1();

    printf("[INIT] Starting LED Matrix...\r\n");
    LatticeLedInit();
    LatticeLedCLS();
    shansuo();
    mie();

    printf("[INIT] Waking up 4G Module (Network Registration)...\r\n");
    TX_4G_Init();

    printf("[INIT] Starting ESP8266 WiFi Module...\r\n");
    ESP8266_Init();
    delay_ms(1000);
    ESP8266_ConnectWiFi();
    delay_ms(2000);
    ESP8266_MQTT_Init();
    delay_ms(1000);
    ESP8266_MQTT_Connect();
    delay_ms(1000);

    printf("[INIT] Initializing Radar Buffer...\r\n");
    target_count = 0;
    memset((void *)targets, 0, sizeof(targets));
    memset(&g_app, 0, sizeof(g_app));

    printf("[SYS] All Hardware Online! Entering Core Loop...\r\n");
    printf("[SYS] MQTT Upload Interval: %d ms\r\n", MQTT_UPLOAD_INTERVAL_MS);
    printf("[SYS] Sensor Print Interval: %d ms\r\n", SENSOR_PRINT_INTERVAL_MS);
    printf("[SYS] Radar Print Interval: %d ms\r\n", RADAR_PRINT_INTERVAL_MS);
    printf("[SYS] Front Send Interval: %d ms\r\n", FRONT_SEND_INTERVAL_MS);
    printf("[SYS] Data Format: Float (Actual Angle in Degrees)\r\n");
}
