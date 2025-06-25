/****************************************************************************************************
 * @file        motor.h
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-20
 * @brief       电机控制模块：包含状态定义、函数声明及系统时间相关接口
 * @details     本文件定义了电机的状态枚举、全局状态变量以及电机控制相关的函数接口，包括：
 *              - 定时器1中断初始化及系统时间获取函数
 *              - PWM初始化及电机控制函数
 *              - 状态处理及按键处理函数
 * @note        使用时需在main.c中调用按键初始化及状态处理函数
 ****************************************************************************************************
 */
#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

/* 电机状态枚举 */
typedef enum {
    MOTOR_STATE_IDLE,        	// 空闲状态（未初始化）
    MOTOR_STATE_READY,       	// 就绪状态（已初始化）
    MOTOR_STATE_FORWARD,     	// 正转运行状态
    MOTOR_STATE_BACKWARD,    	// 反转运行状态
    MOTOR_STATE_STOPPING,     	// 停止中状态
	MOTOR_STATE_CLOUD_FORWARD,  // 云平台控制正转
    MOTOR_STATE_CLOUD_BACKWARD, // 云平台控制反转
	MOTOR_STATE_MENU_FORWARD,   // 菜单控制正转
    MOTOR_STATE_MENU_BACKWARD   // 菜单控制反转
} MotorState;

/* 声明全局电机状态变量 */
extern MotorState motorState;

void Motor_PWM_Init(void);
void Motor_Init(void);
void Motor_Stop(void);
void Motor_SetSpeed(uint16_t dutyCycle, uint8_t direction);

/* 以下五个函数连同按键初始化代码KeyN_Init();一并放入main.c文件调用即可实现按键控制电机停止或者自动延时停止 */
void Motor_RunHandler(void);	// 电机运行时状态处理函数
void Motor_KEY1_Handler(void);
void Motor_KEY2_Handler(void);
void Motor_KEY3_Handler(void);
void Motor_KEY4_Handler(void);

#endif
