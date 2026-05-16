#include "app_esp8266.h"

#include "delay.h"
#include "usart3.h"
#include <stdio.h>

static void ESP8266_SendATCommand(const char *cmd, const char *expected, uint16_t timeout, const char *success_msg, const char *fail_msg);
static void ESP8266_FloatToString(float value, char *str, int precision);

void ESP8266_Init(void)
{
    USART3_ClearBuffer();
    ESP8266_SendATCommand("AT", "OK", 1000, "AT OK", "AT FAIL");
    delay_ms(500);
    ESP8266_SendATCommand("AT+RST", "ready", 5000, "Reset OK", "Reset FAIL");
    delay_ms(2000);
    ESP8266_SendATCommand("AT+CWMODE=1", "OK", 1000, "STA Mode OK", "STA Mode FAIL");
    delay_ms(500);
    ESP8266_SendATCommand("AT+CIPMUX=0", "OK", 1000, "Single Conn OK", "Single Conn FAIL");
}

void ESP8266_ConnectWiFi(void)
{
    char cmd[100];

    USART3_ClearBuffer();
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASSWD);
    ESP8266_SendATCommand(cmd, "WIFI GOT IP", 10000, "WiFi Connected", "WiFi Connect FAIL");
}

void ESP8266_MQTT_Init(void)
{
    char cmd[300];

    ESP8266_SendATCommand("AT+MQTTCLEAN=0", "OK", 2000, "MQTT Clean OK", "MQTT Clean");
    sprintf(cmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"",
            ONENET_DEVICE_ID, ONENET_MQTT_USER, ONENET_MQTT_PASS);
    ESP8266_SendATCommand(cmd, "OK", 3000, "MQTT User Config OK", "MQTT User Config FAIL");
}

void ESP8266_MQTT_Connect(void)
{
    char cmd[100];

    sprintf(cmd, "AT+MQTTCONN=0,\"%s\",%d,1", ONENET_MQTT_BROKER, ONENET_MQTT_PORT);
    ESP8266_SendATCommand(cmd, "+MQTTCONNECTED", 10000, "MQTT Connected", "MQTT Connect FAIL");
}

void ESP8266_MQTT_PublishAll(float gyro_x, float gyro_y, float gyro_z, int32_t radar_speed)
{
    char cmd[512];
    char str_x[16];
    char str_y[16];
    char str_z[16];

    ESP8266_FloatToString(gyro_x, str_x, 2);
    ESP8266_FloatToString(gyro_y, str_y, 2);
    ESP8266_FloatToString(gyro_z, str_z, 2);

    sprintf(cmd, "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"gyro_x\\\":{\\\"value\\\":%s}\\,\\\"gyro_y\\\":{\\\"value\\\":%s}\\,\\\"gyro_z\\\":{\\\"value\\\":%s}\\,\\\"radar_speed\\\":{\\\"value\\\":%d}}}\",0,0",
            ONENET_PRODUCT_ID, ONENET_DEVICE_ID, str_x, str_y, str_z, (int)radar_speed);

    ESP8266_SendATCommand(cmd, "OK", 5000, "All Data Published", "Publish FAIL");
}

void ESP8266_MQTT_PublishGyroX(float gyro_x)
{
    char cmd[256];
    char str_val[16];

    ESP8266_FloatToString(gyro_x, str_val, 2);
    sprintf(cmd, "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"gyro_x\\\":{\\\"value\\\":%s}}}\",0,0",
            ONENET_PRODUCT_ID, ONENET_DEVICE_ID, str_val);

    ESP8266_SendATCommand(cmd, "OK", 3000, "Gyro_X Published", "Publish FAIL");
}

void ESP8266_MQTT_PublishGyroY(float gyro_y)
{
    char cmd[256];
    char str_val[16];

    ESP8266_FloatToString(gyro_y, str_val, 2);
    sprintf(cmd, "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"gyro_y\\\":{\\\"value\\\":%s}}}\",0,0",
            ONENET_PRODUCT_ID, ONENET_DEVICE_ID, str_val);

    ESP8266_SendATCommand(cmd, "OK", 3000, "Gyro_Y Published", "Publish FAIL");
}

void ESP8266_MQTT_PublishGyroZ(float gyro_z)
{
    char cmd[256];
    char str_val[16];

    ESP8266_FloatToString(gyro_z, str_val, 2);
    sprintf(cmd, "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"gyro_z\\\":{\\\"value\\\":%s}}}\",0,0",
            ONENET_PRODUCT_ID, ONENET_DEVICE_ID, str_val);

    ESP8266_SendATCommand(cmd, "OK", 3000, "Gyro_Z Published", "Publish FAIL");
}

void ESP8266_MQTT_PublishRadarSpeed(int32_t radar_speed)
{
    char cmd[256];

    sprintf(cmd, "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"radar_speed\\\":{\\\"value\\\":%d}}}\",0,0",
            ONENET_PRODUCT_ID, ONENET_DEVICE_ID, (int)radar_speed);

    ESP8266_SendATCommand(cmd, "OK", 3000, "Radar Speed Published", "Publish FAIL");
}

void ESP8266_MQTT_Publish(int temperature)
{
    printf("Warning: Deprecated function called, temperature=%d\r\n", temperature);
}

void ESP8266_MQTT_PublishGPS(float latitude_deg, float longitude_deg)
{
    char cmd[384];
    char str_lat[16];
    char str_lng[16];

    ESP8266_FloatToString(latitude_deg, str_lat, 6);
    ESP8266_FloatToString(longitude_deg, str_lng, 6);
    sprintf(cmd, "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"latitude\\\":{\\\"value\\\":%s}\\,\\\"longitude\\\":{\\\"value\\\":%s}}}\",0,0",
            ONENET_PRODUCT_ID, ONENET_DEVICE_ID, str_lat, str_lng);

    ESP8266_SendATCommand(cmd, "OK", 3000, "GPS Publish Success", "GPS Publish FAIL");
}

void ESP8266_MQTT_PublishSensor(int roll, int pitch, int yaw, int radar_speed)
{
    ESP8266_MQTT_PublishAll((float)roll, (float)pitch, (float)yaw, (int32_t)radar_speed);
}

void ESP8266_AutoConnect(void)
{
    printf("\r\n================ ESP8266 Connection Start ================\r\n");
    ESP8266_Init();
    ESP8266_ConnectWiFi();
    ESP8266_MQTT_Init();
    ESP8266_MQTT_Connect();
    printf("================ ESP8266 Connection Complete ================\r\n");
}

static void ESP8266_SendATCommand(const char *cmd, const char *expected, uint16_t timeout, const char *success_msg, const char *fail_msg)
{
    USART3_ClearBuffer();
    printf("[ESP] Send: %s\r\n", cmd);
    USART3_SendCmd(cmd);

    if (USART3_WaitFor(expected, timeout))
    {
        printf("[ESP] Success: %s\r\n", success_msg);
    }
    else
    {
        printf("[ESP] Fail: %s\r\n", fail_msg);
    }
}

static void ESP8266_FloatToString(float value, char *str, int precision)
{
    int int_part;
    int frac_part;
    int multiplier = 1;
    int i;

    if (value < 0.0f)
    {
        *str++ = '-';
        value = -value;
    }

    for (i = 0; i < precision; i++)
    {
        multiplier *= 10;
    }

    int_part = (int)value;
    frac_part = (int)((value - (float)int_part) * (float)multiplier + 0.5f);

    if (precision > 0)
    {
        sprintf(str, "%d.%0*d", int_part, precision, frac_part);
    }
    else
    {
        sprintf(str, "%d", int_part);
    }
}
