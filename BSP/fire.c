/****************************************************************************************************
 * @file        fire.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-22
 * @brief       ���ּ��ģ�飺���洫������ʼ����״̬���
 * @details     ���ļ�ʵ�֣�
 *              - ���洫����Ӳ����ʼ������������ģʽ��
 *              - ������ʵʱ�������ƣ���⵽����ʱ������
 *              - ����״̬��ȡ�ӿ�
 *              Ӳ�����ӣ�
 *              - ���洫������PC10���ţ��͵�ƽ��Ч��
 * @note        ʹ��������ⷽʽ���������͵�ƽ��ʾ��⵽���棬�������������ʵ�����ⱨ��
 *****************************************************************************************************
 */
#include "gpio.h"
#include "fire.h"

/**
  * @brief  ���洫����Ӳ����ʼ��
  * @param  ��
  * @note   �������̣�
  *         1. ʹ��GPIOCʱ��(APB2ʱ��)
  *         2. ����PC10Ϊ��������ģʽ(CNF=01,MODE=00)
  *         3. ����GPIO�ٶ�50MHz
  * @retval ��
  */
void Fire_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(GPIO_FIRE_RCC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;/*����GPIOģʽΪ��������*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_FIRE_PORT, &GPIO_InitStructure);		 /*���ÿ⺯������ʼ��GPIO*/	 	
}

/**
  * @brief  ���ּ���뱨������
  * @param  ��
  * @note   ʵʱ�����洫����״̬��
  *         - �ߵ�ƽ(�޻�)���رշ�����
  *         - �͵�ƽ(�л�)���򿪷���������
  *         ע���������͵�ƽ��ʾ��⵽����
  * @retval ��
  */
void Fire_Sensor_Init(void)
{
	if(GPIO_ReadInputDataBit(GPIO_FIRE_PORT, GPIO_FIRE_PIN))
	{
		BEEP_Ctrl(BEEP_Close);
	}
	else
	{
		BEEP_Ctrl(BEEP_Open);
	}
}

/**
  * @brief  ����״̬��ȡ
  * @param  ��
  * @note   ��ȡ��ǰ���ּ��״̬��
  *         - ����0: �޻��� (�������ߵ�ƽ)
  *         - ����1: ��⵽���� (�������͵�ƽ)
  *         ����ӿڣ�PC10
  * @retval uint8_t ����״ָ̬ʾ (1-�л�, 0-�޻�)
  */
uint8_t Fire_GetStatus(void)
{
    /* ���洫������⣨����͵�ƽ��ʾ�л��֣� */
    if(GPIO_ReadInputDataBit(GPIO_FIRE_PORT, GPIO_FIRE_PIN) == 0)
    {
        return 1;// ��⵽����
    }
    else
    {
        return 0;// �޻���
    }
}
