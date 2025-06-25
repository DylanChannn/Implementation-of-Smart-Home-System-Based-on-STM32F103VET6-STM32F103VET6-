/**
 ****************************************************************************************************
 * @file        PWM.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-20
 * @brief       ���������ش����ͷ�ļ�����SG90Ϊ����
 ****************************************************************************************************
 */
#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

//ʹ�ö����Ҫ��main.c��ӣ� PWM_Init();

/* ��Ӷ���Ƕȷ�Χ�궨�� */
#define SERVO_MIN_ANGLE		0		// �����С�Ƕ�
#define SERVO_MAX_ANGLE		180		// ������Ƕ�
#define SERVO_MIN_PULSE 	500		// 0.5ms (0��)
#define SERVO_MAX_PULSE 	2500	// 2.5ms (180��)
#define PWM_PERIOD			20000   // 20ms����(50Hz)
#define PWM_PRESCALER		72      // 72MHz/72 = 1MHz (1us�ֱ���)

/********************************* ����IO���ŵ�ͨ�ö�ʱ�����ܽ���PWM��� *************************************/

#define TIMX_PWM_CHY_GPIO_PORT     GPIOC
#define TIMX_PWM_CHY_GPIO_PIN      GPIO_Pin_6
#define TIMX_PWM_CHY_GPIO_CLK      RCC_APB2Periph_GPIOC

#define TIMX_PWM                   TIM8
#define TIMX_PWM_CHY               TIM_Channel_1
#define TIMX_PWM_CHY_CLK           RCC_APB2Periph_TIM8

/********************************************* �����ⲿ���� *************************************************/

void PWM_Init(void);

uint16_t Set_Angle(float angle);								//0-180 
extern uint16_t Set_Angle(float angle);							//���ȫ������ 
void Update_PWM(uint16_t pulse);  
void Increase_PWM_Angle(void);
void Decrease_PWM_Angle(void);
void MoveToAngle(float targetAngle);							//�ƶ���ָ���Ƕȣ�0-180�㣩

#endif
