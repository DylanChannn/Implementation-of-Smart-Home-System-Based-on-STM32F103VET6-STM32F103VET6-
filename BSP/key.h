/****************************************************************************************************
 * @file        key.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-18
 * @brief       ��������ͷ�ļ������尴��״̬���ӿں���
 * @details     ���ļ�������
 *              - ����״̬ö�ٶ���(Key_Idle/Key_Down/Key_Up)
 *              - �ĸ����������ĳ�ʼ����������
 *              - �ĸ�����������״̬ɨ�躯������
 * @note        ��������˵����
 *              - KEY1(PC2): �������룬����Ϊ�͵�ƽ
 *              - KEY2(PC3): �������룬����Ϊ�ߵ�ƽ
 *              - KEY3(PA0): �������룬����Ϊ�͵�ƽ
 *              - KEY4(PA1): �������룬����Ϊ�ߵ�ƽ
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

/* ������ʼ������ */
void Key1_Init(void);  // PC2: ��������-����͵�ƽ
void Key2_Init(void);  // PC3: ��������-����ߵ�ƽ
void Key3_Init(void);  // PA0: ��������-����͵�ƽ
void Key4_Init(void);  // PA1: ��������-����ߵ�ƽ

/* ����ɨ�躯�� */
uint8_t Key1_Scan(void);
uint8_t Key2_Scan(void);
uint8_t Key3_Scan(void);
uint8_t Key4_Scan(void);

#endif
