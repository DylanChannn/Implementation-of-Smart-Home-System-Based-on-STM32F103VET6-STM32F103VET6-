#ifndef _FontLIB_H_
#define _FontLIB_H_

#include "stm32f10x.h"
#include "Delay.h"
#include "stdio.h"
#include "string.h"

extern uint8_t FontLib_ASCII0816[];
extern uint8_t FontLib_Chinese1616[];
int GetOffset_ASCII(uint8_t font);
int GetOffset_Chinese1616(uint8_t font1,uint8_t font2);

#endif
