#include "delay.h"
#include "sys.h"

static u8 fac_us = 0;
static u16 fac_ms = 0;
static volatile u32 sysTickCnt = 0;

void SysTick_Handler(void)
{
    sysTickCnt++;
}

u32 getSysTickCnt(void)
{
    return sysTickCnt;
}

void delay_init(u8 SYSCLK)
{
    u32 reload;

    SysTick->CTRL |= SysTick_CLKSource_HCLK;
    fac_us = SYSCLK;
    fac_ms = 1;

    reload = (u32)SYSCLK * 1000U;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD = reload - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void delay_us(u32 nus)
{
    u32 ticks;
    u32 told;
    u32 tnow;
    u32 tcnt = 0;
    u32 reload = SysTick->LOAD;

    ticks = nus * fac_us;
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void delay_ms(u32 nms)
{
    while (nms >= fac_ms)
    {
        nms -= fac_ms;
        delay_us(1000);
    }

    if (nms > 0)
    {
        delay_us(nms * 1000U);
    }
}

void delay_xms(u32 nms)
{
    u32 i;
    for (i = 0; i < nms; i++)
    {
        delay_us(1000);
    }
}