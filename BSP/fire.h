/****************************************************************************************************
 * @file        fire.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-22
 * @brief       ���ּ��ģ��ͷ�ļ������洫�����ӿڶ���
 * @details     ���ļ����壺
 *              - ���洫����Ӳ���������ã�GPIOC Pin10��
 *              - ���ּ�⹦�ܺ����ӿ�����
 *              Ӳ�����ӣ�
 *              - ��������PC10���͵�ƽ��ʾ����״̬��
 * @note        ����fire.c���ʹ�ã��ṩ���ּ��ĺ��Ľӿ�
 ****************************************************************************************************/
#ifndef _FIRE_H
#define _FIRE_H

#include "stm32f10x.h"

#define GPIO_FIRE_RCC RCC_APB2Periph_GPIOC
#define GPIO_FIRE_PORT GPIOC
#define GPIO_FIRE_PIN GPIO_Pin_10

void Fire_Init(void);
void Fire_Sensor_Init(void);
uint8_t Fire_GetStatus(void);// ����״̬��ȡ����

#endif
