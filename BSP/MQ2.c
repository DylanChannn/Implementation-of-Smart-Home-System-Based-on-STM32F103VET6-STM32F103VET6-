/****************************************************************************************************
 * @file        MQ2.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-22
 * @brief       MQ-2���崫��������ģ�飺ʵ��ADC���ݲɼ���Ũ�ȼ���
 * @details     ���ļ�����MQ-2���崫�����������������ܣ�
 *              - ADCģ���ʼ������
 *              - ������ģ���źŲɼ����˲�����
 *              - ����Ũ��(PPM)ת���㷨
 *              - Ũ�ȳ��걨�����
 * @note        ʹ��ǰ������ϵͳʱ�ӣ�����GPIO�����ADCģ��֧��
 *              ������ֵΪMQ2_PPM_ALARM_THRESHOLD����ͷ�ļ����޸���ֵ�궨��
 ****************************************************************************************************
 */
#include "MQ2.h"
#include "gpio.h"

/**
  * @brief  ADC��ʼ������
  * @param  None
  * @retval None
  */
void ADCx_Init(void)
{
	/* ����ADCʱ�� */
	RCC_APB2PeriphClockCmd(ADC_CLK, ENABLE);
	
	/* ADCƵ�ʽ���6��Ƶ */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	/* ����ADC�ṹ�� */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//����ģʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//�����Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�������
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//����ת��
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//��ɨ��ģʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//��ͨ����
	ADC_Init(ADCx, &ADC_InitStructure);									//��ʼ��ADC1
	
	/* ����ADCx */
	ADC_Cmd(ADCx, ENABLE);
	
	/* ����ADCУ׼ */
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx) == SET);
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx) == SET);	
}

/**
  * @brief  ��ȡADCת�����ԭʼ����
  * @param  ADC_Channel��	ѡ��ADC�ɼ�ͨ��
  * @param  ADC_SampleTime��ѡ��ADC����ʱ��
  * @retval ADCת�����ԭʼֵ��12λ���ȣ�
  */
uint16_t ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime)
{
	/* ����ADCͨ�� */
	ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime);
	
	ADC_SoftwareStartConvCmd(ADCx, ENABLE); 						//�������ADCת��
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET); 			//��ȡADCת����ɱ�־λ
	return ADC_GetConversionValue(ADCx);
}

/**
  * @brief  MQ-2��������ʼ��
  * @param  None
  * @retval None
  */
void MQ2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd (MQ2_AO_GPIO_CLK, ENABLE );				// �� ADC IO�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = MQ2_AO_GPIO_PIN;					// ���� ADC IO ����ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					// ����Ϊģ������
	
	GPIO_Init(MQ2_AO_GPIO_PORT, &GPIO_InitStructure);				// ��ʼ�� ADC IO

	ADCx_Init();
}

/**
  * @brief  ��ȡMQ-2��ADCԭʼֵ
  * @param  None
  * @retval MQ-2��������ADCԭʼ����
  */
uint16_t MQ2_ADC_Read(void)
{
	/* ����ָ��ADC�Ĺ�����ͨ��������ʱ�� */
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}

/**
  * @brief  ��ȡ�����˲������MQ-2����
  * @param  None
  * @retval ��β���ƽ��ֵ
  */
uint16_t MQ2_GetData(void)
{
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		Delay_ms(5);
	}

	/* ����ƽ��ֵ */
	tempData /= MQ2_READ_TIMES;
	return tempData;
}

/**
  * @brief  ��ȡMQ-2����������PPMŨ��ֵ
  * @param  None
  * @retval �����������PPMֵ�������ͣ�
  */
float MQ2_GetData_PPM(void)
{
	float tempData = 0;

	for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		Delay_ms(5);
	}
	tempData /= MQ2_READ_TIMES;
	
	float Vol = (tempData*5/4096);
	float RS = (5-Vol)/(Vol*0.5);
	float R0=6.64;
	
	float ppm = pow(11.5428*R0/RS, 0.6549f);
	
	return ppm;
}

/**
  * @brief  �������Ũ���Ƿ񳬱�
  * @param  None
  * @retval ����״̬��
  * @arg 	0: Ũ������
  * @arg 	1: Ũ�ȳ��꣨PPM > 10.00��
  */
uint8_t MQ2_GetStatus(void)
{
    float current_ppm = MQ2_GetData_PPM();
    
    /* ���ñ�����ֵ */
    const float ALARM_THRESHOLD = 10.00f;
    
    if(current_ppm > ALARM_THRESHOLD)
    {
		/* �������� */
//		BEEP_Ctrl(BEEP_Open);
		return 1;
    }
    
    return 0;			//Ũ������
}
