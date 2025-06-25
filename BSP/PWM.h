/**
 ****************************************************************************************************
 * @file        PWM.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-20
 * @brief       舵机控制相关代码的头文件，以SG90为例子
 ****************************************************************************************************
 */
#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

//使用舵机需要在main.c添加： PWM_Init();

/* 添加舵机角度范围宏定义 */
#define SERVO_MIN_ANGLE		0		// 舵机最小角度
#define SERVO_MAX_ANGLE		180		// 舵机最大角度
#define SERVO_MIN_PULSE 	500		// 0.5ms (0°)
#define SERVO_MAX_PULSE 	2500	// 2.5ms (180°)
#define PWM_PERIOD			20000   // 20ms周期(50Hz)
#define PWM_PRESCALER		72      // 72MHz/72 = 1MHz (1us分辨率)

/********************************* 复用IO引脚的通用定时器功能进行PWM输出 *************************************/

#define TIMX_PWM_CHY_GPIO_PORT     GPIOC
#define TIMX_PWM_CHY_GPIO_PIN      GPIO_Pin_6
#define TIMX_PWM_CHY_GPIO_CLK      RCC_APB2Periph_GPIOC

#define TIMX_PWM                   TIM8
#define TIMX_PWM_CHY               TIM_Channel_1
#define TIMX_PWM_CHY_CLK           RCC_APB2Periph_TIM8

/********************************************* 其他外部函数 *************************************************/

void PWM_Init(void);

uint16_t Set_Angle(float angle);								//0-180 
extern uint16_t Set_Angle(float angle);							//添加全局声明 
void Update_PWM(uint16_t pulse);  
void Increase_PWM_Angle(void);
void Decrease_PWM_Angle(void);
void MoveToAngle(float targetAngle);							//移动到指定角度（0-180°）

#endif
