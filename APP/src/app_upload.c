#include "app_upload.h"

#include <stdio.h>

#include "app_context.h"
#include "app_sensor_fusion.h"

void App_ProcessUpload(void)
{
    float roll_deg;
    float pitch_deg;
    float yaw_deg;
    int32_t radar_speed;

    g_app.loop_counter++;
    if (g_app.loop_counter < (MQTT_UPLOAD_INTERVAL_MS / LOOP_DELAY_MS))
    {
        return;
    }

    g_app.loop_counter = 0;

    roll_deg = (float)g_app.roll / 100.0f;
    pitch_deg = (float)g_app.pitch / 100.0f;
    yaw_deg = (float)g_app.yaw / 100.0f;
    radar_speed = App_GetRadarSpeed();

    // ESP8266_MQTT_PublishGyroX(roll_deg);
    // ESP8266_MQTT_PublishGyroY(pitch_deg);
    // ESP8266_MQTT_PublishGyroZ(yaw_deg);
    // ESP8266_MQTT_PublishRadarSpeed(radar_speed);
    // ESP8266_MQTT_PublishAll(roll_deg, pitch_deg, yaw_deg, radar_speed);

    printf("[MQTT] Upload: Roll=%.2f deg Pitch=%.2f deg Yaw=%.2f deg Speed=%d\r\n",
           roll_deg, pitch_deg, yaw_deg, radar_speed);
}
