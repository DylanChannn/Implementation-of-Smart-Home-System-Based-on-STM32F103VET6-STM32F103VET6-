#ifndef _USART1_H
#define _USART1_H

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"

#define USART1_RX_len 256

typedef struct{
	uint8_t RxBuf[USART1_RX_len];
	uint16_t count;
	uint8_t flag;
}_USART_Data;

extern _USART_Data USART1_Data;

void USART1_Init(uint32_t BRR);
void USART1_SendByte(uint8_t byte);
void USART1_SendBuf(uint8_t* buf,uint16_t len);
void USART1_SendStr(char* str);
uint8_t Get_Add(uint8_t * buf,uint8_t len);
#endif
