#include "app_sensor_fusion.h"

#include <stdio.h>

#include "HC-SR04.h"
#include "HX711.h"
#include "LatticeLed.h"
#include "app_context.h"
#include "gy95t.h"
#include "usart6.h"

#define TURN_STATE_IDLE   0U
#define TURN_STATE_LEFT   1U
#define TURN_STATE_RIGHT  2U

static void App_UpdateTurnIndicator(void)
{
    int16_t yaw_delta;

    if (g_app.turn_yaw_initialized == 0U)
    {
        g_app.last_yaw_for_turn = g_app.yaw;
        g_app.turn_yaw_initialized = 1U;
        g_app.turn_state = TURN_STATE_IDLE;
        LatticeLedShowIdle();
        return;
    }

    yaw_delta = (int16_t)(g_app.yaw - g_app.last_yaw_for_turn);
    g_app.last_yaw_for_turn = g_app.yaw;

    if (yaw_delta >= TURN_YAW_TRIGGER)
    {
        g_app.turn_state = TURN_STATE_RIGHT;
        g_app.turn_display_hold = TURN_DISPLAY_HOLD_CYCLES;
        LatticeLedShowRightArrow();
        printf("[TURN] Right detected, yaw_delta=%.2f deg\r\n", (float)yaw_delta / 100.0f);
        return;
    }

    if (yaw_delta <= -TURN_YAW_TRIGGER)
    {
        g_app.turn_state = TURN_STATE_LEFT;
        g_app.turn_display_hold = TURN_DISPLAY_HOLD_CYCLES;
        LatticeLedShowLeftArrow();
        printf("[TURN] Left detected, yaw_delta=%.2f deg\r\n", (float)yaw_delta / 100.0f);
        return;
    }

    if (g_app.turn_display_hold > 0U)
    {
        g_app.turn_display_hold--;

        if (g_app.turn_state == TURN_STATE_RIGHT)
        {
            LatticeLedShowRightArrow();
        }
        else if (g_app.turn_state == TURN_STATE_LEFT)
        {
            LatticeLedShowLeftArrow();
        }

        return;
    }

    if ((g_app.turn_state == TURN_STATE_RIGHT && yaw_delta <= TURN_YAW_RELEASE) ||
        (g_app.turn_state == TURN_STATE_LEFT && yaw_delta >= -TURN_YAW_RELEASE))
    {
        g_app.turn_state = TURN_STATE_IDLE;
        LatticeLedShowIdle();
    }
}

static void App_UpdateGyroAngles(void)
{
    GY95T_Data latest_data;

    if (GY95T_ReadLatest(&latest_data) != 0U)
    {
        g_app.roll = latest_data.roll;
        g_app.pitch = latest_data.pitch;
        g_app.yaw = latest_data.yaw;
        return;
    }

    g_app.roll = GY95T_GetRoll();
    g_app.pitch = GY95T_GetPitch();
    g_app.yaw = GY95T_GetYaw();
}

static void App_PrintSensorSnapshot(void)
{
    printf("Roll:%6d (%.2f deg) Pitch:%6d (%.2f deg) Yaw:%6d (%.2f deg) | W1:%4d W2:%4d W3:%4d | D1:%4d D2:%4d\r\n",
           g_app.roll, (float)g_app.roll / 100.0f,
           g_app.pitch, (float)g_app.pitch / 100.0f,
           g_app.yaw, (float)g_app.yaw / 100.0f,
           Weight_Shiwu, Weight_Shiwu1, Weight_Shiwu2,
           g_app.distance, g_app.distance1);
}

static void App_PrintRadarData(void)
{
    uint8_t i;

    printf("[RADAR] Target Count: %d, Alarm: %d\r\n", target_count, radar_alarm);

    for (i = 0; i < target_count && i < 20; i++)
    {
        printf("  Target[%d]: Angle=%d deg, Distance=%dm, Speed=%dkm/h, SNR=%d\r\n",
               i, targets[i].angle, targets[i].distance / 10, targets[i].speed, targets[i].snr);
    }

    if (target_count == 0)
    {
        printf("  No targets detected\r\n");
    }
}

static int8_t App_FindPrimaryRadarTarget(void)
{
    uint8_t i;
    int8_t best_index = -1;
    uint16_t best_distance = 0xFFFFu;

    for (i = 0; i < target_count && i < 20; i++)
    {
        uint16_t distance = targets[i].distance;
        int8_t angle = targets[i].angle;

        if (distance == 0U || distance > 500U)
        {
            continue;
        }

        if (angle < -45 || angle > 45)
        {
            continue;
        }

        if (best_index < 0 || distance < best_distance)
        {
            best_index = (int8_t)i;
            best_distance = distance;
        }
    }

    return best_index;
}

static uint8_t App_GetCarDirection(void)
{
    int8_t angle;
    int8_t target_index;

    target_index = App_FindPrimaryRadarTarget();
    if (target_index < 0)
    {
        return 0;
    }

    angle = targets[target_index].angle;

    if (angle >= -10 && angle <= 10)
    {
        g_app.last_direction = 1;
    }
    else if (angle >= -45 && angle < -10)
    {
        g_app.last_direction = 2;
    }
    else if (angle > 10 && angle <= 45)
    {
        g_app.last_direction = 3;
    }
    else
    {
        return 0;
    }

    return g_app.last_direction;
}

int32_t App_GetRadarSpeed(void)
{
    int8_t target_index = App_FindPrimaryRadarTarget();

    if (target_index >= 0)
    {
        return (int32_t)targets[target_index].speed;
    }

    return 0;
}

void App_SampleSensors(void)
{
    App_UpdateGyroAngles();
    App_UpdateTurnIndicator();

    Radar_10msTask();

    Get_Weight();
    g_app.distance = HC_SR04_Compute();
    g_app.distance1 = HC_SR04_Compute1();

    g_app.sensor_print_counter++;
    if (g_app.sensor_print_counter >= (SENSOR_PRINT_INTERVAL_MS / LOOP_DELAY_MS))
    {
        g_app.sensor_print_counter = 0;
        App_PrintSensorSnapshot();
    }
}

void App_ProcessRadar(void)
{
    uint8_t direction;
    uint8_t radar_speed;
    int32_t radar_speed_abs;
    int32_t radar_speed_signed;

    g_app.radar_print_counter++;
    if (g_app.radar_print_counter >= (RADAR_PRINT_INTERVAL_MS / LOOP_DELAY_MS))
    {
        g_app.radar_print_counter = 0;
        App_PrintRadarData();
    }

    g_app.front_send_counter++;
    if (g_app.front_send_counter < (FRONT_SEND_INTERVAL_MS / LOOP_DELAY_MS))
    {
        return;
    }

    g_app.front_send_counter = 0;

    direction = App_GetCarDirection();
    radar_speed_signed = App_GetRadarSpeed();
    radar_speed_abs = (radar_speed_signed < 0) ? (-radar_speed_signed) : radar_speed_signed;
    if (radar_speed_abs > 250)
    {
        radar_speed_abs = 250;
    }
    radar_speed = (uint8_t)radar_speed_abs;

    printf("[SEND->FRONT] UL:%dcm UR:%dcm Dir:%d Speed:%dkm/h (raw:%ld)\r\n",
           g_app.distance / 10, g_app.distance1 / 10, direction, radar_speed, radar_speed_signed);

    Serial6_SendRadarInfo((uint16_t)(g_app.distance / 10U),
                          (uint16_t)(g_app.distance1 / 10U),
                          direction,
                          radar_speed);
}
