/****************************************************************************************************
 * @file        menu.c
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-23
 * @brief       智能家居控制系统的菜单模块：实现多级菜单的显示与按键交互
 * @details     本文件包含多级菜单的状态管理、界面绘制和按键处理功能：
 *              - 支持主菜单、本地控制、刷卡状态、灯光控制、窗帘控制、环境监测和巴法云平台共7个界面
 *              - 通过全局状态机管理当前菜单界面
 *              - 提供菜单初始化、刷新、更新功能
 *              - 按键响应逻辑根据当前菜单状态动态调整
 * @note        依赖于ST7789进行LCD显示、外设驱动（LED/电机/传感器等）和按键扫描部分
 ****************************************************************************************************
 */
#include "menu.h"
#include "PWM.h"
#include "Motor.h"
#include "st7789.h"
#include "key.h"
#include "cloudbemfa.h"
#include "ESP01s.h"
#include "DHT11.h"
#include "gpio.h"
#include "Delay.h"
#include "usart1.h"
#include "fire.h"
#include "MQ2.h"
#include "rc522.h"
#include <string.h>

/* 全局变量定义 */
MenuState currentMenu = MENU_MAIN;
static uint8_t currentSelection = 0;  	// 当前选中的菜单项
static uint8_t led1State = 0;         	// LED1状态：0关1开
static uint8_t led2State = 0;         	// LED2状态：0关1开
static uint8_t curtainSelection = 0; 	// 窗帘控制选项

/* 菜单标题和内容 */
const char* menuTitles[] = {
    "主菜单",
    "本地控制",
    "刷卡状态",
    "灯光控制",
    "窗帘控制",
    "环境监测",
    "巴法云平台"
};

/* 菜单项内容 */
const char* mainMenuItems[] = {"本地控制", "巴法云平台控制", NULL};
const char* localMenuItems[] = {"刷卡", "开灯", "窗帘", "温/湿度/火灾/烟雾", NULL};
const char* curtainMenuItems[] = {"关窗帘", "开窗帘", "暂停", NULL};

/**
  * @brief  显示带选中状态的菜单项
  * @param  y: 垂直位置（像素）
  * @param  text: 要显示的文本
  * @param  selected: 选中状态标志（1为选中，0为未选中）
  * @retval None
  */
void ShowMenuItem(uint16_t y, const char* text, uint8_t selected) 
{
    if(selected)
	{
        ST7789_ShowString(20, y, BLACK, WHITE, (char*)text);
    } else 
	{
        ST7789_ShowString(20, y, WHITE, BLACK, (char*)text);
    }
}

/**
  * @brief  显示主菜单界面
  * @param  None
  * @retval None
  */
void DisplayMenuMain(void) 
{
    ST7789_Clear(BLACK);
    
    /* 显示标题 */
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_MAIN]);
    
    /* 显示菜单项 */
    for(uint8_t i = 0; i < 2; i++) 
	{
        ShowMenuItem(40 + i*20, mainMenuItems[i], (i == currentSelection));
    }
}

/**
  * @brief  显示本地控制菜单界面
  * @param  None
  * @retval None
  */
void DisplayMenuLocal(void) 
{
    ST7789_Clear(BLACK);
    
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_LOCAL]);
    
    for(uint8_t i = 0; i < 4; i++) 
	{
		ShowMenuItem(40 + i*20, localMenuItems[i], (i == currentSelection));
    }
}

/**
  * @brief  显示刷卡状态界面
  * @param  None
  * @retval None
  */
void DisplayMenuCardStatus(void) 
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_CARD_STATUS]);
    
	char detectResult = RC522_SingleDetectCard(cardSerial);
    cardStatus = (detectResult == MI_OK) ? CARD_DETECTED : CARD_NONE;

    if(cardStatus == CARD_DETECTED) 
	{
        char cardStr[30];
        sprintf(cardStr, "卡号: %02X%02X%02X%02X", cardSerial[0], cardSerial[1], cardSerial[2], cardSerial[3]);
        ST7789_ShowString(20, 40, WHITE, BLACK, cardStr);
		LED1_Ctrl(LED_Open);
		LED2_Ctrl(LED_Open);
		LED3_Ctrl(LED_Open);
		LED4_Ctrl(LED_Open);
    } 
	else 
	{
        ST7789_ShowString(20, 40, WHITE, BLACK, "卡未找到");
		LED1_Ctrl(LED_Close);
		LED2_Ctrl(LED_Close);
		LED3_Ctrl(LED_Close);
		LED4_Ctrl(LED_Close);
    }
    
    ST7789_ShowString(20, 60, WHITE, BLACK, "KEY1: 开门");
    ST7789_ShowString(20, 80, WHITE, BLACK, "KEY2: 关门");
    ST7789_ShowString(20, 100, WHITE, BLACK, "KEY3: 返回");
}

/**
  * @brief  显示灯光控制界面
  * @param  None
  * @retval None
  */
void DisplayMenuLight(void) 
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_LIGHT]);
    
    char statusText[20];
    snprintf(statusText, sizeof(statusText), "LED1: %s", led1State ? "开" : "关");
    ST7789_ShowString(20, 40, RED, BLACK, statusText);
    
    snprintf(statusText, sizeof(statusText), "LED2: %s", led2State ? "开" : "关");
    ST7789_ShowString(20, 60, YELLOW, BLACK, statusText);
    
    ST7789_ShowString(20, 80, RED, BLACK, "KEY1: LED1开关");
    ST7789_ShowString(20, 100, YELLOW, BLACK, "KEY2: LED2开关");
    ST7789_ShowString(20, 120, WHITE, BLACK, "KEY3: 返回");
}

/**
  * @brief  显示窗帘控制界面
  * @param  None
  * @retval None
  */
void DisplayMenuCurtain(void) 
{
    ST7789_Clear(BLACK);
    
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_CURTAIN]);
    
    for(uint8_t i = 0; i < 3; i++) 
	{
        ShowMenuItem(40 + i*20, curtainMenuItems[i], (i == curtainSelection));
    }
	ST7789_ShowString(20, 120, WHITE, BLACK, "KEY3: 返回");
}

/**
  * @brief  显示环境监测界面（温湿度/火灾/烟雾）
  * @param  None
  * @retval None
  */
uint8_t fireStatus = 0;  // 默认无火灾
void DisplayMenuTempHum(void) 
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_TEMP_HUM]);
    
	ST7789_ShowString(20, 20, WHITE, BLACK, "温/湿度/火灾/烟雾");
	
	/* 获取温湿度数据（如果获取失败则使用上次的值） */
	DHT11_GetData(&DHT11_Data);
	
	/* 第三行：火灾状态 */
    fireStatus = Fire_GetStatus();		/* 获取火灾状态 */
    if(fireStatus) 
	{
        ST7789_ShowString(20, 80, BLACK, RED, "有火灾！");
		BEEP_Ctrl(BEEP_Open);
    } 
	else 
	{
        ST7789_ShowString(20, 80, BLACK, GREEN, "无火灾");
		BEEP_Ctrl(BEEP_Close);
    }
    
	/* 第四行：烟雾PPM值读取 */
	float ppmValue = MQ2_GetData_PPM(); // 获取PPM值
    uint8_t gasAlarm = (ppmValue > MQ2_PPM_ALARM_THRESHOLD) ? 1 : 0; // 判断是否超标
	
	/* 格式化显示内容 */
	char displayBuffer[32];
    snprintf(displayBuffer, sizeof(displayBuffer), "PPM: %.2f", ppmValue);
	
    if(gasAlarm) 
	{
        ST7789_ShowString(20, 100, RED, WHITE, displayBuffer);
		ST7789_ShowString(92, 100, RED, WHITE, "浓度过高!");
		LED1_Ctrl(LED_Open);
		LED2_Ctrl(LED_Open); 
		LED3_Ctrl(LED_Open); 
		LED4_Ctrl(LED_Open); 
    } 
	else 
	{
        ST7789_ShowString(20, 100, LIGHTBLUE, WHITE, displayBuffer);
		LED1_Ctrl(LED_Close);
		LED2_Ctrl(LED_Close); 
		LED3_Ctrl(LED_Close); 
		LED4_Ctrl(LED_Close); 
		ST7789_ShowString(92, 100, LIGHTBLUE, WHITE, "浓度正常");
    }
	
    ST7789_ShowString(20, 120, WHITE, BLACK, "KEY3: 返回");
}

/********************************************************** 巴法云平台显示 **********************************************************/
/**
  * @brief  巴法云平台界面显示函数
  * @param  None
  * @retval None
  * @note   本函数实现巴法云平台界面的显示功能：
  *         1. 如果是首次进入，初始化云平台连接
  *         2. 显示连接状态提示
  *         3. 获取并显示最新环境数据（温湿度、烟雾浓度）
  *         4. 根据烟雾浓度值进行状态提示和颜色标记
  *         5. 显示用户操作提示（KEY3返回）
  */
uint8_t bemfaConfigured = 0;           // 初始未配置
uint32_t lastCloudTick = 0;            // 上次云平台数据更新时间
_CloudIOT_Connect BemfaData;           // 巴法云结构

void DisplayMenuCloud(void)
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_CLOUD]);
	
	/* 首次进入时配置云平台 */
    if(!bemfaConfigured) 
	{
        ST7789_ShowString(30, 40, WHITE, BLACK, "连接巴法云平台ing");
        Bemfa_Config();
        bemfaConfigured = 1;
    }
	/********************************** 以下是温湿度部分显示 **********************************/
	/* 刷新温湿度数据 */
    DHT11_RefreshData();
    
    /* 显示温湿度 */
    DHT11_GetData(&DHT11_Data);
    
    /* 显示连接状态 */
    ST7789_ShowString(20, 110, WHITE, BLACK, "KEY3: 返回");
	
	/********************************** 以下是烟雾部分显示 **********************************/
	/* 刷新烟雾传感器数据 */
    float ppm = MQ2_GetData_PPM();  // 获取烟雾PPM值
    char gasBuf[40];
	
	/* 显示烟雾浓度 */
    sprintf(gasBuf, "烟雾浓度: %.2f PPM", ppm);
    ST7789_ShowString(20, 78, BLACK, ppm > MQ2_PPM_ALARM_THRESHOLD ? RED : GREEN, gasBuf);
	
	/* 显示状态信息和操作提示 */
    char statusMsg[40];
    sprintf(statusMsg, "烟雾状态: %s", ppm > MQ2_PPM_ALARM_THRESHOLD ? "浓度过高!" : "浓度正常");
    ST7789_ShowString(20, 94, BLACK, ppm > MQ2_PPM_ALARM_THRESHOLD ? RED : GREEN, statusMsg);
}

/************************************************************************************************************************************/


/**
  * @brief  初始化菜单系统
  * @param  None
  * @retval None
  * @note   设置默认菜单为主菜单，初始化各菜单状态变量
  */
void Menu_Init(void) 
{
    currentMenu = MENU_MAIN;
    currentSelection = 0;
    led1State = 0;
    led2State = 0;
    curtainSelection = 0;
    
    DisplayMenuMain();
}

/**
  * @brief  刷新当前菜单界面
  * @param  None
  * @retval None
  */
void Refresh_Current_Menu(void) 
{
    switch(currentMenu) 
	{
        case MENU_MAIN:
            DisplayMenuMain();
            break;
        case MENU_LOCAL:
            DisplayMenuLocal();
            break;
        case MENU_CARD_STATUS:
            DisplayMenuCardStatus();
            break;
        case MENU_LIGHT:
            DisplayMenuLight();
            break;
        case MENU_CURTAIN:
            DisplayMenuCurtain();
            break;
        case MENU_TEMP_HUM:
            DisplayMenuTempHum();
            break;
        case MENU_CLOUD:
            DisplayMenuCloud();
            break;
    }
}

/**
  * @brief  菜单更新处理函数：根据当前菜单状态处理按键事件
  * @param  key - 按键输入值（KEY_NONE/KEY1_PRESS等按键事件）
  * @retval None
  * @note   这是一个状态机实现的多级菜单处理函数，根据当前菜单状态和按键输入：
  *         1. 在主菜单中：KEY1切换选项，KEY2确认进入下级菜单
  *         2. 在本地控制菜单：KEY1切换选项，KEY2确认选择，KEY3返回主菜单
  *         3. 在刷卡状态菜单：KEY1开门操作，KEY2关门操作，KEY3返回
  *         4. 在灯光控制菜单：KEY1控制LED1，KEY2控制LED2，KEY3返回
  *         5. 在窗帘控制菜单：KEY1切换选项，KEY2执行选项，KEY3返回，KEY4紧急停止
  *         6. 在环境监测菜单：KEY3返回，其他按键强制刷新数据
  *         7. 在巴法云菜单：KEY3返回主菜单
  */
void Menu_Update(uint8_t key) 
{
    if(key == KEY_NONE) 
		return;
    
    switch(currentMenu) 
	{
		/********************************** 主菜单 **********************************/
        case MENU_MAIN:  
            if(key == KEY1_PRESS) 
			{  
				// 下一条
                currentSelection = (currentSelection + 1) % 2;
                DisplayMenuMain();
            } 
            else if(key == KEY2_PRESS) 
			{  
				// 确认
                if(currentSelection == 0) 
				{
                    currentMenu = MENU_LOCAL;
                    DisplayMenuLocal();
                } 
				else 
				{
                    currentMenu = MENU_CLOUD;
                    DisplayMenuCloud();
                }
            }
            break;
            
		/******************************** 本地控制菜单 ********************************/
        case MENU_LOCAL:
            if(key == KEY1_PRESS) 
			{  
				// 下一条
                currentSelection = (currentSelection + 1) % 4;
                DisplayMenuLocal();
            } 
            else if(key == KEY2_PRESS) 
			{  
				// 确认
                switch(currentSelection) 
				{
                    case 0:  // 刷卡开门
                        currentMenu = MENU_CARD_STATUS;
                        DisplayMenuCardStatus();
                        break;
                    case 1:  // 开灯
                        currentMenu = MENU_LIGHT;
                        DisplayMenuLight();
                        break;
                    case 2:  // 开关窗帘
                        currentMenu = MENU_CURTAIN;
                        DisplayMenuCurtain();
                        break;
                    case 3:  // 温湿度和火情
                        currentMenu = MENU_TEMP_HUM;
                        DisplayMenuTempHum();
                        break;
                }
            } 
            else if(key == KEY3_PRESS) 
			{  
				/* 返回 */
                currentMenu = MENU_MAIN;
                DisplayMenuMain();
            }
            break;
         
		/******************************** 刷卡状态菜单 ********************************/
		extern void Increase_PWM_Angle(void);
		extern void Decrease_PWM_Angle(void);	
		extern float duty_circle_A;
		
        case MENU_CARD_STATUS:
			if(key == KEY1_PRESS) 
			{
				if(GetCardDetectStatus() == CARD_DETECTED) 
				{
					/* 开门操作 */
					Increase_PWM_Angle();  // 调用增加角度函数
                    
                    /* 显示当前角度 */
                    char angleMsg[32];
                    snprintf(angleMsg, sizeof(angleMsg), "角度: %.1f°", duty_circle_A);
                    ST7789_ShowString(140, 120, GREEN, BLACK, angleMsg);
					
					ST7789_ShowString(20, 120, GREEN, BLACK, "开门中ing");
				}
				else 
				{
					ST7789_ShowString(20, 120, RED, BLACK, "未找到卡!");
				}
			} 
			else if(key == KEY2_PRESS) 
			{  
				/* 关门操作：增加角度 */
                Decrease_PWM_Angle();  // 调用减少角度函数
                ST7789_ShowString(20, 120, GREEN, BLACK, "关门中ing");
                /* 显示当前角度 */
                char angleMsg[32];
                snprintf(angleMsg, sizeof(angleMsg), "角度: %.1f°", duty_circle_A);
                ST7789_ShowString(140, 120, GREEN, BLACK, angleMsg);
			}
			else if(key == KEY3_PRESS) 
			{
				LED1_Ctrl(LED_Close);
				LED2_Ctrl(LED_Close);
				LED3_Ctrl(LED_Close);
				LED4_Ctrl(LED_Close);
				currentMenu = MENU_LOCAL;
				DisplayMenuLocal();
			}
			else if(key == KEY_NONE) 
				return;
			break;
        
		/******************************** 灯光控制菜单 ********************************/
        case MENU_LIGHT:
            if(key == KEY1_PRESS) 
			{
                led1State = !led1State;
				LED1_Toggle;
                DisplayMenuLight();
            } 
            else if(key == KEY2_PRESS) 
			{
                led2State = !led2State;
                LED2_Toggle;
                DisplayMenuLight();
            } 
            else if(key == KEY3_PRESS) 
			{  
				/* 返回 */
                currentMenu = MENU_LOCAL;
                DisplayMenuLocal();
				LED1_Ctrl(LED_Close);
				LED2_Ctrl(LED_Close);
            }
            break;
            
		/******************************** 窗帘控制菜单 ********************************/
        case MENU_CURTAIN:
            if(key == KEY1_PRESS) 
            {  
                /* 切换到下一个选项 */
                curtainSelection = (curtainSelection + 1) % 3;
                DisplayMenuCurtain();
            } 
            else if(key == KEY2_PRESS) 
            {
                /* 执行当前选中选项的操作 */
                switch(curtainSelection) 
                {
                    case 0: 
						/* 关闭窗帘 */
						Motor_SetSpeed(49, 1);
						ST7789_ShowString(20, 100, GREEN, BLACK, "关闭ing...");
                        break;
                    case 1: 
						/* 开启窗帘 */
						Motor_SetSpeed(49, 0);
						ST7789_ShowString(20, 100, GREEN, BLACK, "开启ing...");
                        break;
                    case 2: 
                        /* 执行停止 */
						Motor_Stop();
						ST7789_ShowString(20, 100, YELLOW, BLACK, "已暂停");
                        break;
                }
            }
			
            else if(key == KEY3_PRESS) 
            {  
                /* 返回 */
				Motor_Stop(); 
				ST7789_ShowString(20, 100, WHITE, BLACK, "                 ");
                currentMenu = MENU_LOCAL;
                DisplayMenuLocal();
            }
			
            else if(key == KEY4_PRESS) 
            {  
                /* 停止 */
				Motor_Stop();
				ST7789_ShowString(20, 100, YELLOW, BLACK, "已暂停");
                /* 执行停止 */
                DisplayMenuCurtain();
            }
            break;
		
		/******************************** 温湿度显示菜单 ********************************/
        case MENU_TEMP_HUM:
            if(key == KEY3_PRESS) 
			{  
				/* 返回 */
                currentMenu = MENU_LOCAL;
                DisplayMenuLocal();
            }
            /* 如果按下其他键（如KEY1/KEY2）强制刷新数据 */
			else if(key != KEY_NONE)
			{
				DisplayMenuTempHum(); // 刷新显示
			}
            break;
            
		/******************************** 巴法云平台菜单 ********************************/
        case MENU_CLOUD:
			/* 巴法云平台 */
            if(key == KEY3_PRESS) 
			{  
				/* 返回 */
                currentMenu = MENU_MAIN;
                DisplayMenuMain();
            }
            break;
    }
}
