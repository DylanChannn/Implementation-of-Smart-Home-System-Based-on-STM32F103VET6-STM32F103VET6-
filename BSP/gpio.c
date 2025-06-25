/****************************************************************************************************
 * @file        gpio.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-17
 * @brief       GPIO�������ģ�飺LED�ͷ�����Ӳ����ʼ�������
 * @details     ���ļ�ʵ�֣�
 *              - 4·LEDָʾ�ƿ��ƣ�PB8��PB9��PE0��PE1��
 *              - ���������ƣ�PC8��
 *              ���ž�����Ϊ�������ģʽ��50MHz��������
 * @note        ��ʼ����Ĭ�Ϲر������豸���ṩ�������ƽӿ�
 *****************************************************************************************************
 */
#include "gpio.h"

/**
  * @brief  LEDָʾ�Ƴ�ʼ��
  * @param  ��
  * @note   �������̣�
  *         1. ����GPIOB��GPIOE��APB2ʱ��
  *         2. ����PB8��PB9Ϊ�������ģʽ(CNF=00, MODE=11)
  *         3. ����PE0��PE1Ϊ�������ģʽ
  *         4. ��ʼ���ر�����LED
  * @retval ��
  */
/* LED���ų�ʼ�� */
void LED_Init(void)
{
		RCC->APB2ENR |= (1<<3)|(1<<6);//��GPIOB��GPIOEʱ��
		
		/* ��ӦPB8��PB9�������� */
		GPIOB->CRH &= ~(0xFF<<0);			//0-3��4-7bit��ȡ1111��Ȼ��ȡ������
		GPIOB->CRH |= (33<<0);				//0-3��4-7bit��ȡ0011������Ϊ��������� -> 00, 50MHz -> 11
		
		/* ��ӦPE0��PE1�������� */
		GPIOE->CRL &= ~(0xFF<<0);
		GPIOE->CRL |= (33<<0);
		
		LED1_Ctrl(LED_Close);
		LED2_Ctrl(LED_Close);
		LED3_Ctrl(LED_Close);
		LED4_Ctrl(LED_Close);
}

/**
  * @brief  ��������ʼ��
  * @param  ��
  * @note   �������̣�
  *         1. ����GPIOC��APB2ʱ��
  *         2. ����PC8Ϊ�������ģʽ(CNF=00, MODE=11)
  *         3. ��ʼ���رշ�����
  * @retval ��
  */
void BEEP_Init(void)
{
		RCC->APB2ENR |= (1<<4);				//��GPIOCʱ��
		
		GPIOC->CRH &= ~(0xF<<0);			//����
		GPIOC->CRH |= (3<<0);				//����Ϊ���졢50MHz
	
		BEEP_Ctrl(BEEP_Close);
}
