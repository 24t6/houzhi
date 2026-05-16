/**
 **************************************************************************************************
 * @file        LatticeLed.c
 * @brief       点阵显示驱动实现
 * @description 实现Max7219芯片驱动的8x8点阵显示，支持多级联显示
 * @note        硬件连接: PB12(CLK), PB13(CS), PB14(DIN)
 * @author      
 * @date        2024
 * @version     V1.0
 **************************************************************************************************
 */

#include "LatticeLed.h"
#include "delay.h"

/* Max7219寄存器地址 */
#define DECODEMODE    0x09   ///< 译码模式寄存器
#define INTENSITY     0x0A   ///< 亮度寄存器
#define SCANLIMT      0x0B   ///< 扫描限制寄存器
#define SHUTDOWN      0x0C   ///< 关断模式寄存器
#define DISPLAYTEST   0x0F   ///< 显示测试寄存器

#define CLK(x)        do { if (x) GPIO_SetBits(GPIOB, GPIO_Pin_12); else GPIO_ResetBits(GPIOB, GPIO_Pin_12); } while(0)
#define CS(x)         do { if (x) GPIO_SetBits(GPIOB, GPIO_Pin_13); else GPIO_ResetBits(GPIOB, GPIO_Pin_13); } while(0)
#define DIN(x)        do { if (x) GPIO_SetBits(GPIOB, GPIO_Pin_14); else GPIO_ResetBits(GPIOB, GPIO_Pin_14); } while(0)

uint8_t displayData2[8][8] = {
    {0x10,0x18,0x14,0x10,0x10,0x10,0x10,0x10},
    {0x7E,0x2,0x2,0x7E,0x40,0x40,0x40,0x7E},
    {0x3E,0x2,0x2,0x3E,0x2,0x2,0x3E,0x0},
    {0x8,0x18,0x28,0x48,0xFE,0x8,0x8,0x8},
    {0x3C,0x20,0x20,0x3C,0x4,0x4,0x3C,0x0},
    {0x3C,0x20,0x20,0x3C,0x24,0x24,0x3C,0x0},
    {0x3E,0x22,0x4,0x8,0x8,0x8,0x8,0x8},
    {0x0,0x3E,0x22,0x22,0x3E,0x22,0x22,0x3E}
};

uint8_t you_tsd[8][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF},
    {0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00},
    {0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80}
};

uint8_t zuo_tsd[8][8] = {
    {0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF},
    {0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01},
    {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

uint8_t selectedChips[] = {0,1,2,3,4,5,6,7};
uint8_t numSelectedChips = 8;

/**
 * @brief 配置GPIO引脚
 * @note 配置PB12, PB13, PB14为推挽输出
 */
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

/**
 * @brief 开始写入数据
 * @note CS拉低
 */
void StartWrite(void)
{
    CS(0);
}
 
/**
 * @brief 停止写入数据
 * @note CS拉高
 */
void StopWrite(void)
{
    CS(1);
}

/**
 * @brief 锁存数据
 * @note CS产生一个上升沿脉冲
 */
void Max7219_Lock(void)
{
    CS(1);
    CS(0);
}

/**
 * @brief 发送一个字节到Max7219
 * @param data 要发送的字节
 * @note 高位先发送
 */
void LatticeLedWrite_byte(uint8_t data)
{
    uint8_t i;
    for(i = 8; i > 0; i--)
    {
        if(data & 0x80)
            DIN(1);
        else
            DIN(0);
        CLK(1);
        CLK(0);
        data = data << 1;
    }
}
 
/**
 * @brief 写入命令到Max7219
 * @param addr 寄存器地址
 * @param data 数据
 */
void LatticeLed_Write_Command(uint8_t addr, uint8_t data)
{
    CLK(0);
    StartWrite();
    LatticeLedWrite_byte(addr);
    LatticeLedWrite_byte(data);
    StopWrite();
}
 
/**
 * @brief 初始化点阵显示
 * @note 配置Max7219工作模式，清空显示
 */
void LatticeLedInit(void)
{
    GPIO_Configuration();
    
    StartWrite();
    CLK(0);
    
    for(int i = 0; i < 8; i++)
    {
        LatticeLed_Write_Command(DECODEMODE, 0X00);
    }
    Max7219_Lock();
    
    for(int i = 0; i < 8; i++)
    {
        LatticeLed_Write_Command(INTENSITY, 0X01);
    }
    Max7219_Lock();
    
    for(int i = 0; i < 8; i++)
    {
        LatticeLed_Write_Command(SCANLIMT, 0X07);
    }
    Max7219_Lock();
    
    for(int i = 0; i < 8; i++)
    {
        LatticeLed_Write_Command(SHUTDOWN, 0X01);
    }
    Max7219_Lock();
    
    for(int i = 0; i < 8; i++)
    {
        LatticeLed_Write_Command(DISPLAYTEST, 0X00);
    }
    Max7219_Lock();
}

/**
 * @brief 清空所有点阵显示
 */
void LatticeLedCLS(void)
{
    uint8_t i;
    for(i = 8; i >= 1; i--)
    {
        LatticeLed_Write_Command(i, 0x00);
        if(i == 1) break;  /* 防止uint8_t下溢死循环 */
    }
}

/**
 * @brief 点亮所有8个点阵芯片
 */
void lian(void)
{
    LatticeLedAllOn(0);
    LatticeLedAllOn(1);
    LatticeLedAllOn(2);
    LatticeLedAllOn(3);
    LatticeLedAllOn(4);
    LatticeLedAllOn(5);
    LatticeLedAllOn(6);
    LatticeLedAllOn(7);
}

/**
 * @brief 关闭所有8个点阵芯片
 */
void mie(void)
{
    LatticeLedAllOff(0);
    LatticeLedAllOff(1);
    LatticeLedAllOff(2);
    LatticeLedAllOff(3);
    LatticeLedAllOff(4);
    LatticeLedAllOff(5);
    LatticeLedAllOff(6);
    LatticeLedAllOff(7);
}

/**
 * @brief 闪烁效果
 * @note 关闭->delay->点亮，循环显示
 */
void shansuo(void)
{
    delay_ms(700);
    mie();
    delay_ms(300);
    lian();
}

/**
 * @brief 点亮指定芯片的全部LED
 * @param chipIndex 芯片索引(0-7)
 */
void LatticeLedAllOn(uint8_t chipIndex)
{
    CLK(0);
    StartWrite();
    for (uint8_t i = 1; i <= 8; i++)
    {
        for (uint8_t j = 0; j < chipIndex; j++)
        {
            LatticeLedWrite_byte(0x00);
            LatticeLedWrite_byte(0x00);
        }
        LatticeLedWrite_byte(i);
        LatticeLedWrite_byte(0xFF);
        for (uint8_t j = chipIndex + 1; j < num_of_chips; j++)
        {
            LatticeLedWrite_byte(0x00);
            LatticeLedWrite_byte(0x00);
        }
        Max7219_Lock();
    }
}

/**
 * @brief 关闭指定芯片的全部LED
 * @param chipIndex 芯片索引(0-7)
 */
void LatticeLedAllOff(uint8_t chipIndex)
{
    CLK(0);
    StartWrite();
    for (uint8_t i = 1; i <= 8; i++)
    {
        for (uint8_t j = 0; j < chipIndex; j++)
        {
            LatticeLedWrite_byte(0x00);
            LatticeLedWrite_byte(0x00);
        }
        LatticeLedWrite_byte(i);
        LatticeLedWrite_byte(0x00);
        for (uint8_t j = chipIndex + 1; j < num_of_chips; j++)
        {
            LatticeLedWrite_byte(0x00);
            LatticeLedWrite_byte(0x00);
        }
        Max7219_Lock();
    }
}

/**
 * @brief 更新指定芯片的显示数据
 * @param chipIndex 芯片索引(0-7)
 * @param data 8x8显示数据
 */
void LatticeLedUpdateChip(uint8_t chipIndex, uint8_t data[8])
{
    CLK(0);
    StartWrite();
    for (uint8_t i = 0; i < 8; i++)
    {
        for (uint8_t j = 0; j < chipIndex; j++)
        {
            LatticeLedWrite_byte(0x00);
            LatticeLedWrite_byte(0x00);
        }
        LatticeLedWrite_byte(i + 1);
        LatticeLedWrite_byte(data[i]);
        for (uint8_t j = chipIndex + 1; j < num_of_chips; j++)
        {
            LatticeLedWrite_byte(0x00);
            LatticeLedWrite_byte(0x00);
        }
        Max7219_Lock();
    }
}

/**
 * @brief 更新指定芯片组的数据显示
 * @param chipIndices 芯片索引数组
 * @param numChips 芯片数量
 * @param data 显示数据数组
 */
void LatticeLedUpdateSpecificChips(uint8_t chipIndices[], uint8_t numChips, uint8_t data[][8])
{
    CLK(0);
    StartWrite();
    for (uint8_t c = 0; c < numChips; c++)
    {
        uint8_t selectedChip = chipIndices[c];
        for (uint8_t regIndex = 0; regIndex < 8; regIndex++)
        {
            for (uint8_t j = 0; j < selectedChip; j++)
            {
                LatticeLedWrite_byte(0x00);
                LatticeLedWrite_byte(0x00);
            }
            LatticeLedWrite_byte(regIndex + 1);
            LatticeLedWrite_byte(data[selectedChip][regIndex]);
            for (uint8_t j = selectedChip + 1; j < num_of_chips; j++)
            {
                LatticeLedWrite_byte(0x00);
                LatticeLedWrite_byte(0x00);
            }
            Max7219_Lock();
        }
    }
}

/**
 * @brief 右转提示动画
 * @note 闪烁后显示右转箭头图案
 */
void You_ts(void)
{
    delay_ms(700);
    mie();
    delay_ms(300);
    LatticeLedUpdateSpecificChips(selectedChips, numSelectedChips, you_tsd);
}

/**
 * @brief 左转提示动画
 * @note 闪烁后显示左转箭头图案
 */
void Zuo_ts(void)
{
    delay_ms(500);
    mie();
    delay_ms(300);
    LatticeLedUpdateSpecificChips(selectedChips, numSelectedChips, zuo_tsd);
}

void LatticeLedShowRightArrow(void)
{
    LatticeLedUpdateSpecificChips(selectedChips, numSelectedChips, you_tsd);
}

void LatticeLedShowLeftArrow(void)
{
    LatticeLedUpdateSpecificChips(selectedChips, numSelectedChips, zuo_tsd);
}

void LatticeLedShowIdle(void)
{
    mie();
}
