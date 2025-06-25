/****************************************************************************************************
 * @file        gpio.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-17
 * @brief       GPIO�������ͷ�ļ���LED�ͷ�����Ӳ�����ƽӿ�
 * @details     ���ļ����壺
 *              - 4·LEDָʾ�ƵĿ��ƺ꣨PB8��PB9��PE0��PE1��
 *              - ���������ƺ꣨PC8��
 *              - �豸״̬ö������
 *              - Ӳ����ʼ����������
 * @note        ͨ���궨���ṩ�豸�ĸ�Ч���ƽӿڣ�֧�ֿ��غͷ�ת����
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
