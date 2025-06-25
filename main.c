#include "stm32f10x.h"
#include "PWM.h"
#include "Motor.h"
#include "st7789.h"
#include "key.h"
#include "Cloudbemfa.h"
#include "esp01s.h"
#include "DHT11.h"
#include "gpio.h"
#include "Delay.h"
#include "usart1.h"
#include "fire.h"
#include "MQ2.h"
#include "rc522.h"

#include "menu.h"

int main(void)
{
	Delay_Init();
	
	USART1_Init(9600);
	
	LED_Init();
	BEEP_Init();
	Key1_Init();
	Key2_Init();
	Key3_Init();
	Key4_Init();
	
	LCD_Config();
	DHT11_Config();
	Fire_Init();
	MQ2_Init();
	MFRC522_Init();
	
	ST7789_ShowString(48, 60, BLUE, YELLOW, "连接WiFi模块ing...");
	ESP8266_Config();
	
	Menu_Init();
	
	PWM_Init();
	Motor_Init();
	
	while (1)
	{
		/***************************** 菜单操作部分 *****************************/
		/* 扫描按键 */
        uint8_t key = KEY_NONE;
        if(Key1_Scan()) key = KEY1_PRESS;
        else if(Key2_Scan()) key = KEY2_PRESS;
        else if(Key3_Scan()) key = KEY3_PRESS;
        else if(Key4_Scan()) key = KEY4_PRESS;
		
		/* 更新菜单 */
        Menu_Update(key);
		
		/* 处理电机状态 */
		Motor_RunHandler();
		
		/* RFID模块的卡片检测 */
		RC522_PeriodicCheck(); 
		
		/* 云平台数据处理（仅在云平台菜单中处理）*/
		if(currentMenu == MENU_CLOUD && bemfaConfigured) 
		{
			Cloud_Ctrl_LEDBEEP_DHT11_MOTOR();
		}
		
		/* 定期刷新需要实时更新的菜单 */
        static uint32_t lastTick = 0;
        if(Delay_GetTickCount() - lastTick > 2000) 
		{  
			/* 每2000ms刷新一次 */
            lastTick = Delay_GetTickCount();
            
            if(currentMenu == MENU_TEMP_HUM || currentMenu == MENU_CARD_STATUS || currentMenu == MENU_CLOUD) 
			{
                Refresh_Current_Menu();
            }
        }
    }
}
