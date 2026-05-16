#ifndef __APP_ESP8266_H
#define __APP_ESP8266_H

#include <stdint.h>

#define WIFI_SSID             "1234"
#define WIFI_PASSWD           "12345678"
#define ONENET_DEVICE_ID      "test2"
#define ONENET_PRODUCT_ID     "1Fup7Aw2Px"
#define ONENET_MQTT_USER      "1Fup7Aw2Px"
#define ONENET_MQTT_PASS      "version=2018-10-31&res=products%2F1Fup7Aw2Px%2Fdevices%2Ftest2&et=1796948800&method=md5&sign=B8rBpXLuco5La6jK8KdHJg%3D%3D"
#define ONENET_MQTT_BROKER    "mqtts.heclouds.com"
#define ONENET_MQTT_PORT      1883

void ESP8266_Init(void);
void ESP8266_ConnectWiFi(void);
void ESP8266_MQTT_Init(void);
void ESP8266_MQTT_Connect(void);
void ESP8266_MQTT_PublishAll(float gyro_x, float gyro_y, float gyro_z, int32_t radar_speed);
void ESP8266_MQTT_PublishGyroX(float gyro_x);
void ESP8266_MQTT_PublishGyroY(float gyro_y);
void ESP8266_MQTT_PublishGyroZ(float gyro_z);
void ESP8266_MQTT_PublishRadarSpeed(int32_t radar_speed);
void ESP8266_MQTT_Publish(int temperature);
void ESP8266_MQTT_PublishGPS(float latitude_deg, float longitude_deg);
void ESP8266_MQTT_PublishSensor(int roll, int pitch, int yaw, int radar_speed);
void ESP8266_AutoConnect(void);

#endif
