#ifndef __APP_CONTEXT_H
#define __APP_CONTEXT_H

#include <stdint.h>

#include "Radar.h"

#define IMPACT_WEIGHT_THRESHOLD  500
#define ANGLE_TILT_THRESHOLD     4000
#define TURN_YAW_TRIGGER         500
#define TURN_YAW_RELEASE         250
#define TURN_DISPLAY_HOLD_CYCLES 4
#define MQTT_UPLOAD_INTERVAL_MS  500
#define LOOP_DELAY_MS            50
#define SENSOR_PRINT_INTERVAL_MS 500
#define RADAR_PRINT_INTERVAL_MS  500
#define FRONT_SEND_INTERVAL_MS   100

typedef struct
{
    uint32_t distance;
    uint32_t distance1;
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    uint8_t tishi[3];
    uint8_t warning_flag;
    uint8_t cm_flag;
    uint32_t loop_counter;
    uint32_t sensor_print_counter;
    uint32_t radar_print_counter;
    uint32_t front_send_counter;
    uint8_t last_direction;
    uint8_t turn_state;
    uint8_t turn_display_hold;
    int16_t last_yaw_for_turn;
    uint8_t turn_yaw_initialized;
} AppState;

extern AppState g_app;
extern uint8_t target_count;
extern FrontRadar_Message targets[20];

#endif
