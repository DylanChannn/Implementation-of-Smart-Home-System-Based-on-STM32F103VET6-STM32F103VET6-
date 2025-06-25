#ifndef _DELAY_H
#define _DELAY_H

#include "gpio.h"
#include "stm32f10x.h"

extern uint16_t DHT11_Run[2];

void Delay_Init(void);					// ������ʼ������
extern volatile uint32_t systemTicks;	// ���ϵͳ�δ���
uint32_t Delay_GetTickCount(void);		// ��ȡϵͳ�δ���

void Delay_1us(void);
void Delay_us(uint32_t nus);
void Delay_ms(uint32_t nms);

#define delay_nms(x) delay_nus((x)*1000)

#endif
