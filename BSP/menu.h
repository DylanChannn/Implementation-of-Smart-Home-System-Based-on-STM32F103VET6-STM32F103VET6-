/****************************************************************************************************
 * @file        menu.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-23
 * @brief       ���ܼҾӿ���ϵͳ�˵�ģ��ͷ�ļ�������˵�״̬�ͽӿ�
 * @details     ��ͷ�ļ�����˵�ϵͳ�ĺ���Ԫ�أ�
 *              - �˵�״̬ö�٣�MENU_MAIN��MENU_CLOUD��7��״̬��
 *              - �����¼����壨KEY_NONE��KEY1_PRESS�ȣ�
 *              - ȫ��״̬��������
 *              - �˵�������ԭ������
 * @note        ��menu.c�ļ����ʹ�ã����������Ĳ˵�״̬��ϵͳ
 ****************************************************************************************************
 */
#ifndef _MENU_H
#define _MENU_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

/* �˵�״̬ö�� */
typedef enum {
    MENU_MAIN = 0,       // ���˵���ѡ����Ʒ�ʽ��
    MENU_LOCAL,          // ���ؿ��Ʋ˵�
    MENU_CARD_STATUS,    // ˢ��״̬��ʾ
    MENU_LIGHT,          // LED����
    MENU_CURTAIN,        // ��������
    MENU_TEMP_HUM,       // ��ʪ�Ⱥͻ�����ʾ
    MENU_CLOUD           // �ͷ��ƿ���
} MenuState;

/* �������� */
#define KEY_NONE     0
#define KEY1_PRESS   1
#define KEY2_PRESS   2
#define KEY3_PRESS   3
#define KEY4_PRESS   4

extern MenuState currentMenu;    			// ����ȫ�ֱ���
extern uint8_t fireStatus;  	 			// 0:�޻��� 1:�л���
extern uint32_t lastTempUpdate;  			//��ʱˢ����ʪ�����ݻ���

/* �˵������� */
void Menu_Init(void);
void Menu_Update(uint8_t key);
void Refresh_Current_Menu(void);

#endif
