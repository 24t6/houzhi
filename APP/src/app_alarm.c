#include "app_alarm.h"

#include <stdio.h>

#include "4GTX.h"
#include "HX711.h"
#include "app_context.h"

void App_UpdateWarningState(void)
{
    uint8_t is_tilted;
    uint8_t is_impact;

    if (g_app.roll == 0)
    {
        return;
    }

    is_tilted = (g_app.roll < -ANGLE_TILT_THRESHOLD || g_app.roll > ANGLE_TILT_THRESHOLD);
    is_impact = (Weight_Shiwu > IMPACT_WEIGHT_THRESHOLD ||
                 Weight_Shiwu1 > IMPACT_WEIGHT_THRESHOLD ||
                 Weight_Shiwu2 > IMPACT_WEIGHT_THRESHOLD);

    if (is_tilted && is_impact)
    {
        g_app.warning_flag = 1;
        Weight_Shiwu = 0;
        Weight_Shiwu1 = 0;
        Weight_Shiwu2 = 0;
    }
    else
    {
        g_app.warning_flag = 0;
        g_app.cm_flag = 0;
    }
}

void App_ProcessEmergencyAlarm(void)
{
    if (g_app.warning_flag == 0 || g_app.cm_flag != 0)
    {
        return;
    }

    g_app.cm_flag = 1;
    printf("\r\n[SOS] IMPACT & TILT DETECTED! Executing 4G Protocol...\r\n");
    qiujiu();
    printf("[OK] 4G Protocol Executed Successfully!\r\n\r\n");
}
