#ifndef __RADAR_H
#define __RADAR_H

#include "stm32f4xx.h"

typedef struct
{
    int8_t angle;
    uint16_t distance;
    int16_t speed;
    uint8_t snr;
} FrontRadar_Message;

extern FrontRadar_Message targets[20];
extern uint8_t target_count;
extern uint8_t radar_alarm;

void Radar_10msTask(void);

#endif
