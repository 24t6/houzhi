#include "app_alarm.h"
#include "app_context.h"
#include "app_init.h"
#include "app_sensor_fusion.h"
#include "app_upload.h"
#include "delay.h"

int main(void)
{
    App_InitSystem();

    while (1)
    {
        App_SampleSensors();
        App_ProcessRadar();
        App_UpdateWarningState();
        App_ProcessEmergencyAlarm();
        App_ProcessUpload();
        delay_ms(LOOP_DELAY_MS);
    }
}
