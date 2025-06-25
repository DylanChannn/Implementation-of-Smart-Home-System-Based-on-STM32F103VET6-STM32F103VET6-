/****************************************************************************************************
 * @file        gpio.h
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-17
 * @brief       GPIO外设控制头文件：LED和蜂鸣器硬件控制接口
 * @details     本文件定义：
 *              - 4路LED指示灯的控制宏（PB8、PB9、PE0、PE1）
 *              - 蜂鸣器控制宏（PC8）
 *              - 设备状态枚举类型
 *              - 硬件初始化函数声明
 * @note        通过宏定义提供设备的高效控制接口，支持开关和翻转操作
 ****************************************************************************************************/
#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f10x.h"

#define LED1_Ctrl(x) (x)?(GPIOB->ODR &= ~(1<<8)):(GPIOB->ODR |= (1<<8))
#define LED2_Ctrl(x) (x)?(GPIOB->ODR &= ~(1<<9)):(GPIOB->ODR |= (1<<9))
#define LED3_Ctrl(x) (x)?(GPIOE->ODR &= ~(1<<0)):(GPIOE->ODR |= (1<<0))
#define LED4_Ctrl(x) (x)?(GPIOE->ODR &= ~(1<<1)):(GPIOE->ODR |= (1<<1))

#define BEEP_Ctrl(x) (x)?(GPIOC->ODR |= (1<<8)):(GPIOC->ODR &= ~(1<<8))

#define LED1_Toggle GPIOB->ODR ^= (1<<8)
#define LED2_Toggle GPIOB->ODR ^= (1<<9) 
#define LED3_Toggle GPIOE->ODR ^= (1<<0) 
#define LED4_Toggle GPIOE->ODR ^= (1<<1) 
#define BEEP_Toggle GPIOC->ODR ^= (1<<8)

typedef enum{
	LED_Close = 0,
	LED_Open 
}_LED_State;

typedef enum{
	BEEP_Close = 0,
	BEEP_Open 
}_BEEP_State;

void LED_Init(void);
void BEEP_Init(void);

#endif
