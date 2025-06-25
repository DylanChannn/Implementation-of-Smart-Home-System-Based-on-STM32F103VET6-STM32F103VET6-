/****************************************************************************************************
 * @file        menu.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-23
 * @brief       智能家居控制系统菜单模块头文件：定义菜单状态和接口
 * @details     本头文件定义菜单系统的核心元素：
 *              - 菜单状态枚举（MENU_MAIN至MENU_CLOUD共7种状态）
 *              - 按键事件定义（KEY_NONE、KEY1_PRESS等）
 *              - 全局状态变量声明
 *              - 菜单处理函数原型声明
 * @note        与menu.c文件配合使用，构建完整的菜单状态机系统
 ****************************************************************************************************
 */
#ifndef _MENU_H
#define _MENU_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

/* 菜单状态枚举 */
typedef enum {
    MENU_MAIN = 0,       // 主菜单（选择控制方式）
    MENU_LOCAL,          // 本地控制菜单
    MENU_CARD_STATUS,    // 刷卡状态显示
    MENU_LIGHT,          // LED控制
    MENU_CURTAIN,        // 窗帘控制
    MENU_TEMP_HUM,       // 温湿度和火情显示
    MENU_CLOUD           // 巴法云控制
} MenuState;

/* 按键定义 */
#define KEY_NONE     0
#define KEY1_PRESS   1
#define KEY2_PRESS   2
#define KEY3_PRESS   3
#define KEY4_PRESS   4

extern MenuState currentMenu;    			// 声明全局变量
extern uint8_t fireStatus;  	 			// 0:无火灾 1:有火灾
extern uint32_t lastTempUpdate;  			//定时刷新温湿度数据机制

/* 菜单处理函数 */
void Menu_Init(void);
void Menu_Update(uint8_t key);
void Refresh_Current_Menu(void);

#endif
