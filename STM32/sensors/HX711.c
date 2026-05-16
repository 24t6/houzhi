/**
 * @file    HX711.c
 * @brief   HX711称重传感器驱动
 * @details 实现三路HX711称重传感器数据读取
 * @note    通道1: SCK-PB1, DOUT-PB0
 *          通道2: SCK-PB8, DOUT-PB7
 *          通道3: SCK-PD2, DOUT-PD1
 */

#include "HX711.h"
#include "delay.h"

/* 全局变量 - 称重数据 */
u32 HX711_Buffer;      /**< 通道1原始数据 */
u32 Weight_Maopi;      /**< 通道1毛皮重量 */
s32 Weight_Shiwu;     /**< 通道1实际重量 */

u32 HX711_Buffer1;    /**< 通道2原始数据 */
u32 Weight_Maopi1;    /**< 通道2毛皮重量 */
s32 Weight_Shiwu1;    /**< 通道2实际重量 */

u32 HX711_Buffer2;    /**< 通道3原始数据 */
u32 Weight_Maopi2;    /**< 通道3毛皮重量 */
s32 Weight_Shiwu2;    /**< 通道3实际重量 */

/* 称重换算系数(根据实际传感器调整) */
#define GapValue  439.2f    /* 通道1，待修复后校准 */
#define GapValue1 400.0f    /* 通道2，约500g产生474,000变化 */
#define GapValue2 950.0f   /* 通道3，灵敏度很低，需实际校准 */

/**
 * @brief HX711引脚初始化
 * @note 配置三路SCK为输出,DOUT为输入上拉
 */
void Init_HX711pin(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* 通道1 */
    GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(HX711_SCK_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = HX711_DOUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(HX711_DOUT_PORT, &GPIO_InitStructure);

    /* 通道2 */
    GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(HX711_SCK_PORT1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = HX711_DOUT_PIN1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(HX711_DOUT_PORT1, &GPIO_InitStructure);

    /* 通道3 */
    GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(HX711_SCK_PORT2, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = HX711_DOUT_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(HX711_DOUT_PORT2, &GPIO_InitStructure);

    GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    GPIO_ResetBits(HX711_SCK_PORT1, HX711_SCK_PIN1);
    GPIO_ResetBits(HX711_SCK_PORT2, HX711_SCK_PIN2);
}

/**
 * @brief 读取通道1HX711数据
 * @return 24位AD原始值
 */
u32 HX711_Read(void)
{
    unsigned long count;
    unsigned char i;
    uint32_t timeout = 0; /* 新增超时计数 */

    GPIO_SetBits(HX711_DOUT_PORT, HX711_DOUT_PIN);
    delay_us(1);
    GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    count = 0;
    
    /* 防卡死机制：HX711最低转换率为10Hz(100ms)，我们等150ms */
    while (GPIO_ReadInputDataBit(HX711_DOUT_PORT, HX711_DOUT_PIN) == Bit_SET) {
        timeout++;
        delay_us(1);
        if (timeout > 150000) return 0; /* 超时直接跳出，返回0代表读取失败 */
    }

    for (i = 0; i < 24; i++) {
        GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN);
        count = count << 1;
        delay_us(1);
        GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
        if (GPIO_ReadInputDataBit(HX711_DOUT_PORT, HX711_DOUT_PIN) == Bit_SET) count++;
        delay_us(1);
    }
    GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    count = count ^ 0x800000;
    delay_us(1);
    GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    return count;
}

/**
 * @brief 读取通道2HX711数据
 */
u32 HX711_Read1(void)
{
    unsigned long count;
    unsigned char i;
    uint32_t timeout = 0;

    GPIO_SetBits(HX711_DOUT_PORT1, HX711_DOUT_PIN1);
    delay_us(1);
    GPIO_ResetBits(HX711_SCK_PORT1, HX711_SCK_PIN1);
    count = 0;
    
    while (GPIO_ReadInputDataBit(HX711_DOUT_PORT1, HX711_DOUT_PIN1) == Bit_SET) {
        timeout++;
        delay_us(1);
        if (timeout > 150000) return 0; 
    }

    for (i = 0; i < 24; i++) {
        GPIO_SetBits(HX711_SCK_PORT1, HX711_SCK_PIN1);
        count = count << 1;
        delay_us(1);
        GPIO_ResetBits(HX711_SCK_PORT1, HX711_SCK_PIN1);
        if (GPIO_ReadInputDataBit(HX711_DOUT_PORT1, HX711_DOUT_PIN1) == Bit_SET) count++;
        delay_us(1);
    }
    GPIO_SetBits(HX711_SCK_PORT1, HX711_SCK_PIN1);
    count = count ^ 0x800000;
    delay_us(1);
    GPIO_ResetBits(HX711_SCK_PORT1, HX711_SCK_PIN1);
    return count;
}

/**
 * @brief 读取通道3HX711数据
 */
u32 HX711_Read2(void)
{
    unsigned long count;
    unsigned char i;
    uint32_t timeout = 0;

    GPIO_SetBits(HX711_DOUT_PORT2, HX711_DOUT_PIN2);
    delay_us(1);
    GPIO_ResetBits(HX711_SCK_PORT2, HX711_SCK_PIN2);
    count = 0;
    
    while (GPIO_ReadInputDataBit(HX711_DOUT_PORT2, HX711_DOUT_PIN2) == Bit_SET) {
        timeout++;
        delay_us(1);
        if (timeout > 150000) return 0; 
    }

    for (i = 0; i < 24; i++) {
        GPIO_SetBits(HX711_SCK_PORT2, HX711_SCK_PIN2);
        count = count << 1;
        delay_us(1);
        GPIO_ResetBits(HX711_SCK_PORT2, HX711_SCK_PIN2);
        if (GPIO_ReadInputDataBit(HX711_DOUT_PORT2, HX711_DOUT_PIN2) == Bit_SET) count++;
        delay_us(1);
    }
    GPIO_SetBits(HX711_SCK_PORT2, HX711_SCK_PIN2);
    count = count ^ 0x800000;
    delay_us(1);
    GPIO_ResetBits(HX711_SCK_PORT2, HX711_SCK_PIN2);
    return count;
}

/**
 * @brief 获取毛皮重量(去皮)
 * @note 读取三路传感器数据作为基准
 */
void Get_Maopi(void)
{
    Weight_Maopi = HX711_Read();
    Weight_Maopi1 = HX711_Read1();
    Weight_Maopi2 = HX711_Read2();
}

/**
 * @brief 获取实际重量
 * @note 用当前值减去毛皮值,再除以换算系数得到实际重量
 */
void Get_Weight(void)
{
    HX711_Buffer = HX711_Read();
    if (HX711_Buffer > Weight_Maopi && HX711_Buffer != 0) {
        Weight_Shiwu = (s32)((float)(HX711_Buffer - Weight_Maopi) / GapValue);
    } else { Weight_Shiwu = 0; }

    HX711_Buffer1 = HX711_Read1();
    if (HX711_Buffer1 > Weight_Maopi1 && HX711_Buffer1 != 0) {
        Weight_Shiwu1 = (s32)((float)(HX711_Buffer1 - Weight_Maopi1) / GapValue1);
    } else { Weight_Shiwu1 = 0; }

    HX711_Buffer2 = HX711_Read2();
    if (HX711_Buffer2 > Weight_Maopi2 && HX711_Buffer2 != 0) {
        Weight_Shiwu2 = (s32)((float)(HX711_Buffer2 - Weight_Maopi2) / GapValue2);
    } else { Weight_Shiwu2 = 0; }
}


