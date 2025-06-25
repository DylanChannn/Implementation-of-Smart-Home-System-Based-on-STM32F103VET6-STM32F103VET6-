#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "st7789.h"


#define DHT11_OUT {GPIOD->CRL &= ~((uint32_t)0XF << 8);GPIOD->CRL |= ((uint32_t)0X3 << 8);}
#define DHT11_IN  {GPIOD->CRL &= ~((uint32_t)0XF << 8);GPIOD->CRL |= ((uint32_t)0X4 << 8);}

#define DHT11_PUT(x) ((x)?(GPIOD->ODR |= (1 << 2)):(GPIOD->ODR &= ~(1 << 2)))
#define DHT11_GET !!(GPIOD->IDR & (1 << 2))

typedef struct{
	uint8_t hum;
	float tem;
}_DHT11Data;

extern _DHT11Data DHT11_Data;
void DHT11_Config(void);
uint8_t DHT11_GetData(_DHT11Data *Data);
void DHT11_RefreshData(void);

#endif
