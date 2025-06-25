/****************************************************************************************************
 * @file        fire.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-22
 * @brief       火灾检测模块头文件：火焰传感器接口定义
 * @details     本文件定义：
 *              - 火焰传感器硬件连接配置（GPIOC Pin10）
 *              - 火灾检测功能函数接口声明
 *              硬件连接：
 *              - 传感器：PC10（低电平表示火灾状态）
 * @note        需与fire.c配合使用，提供火灾检测的核心接口
 ****************************************************************************************************/
#ifndef _FIRE_H
#define _FIRE_H

#include "stm32f10x.h"

#define GPIO_FIRE_RCC RCC_APB2Periph_GPIOC
#define GPIO_FIRE_PORT GPIOC
#define GPIO_FIRE_PIN GPIO_Pin_10

void Fire_Init(void);
void Fire_Sensor_Init(void);
uint8_t Fire_GetStatus(void);// 火灾状态获取函数

#endif
