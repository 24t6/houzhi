/**
 * @file    LatticeLed.h
 * @brief   MAX7219 lattice LED display driver
 */

#ifndef __LATTICELED_H
#define __LATTICELED_H

#include "stm32f4xx.h"

#define num_of_chips 8

extern uint8_t displayData2[8][8];
extern uint8_t you_tsd[8][8];
extern uint8_t zuo_tsd[8][8];
extern uint8_t selectedChips[];
extern uint8_t numSelectedChips;

void GPIO_Configuration(void);
void StartWrite(void);
void StopWrite(void);
void Max7219_Lock(void);
void LatticeLedWrite_byte(uint8_t data);
void LatticeLed_Write_Command(uint8_t addr, uint8_t data);
void LatticeLedInit(void);
void LatticeLedCLS(void);
void lian(void);
void mie(void);
void shansuo(void);
void You_ts(void);
void Zuo_ts(void);
void LatticeLedUpdateChip(uint8_t chipIndex, uint8_t data[8]);
void LatticeLedUpdateSpecificChips(uint8_t chipIndices[], uint8_t numChips, uint8_t data[][8]);
void LatticeLedAllOn(uint8_t chipIndex);
void LatticeLedAllOff(uint8_t chipIndex);
void LatticeLedShowRightArrow(void);
void LatticeLedShowLeftArrow(void);
void LatticeLedShowIdle(void);

#endif
