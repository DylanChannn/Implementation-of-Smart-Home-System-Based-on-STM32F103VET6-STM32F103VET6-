/****************************************************************************************************
 * @file        key.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-18
 * @brief       按键控制头文件：定义按键状态及接口函数
 * @details     本文件包含：
 *              - 按键状态枚举定义(Key_Idle/Key_Down/Key_Up)
 *              - 四个独立按键的初始化函数声明
 *              - 四个独立按键的状态扫描函数声明
 * @note        按键特性说明：
 *              - KEY1(PC2): 浮空输入，按下为低电平
 *              - KEY2(PC3): 浮空输入，按下为高电平
 *              - KEY3(PA0): 上拉输入，按下为低电平
 *              - KEY4(PA1): 下拉输入，按下为高电平
 ****************************************************************************************************
 */
#ifndef _KEY_H
#define _KEY_H

#include "stm32f10x.h"

typedef enum {
	Key_Idle = 0,
	Key_Down,
	Key_Up
}_Key_State;

/* 按键初始化函数 */
void Key1_Init(void);  // PC2: 浮空输入-点击低电平
void Key2_Init(void);  // PC3: 浮空输入-点击高电平
void Key3_Init(void);  // PA0: 上拉输入-点击低电平
void Key4_Init(void);  // PA1: 下拉输入-点击高电平

/* 按键扫描函数 */
uint8_t Key1_Scan(void);
uint8_t Key2_Scan(void);
uint8_t Key3_Scan(void);
uint8_t Key4_Scan(void);

#endif
