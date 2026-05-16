#ifndef __APP_SENSOR_FUSION_H
#define __APP_SENSOR_FUSION_H

#include <stdint.h>

void App_SampleSensors(void);
void App_ProcessRadar(void);
int32_t App_GetRadarSpeed(void);

#endif
