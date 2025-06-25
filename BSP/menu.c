/****************************************************************************************************
 * @file        menu.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-23
 * @brief       ���ܼҾӿ���ϵͳ�Ĳ˵�ģ�飺ʵ�ֶ༶�˵�����ʾ�밴������
 * @details     ���ļ������༶�˵���״̬����������ƺͰ��������ܣ�
 *              - ֧�����˵������ؿ��ơ�ˢ��״̬���ƹ���ơ��������ơ��������Ͱͷ���ƽ̨��7������
 *              - ͨ��ȫ��״̬������ǰ�˵�����
 *              - �ṩ�˵���ʼ����ˢ�¡����¹���
 *              - ������Ӧ�߼����ݵ�ǰ�˵�״̬��̬����
 * @note        ������ST7789����LCD��ʾ������������LED/���/�������ȣ��Ͱ���ɨ�貿��
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

/* ȫ�ֱ������� */
MenuState currentMenu = MENU_MAIN;
static uint8_t currentSelection = 0;  	// ��ǰѡ�еĲ˵���
static uint8_t led1State = 0;         	// LED1״̬��0��1��
static uint8_t led2State = 0;         	// LED2״̬��0��1��
static uint8_t curtainSelection = 0; 	// ��������ѡ��

/* �˵���������� */
const char* menuTitles[] = {
    "���˵�",
    "���ؿ���",
    "ˢ��״̬",
    "�ƹ����",
    "��������",
    "�������",
    "�ͷ���ƽ̨"
};

/* �˵������� */
const char* mainMenuItems[] = {"���ؿ���", "�ͷ���ƽ̨����", NULL};
const char* localMenuItems[] = {"ˢ��", "����", "����", "��/ʪ��/����/����", NULL};
const char* curtainMenuItems[] = {"�ش���", "������", "��ͣ", NULL};

/**
  * @brief  ��ʾ��ѡ��״̬�Ĳ˵���
  * @param  y: ��ֱλ�ã����أ�
  * @param  text: Ҫ��ʾ���ı�
  * @param  selected: ѡ��״̬��־��1Ϊѡ�У�0Ϊδѡ�У�
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
  * @brief  ��ʾ���˵�����
  * @param  None
  * @retval None
  */
void DisplayMenuMain(void) 
{
    ST7789_Clear(BLACK);
    
    /* ��ʾ���� */
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_MAIN]);
    
    /* ��ʾ�˵��� */
    for(uint8_t i = 0; i < 2; i++) 
	{
        ShowMenuItem(40 + i*20, mainMenuItems[i], (i == currentSelection));
    }
}

/**
  * @brief  ��ʾ���ؿ��Ʋ˵�����
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
  * @brief  ��ʾˢ��״̬����
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
        sprintf(cardStr, "����: %02X%02X%02X%02X", cardSerial[0], cardSerial[1], cardSerial[2], cardSerial[3]);
        ST7789_ShowString(20, 40, WHITE, BLACK, cardStr);
		LED1_Ctrl(LED_Open);
		LED2_Ctrl(LED_Open);
		LED3_Ctrl(LED_Open);
		LED4_Ctrl(LED_Open);
    } 
	else 
	{
        ST7789_ShowString(20, 40, WHITE, BLACK, "��δ�ҵ�");
		LED1_Ctrl(LED_Close);
		LED2_Ctrl(LED_Close);
		LED3_Ctrl(LED_Close);
		LED4_Ctrl(LED_Close);
    }
    
    ST7789_ShowString(20, 60, WHITE, BLACK, "KEY1: ����");
    ST7789_ShowString(20, 80, WHITE, BLACK, "KEY2: ����");
    ST7789_ShowString(20, 100, WHITE, BLACK, "KEY3: ����");
}

/**
  * @brief  ��ʾ�ƹ���ƽ���
  * @param  None
  * @retval None
  */
void DisplayMenuLight(void) 
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_LIGHT]);
    
    char statusText[20];
    snprintf(statusText, sizeof(statusText), "LED1: %s", led1State ? "��" : "��");
    ST7789_ShowString(20, 40, RED, BLACK, statusText);
    
    snprintf(statusText, sizeof(statusText), "LED2: %s", led2State ? "��" : "��");
    ST7789_ShowString(20, 60, YELLOW, BLACK, statusText);
    
    ST7789_ShowString(20, 80, RED, BLACK, "KEY1: LED1����");
    ST7789_ShowString(20, 100, YELLOW, BLACK, "KEY2: LED2����");
    ST7789_ShowString(20, 120, WHITE, BLACK, "KEY3: ����");
}

/**
  * @brief  ��ʾ�������ƽ���
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
	ST7789_ShowString(20, 120, WHITE, BLACK, "KEY3: ����");
}

/**
  * @brief  ��ʾ���������棨��ʪ��/����/����
  * @param  None
  * @retval None
  */
uint8_t fireStatus = 0;  // Ĭ���޻���
void DisplayMenuTempHum(void) 
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_TEMP_HUM]);
    
	ST7789_ShowString(20, 20, WHITE, BLACK, "��/ʪ��/����/����");
	
	/* ��ȡ��ʪ�����ݣ������ȡʧ����ʹ���ϴε�ֵ�� */
	DHT11_GetData(&DHT11_Data);
	
	/* �����У�����״̬ */
    fireStatus = Fire_GetStatus();		/* ��ȡ����״̬ */
    if(fireStatus) 
	{
        ST7789_ShowString(20, 80, BLACK, RED, "�л��֣�");
		BEEP_Ctrl(BEEP_Open);
    } 
	else 
	{
        ST7789_ShowString(20, 80, BLACK, GREEN, "�޻���");
		BEEP_Ctrl(BEEP_Close);
    }
    
	/* �����У�����PPMֵ��ȡ */
	float ppmValue = MQ2_GetData_PPM(); // ��ȡPPMֵ
    uint8_t gasAlarm = (ppmValue > MQ2_PPM_ALARM_THRESHOLD) ? 1 : 0; // �ж��Ƿ񳬱�
	
	/* ��ʽ����ʾ���� */
	char displayBuffer[32];
    snprintf(displayBuffer, sizeof(displayBuffer), "PPM: %.2f", ppmValue);
	
    if(gasAlarm) 
	{
        ST7789_ShowString(20, 100, RED, WHITE, displayBuffer);
		ST7789_ShowString(92, 100, RED, WHITE, "Ũ�ȹ���!");
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
		ST7789_ShowString(92, 100, LIGHTBLUE, WHITE, "Ũ������");
    }
	
    ST7789_ShowString(20, 120, WHITE, BLACK, "KEY3: ����");
}

/********************************************************** �ͷ���ƽ̨��ʾ **********************************************************/
/**
  * @brief  �ͷ���ƽ̨������ʾ����
  * @param  None
  * @retval None
  * @note   ������ʵ�ְͷ���ƽ̨�������ʾ���ܣ�
  *         1. ������״ν��룬��ʼ����ƽ̨����
  *         2. ��ʾ����״̬��ʾ
  *         3. ��ȡ����ʾ���»������ݣ���ʪ�ȡ�����Ũ�ȣ�
  *         4. ��������Ũ��ֵ����״̬��ʾ����ɫ���
  *         5. ��ʾ�û�������ʾ��KEY3���أ�
  */
uint8_t bemfaConfigured = 0;           // ��ʼδ����
uint32_t lastCloudTick = 0;            // �ϴ���ƽ̨���ݸ���ʱ��
_CloudIOT_Connect BemfaData;           // �ͷ��ƽṹ

void DisplayMenuCloud(void)
{
    ST7789_Clear(BLACK);
    ST7789_ShowString(20, 20, WHITE, BLACK, (char*)menuTitles[MENU_CLOUD]);
	
	/* �״ν���ʱ������ƽ̨ */
    if(!bemfaConfigured) 
	{
        ST7789_ShowString(30, 40, WHITE, BLACK, "���Ӱͷ���ƽ̨ing");
        Bemfa_Config();
        bemfaConfigured = 1;
    }
	/********************************** ��������ʪ�Ȳ�����ʾ **********************************/
	/* ˢ����ʪ������ */
    DHT11_RefreshData();
    
    /* ��ʾ��ʪ�� */
    DHT11_GetData(&DHT11_Data);
    
    /* ��ʾ����״̬ */
    ST7789_ShowString(20, 110, WHITE, BLACK, "KEY3: ����");
	
	/********************************** ��������������ʾ **********************************/
	/* ˢ�������������� */
    float ppm = MQ2_GetData_PPM();  // ��ȡ����PPMֵ
    char gasBuf[40];
	
	/* ��ʾ����Ũ�� */
    sprintf(gasBuf, "����Ũ��: %.2f PPM", ppm);
    ST7789_ShowString(20, 78, BLACK, ppm > MQ2_PPM_ALARM_THRESHOLD ? RED : GREEN, gasBuf);
	
	/* ��ʾ״̬��Ϣ�Ͳ�����ʾ */
    char statusMsg[40];
    sprintf(statusMsg, "����״̬: %s", ppm > MQ2_PPM_ALARM_THRESHOLD ? "Ũ�ȹ���!" : "Ũ������");
    ST7789_ShowString(20, 94, BLACK, ppm > MQ2_PPM_ALARM_THRESHOLD ? RED : GREEN, statusMsg);
}

/************************************************************************************************************************************/


/**
  * @brief  ��ʼ���˵�ϵͳ
  * @param  None
  * @retval None
  * @note   ����Ĭ�ϲ˵�Ϊ���˵�����ʼ�����˵�״̬����
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
  * @brief  ˢ�µ�ǰ�˵�����
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
  * @brief  �˵����´����������ݵ�ǰ�˵�״̬�������¼�
  * @param  key - ��������ֵ��KEY_NONE/KEY1_PRESS�Ȱ����¼���
  * @retval None
  * @note   ����һ��״̬��ʵ�ֵĶ༶�˵������������ݵ�ǰ�˵�״̬�Ͱ������룺
  *         1. �����˵��У�KEY1�л�ѡ�KEY2ȷ�Ͻ����¼��˵�
  *         2. �ڱ��ؿ��Ʋ˵���KEY1�л�ѡ�KEY2ȷ��ѡ��KEY3�������˵�
  *         3. ��ˢ��״̬�˵���KEY1���Ų�����KEY2���Ų�����KEY3����
  *         4. �ڵƹ���Ʋ˵���KEY1����LED1��KEY2����LED2��KEY3����
  *         5. �ڴ������Ʋ˵���KEY1�л�ѡ�KEY2ִ��ѡ�KEY3���أ�KEY4����ֹͣ
  *         6. �ڻ������˵���KEY3���أ���������ǿ��ˢ������
  *         7. �ڰͷ��Ʋ˵���KEY3�������˵�
  */
void Menu_Update(uint8_t key) 
{
    if(key == KEY_NONE) 
		return;
    
    switch(currentMenu) 
	{
		/********************************** ���˵� **********************************/
        case MENU_MAIN:  
            if(key == KEY1_PRESS) 
			{  
				// ��һ��
                currentSelection = (currentSelection + 1) % 2;
                DisplayMenuMain();
            } 
            else if(key == KEY2_PRESS) 
			{  
				// ȷ��
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
            
		/******************************** ���ؿ��Ʋ˵� ********************************/
        case MENU_LOCAL:
            if(key == KEY1_PRESS) 
			{  
				// ��һ��
                currentSelection = (currentSelection + 1) % 4;
                DisplayMenuLocal();
            } 
            else if(key == KEY2_PRESS) 
			{  
				// ȷ��
                switch(currentSelection) 
				{
                    case 0:  // ˢ������
                        currentMenu = MENU_CARD_STATUS;
                        DisplayMenuCardStatus();
                        break;
                    case 1:  // ����
                        currentMenu = MENU_LIGHT;
                        DisplayMenuLight();
                        break;
                    case 2:  // ���ش���
                        currentMenu = MENU_CURTAIN;
                        DisplayMenuCurtain();
                        break;
                    case 3:  // ��ʪ�Ⱥͻ���
                        currentMenu = MENU_TEMP_HUM;
                        DisplayMenuTempHum();
                        break;
                }
            } 
            else if(key == KEY3_PRESS) 
			{  
				/* ���� */
                currentMenu = MENU_MAIN;
                DisplayMenuMain();
            }
            break;
         
		/******************************** ˢ��״̬�˵� ********************************/
		extern void Increase_PWM_Angle(void);
		extern void Decrease_PWM_Angle(void);	
		extern float duty_circle_A;
		
        case MENU_CARD_STATUS:
			if(key == KEY1_PRESS) 
			{
				if(GetCardDetectStatus() == CARD_DETECTED) 
				{
					/* ���Ų��� */
					Increase_PWM_Angle();  // �������ӽǶȺ���
                    
                    /* ��ʾ��ǰ�Ƕ� */
                    char angleMsg[32];
                    snprintf(angleMsg, sizeof(angleMsg), "�Ƕ�: %.1f��", duty_circle_A);
                    ST7789_ShowString(140, 120, GREEN, BLACK, angleMsg);
					
					ST7789_ShowString(20, 120, GREEN, BLACK, "������ing");
				}
				else 
				{
					ST7789_ShowString(20, 120, RED, BLACK, "δ�ҵ���!");
				}
			} 
			else if(key == KEY2_PRESS) 
			{  
				/* ���Ų��������ӽǶ� */
                Decrease_PWM_Angle();  // ���ü��ٽǶȺ���
                ST7789_ShowString(20, 120, GREEN, BLACK, "������ing");
                /* ��ʾ��ǰ�Ƕ� */
                char angleMsg[32];
                snprintf(angleMsg, sizeof(angleMsg), "�Ƕ�: %.1f��", duty_circle_A);
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
        
		/******************************** �ƹ���Ʋ˵� ********************************/
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
				/* ���� */
                currentMenu = MENU_LOCAL;
                DisplayMenuLocal();
				LED1_Ctrl(LED_Close);
				LED2_Ctrl(LED_Close);
            }
            break;
            
		/******************************** �������Ʋ˵� ********************************/
        case MENU_CURTAIN:
            if(key == KEY1_PRESS) 
            {  
                /* �л�����һ��ѡ�� */
                curtainSelection = (curtainSelection + 1) % 3;
                DisplayMenuCurtain();
            } 
            else if(key == KEY2_PRESS) 
            {
                /* ִ�е�ǰѡ��ѡ��Ĳ��� */
                switch(curtainSelection) 
                {
                    case 0: 
						/* �رմ��� */
						Motor_SetSpeed(49, 1);
						ST7789_ShowString(20, 100, GREEN, BLACK, "�ر�ing...");
                        break;
                    case 1: 
						/* �������� */
						Motor_SetSpeed(49, 0);
						ST7789_ShowString(20, 100, GREEN, BLACK, "����ing...");
                        break;
                    case 2: 
                        /* ִ��ֹͣ */
						Motor_Stop();
						ST7789_ShowString(20, 100, YELLOW, BLACK, "����ͣ");
                        break;
                }
            }
			
            else if(key == KEY3_PRESS) 
            {  
                /* ���� */
				Motor_Stop(); 
				ST7789_ShowString(20, 100, WHITE, BLACK, "                 ");
                currentMenu = MENU_LOCAL;
                DisplayMenuLocal();
            }
			
            else if(key == KEY4_PRESS) 
            {  
                /* ֹͣ */
				Motor_Stop();
				ST7789_ShowString(20, 100, YELLOW, BLACK, "����ͣ");
                /* ִ��ֹͣ */
                DisplayMenuCurtain();
            }
            break;
		
		/******************************** ��ʪ����ʾ�˵� ********************************/
        case MENU_TEMP_HUM:
            if(key == KEY3_PRESS) 
			{  
				/* ���� */
                currentMenu = MENU_LOCAL;
                DisplayMenuLocal();
            }
            /* �����������������KEY1/KEY2��ǿ��ˢ������ */
			else if(key != KEY_NONE)
			{
				DisplayMenuTempHum(); // ˢ����ʾ
			}
            break;
            
		/******************************** �ͷ���ƽ̨�˵� ********************************/
        case MENU_CLOUD:
			/* �ͷ���ƽ̨ */
            if(key == KEY3_PRESS) 
			{  
				/* ���� */
                currentMenu = MENU_MAIN;
                DisplayMenuMain();
            }
            break;
    }
}
