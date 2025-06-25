/****************************************************************************************************
 * @file        MQ2.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-22
 * @brief       MQ-2���崫��������ͷ�ļ����ṩ�ӿڶ�����Ӳ����������
 * @details     ��ͷ�ļ�����MQ-2���崫���������Ӳ���ӿں͹��ܺ���ԭ�ͣ�
 *              - ADCģ����GPIO�ӿں궨��
 *              - ������ԭʼ���ݶ�ȡ�ӿ�
 *              - ����Ũ��ת���ӿ�
 *              - ����״̬���ӿ�
 * @note        ͨ���궨��MQ2_PPM_ALARM_THRESHOLD�����ñ�����ֵ��Ĭ��10.00PPM��
 ****************************************************************************************************
 */
#ifndef __MQ2_H
#define	__MQ2_H
#include "stm32f10x.h"
#include "Delay.h"
#include "math.h"
/*************** ADC���ֺ궨�� ****************/
#define    	ADCx							ADC1
#define    	ADC_CLK							RCC_APB2Periph_ADC1
/************************************************/

/*************** MQ-2 GPIO�궨�� ****************/
#define 	MQ2_READ_TIMES					10  //MQ-2������ADCѭ����ȡ����
/* PB0/ADC12_IN8 */
#define		MQ2_AO_GPIO_CLK					RCC_APB2Periph_GPIOB
#define 	MQ2_AO_GPIO_PORT				GPIOB
#define 	MQ2_AO_GPIO_PIN					GPIO_Pin_0
#define   	ADC_CHANNEL               		ADC_Channel_8
#define 	MQ2_PPM_ALARM_THRESHOLD 		2.6f
/************************************************/
void ADCx_Init(void);
uint16_t ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime);

void MQ2_Init(void);
uint16_t MQ2_GetData(void);
float MQ2_GetData_PPM(void);
uint8_t MQ2_GetStatus(void);

#endif
