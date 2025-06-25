/****************************************************************************************************
 * @file        MQ2.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-22
 * @brief       MQ-2气体传感器驱动模块：实现ADC数据采集与浓度计算
 * @details     本文件包含MQ-2气体传感器的完整驱动功能：
 *              - ADC模块初始化配置
 *              - 传感器模拟信号采集与滤波处理
 *              - 气体浓度(PPM)转换算法
 *              - 浓度超标报警检测
 * @note        使用前需配置系统时钟，依赖GPIO外设和ADC模块支持
 *              报警阈值为MQ2_PPM_ALARM_THRESHOLD，在头文件中修改阈值宏定义
 ****************************************************************************************************
 */
#include "MQ2.h"
#include "gpio.h"

/**
  * @brief  ADC初始化配置
  * @param  None
  * @retval None
  */
void ADCx_Init(void)
{
	/* 开启ADC时钟 */
	RCC_APB2PeriphClockCmd(ADC_CLK, ENABLE);
	
	/* ADC频率进行6分频 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	/* 配置ADC结构体 */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//单次转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//非扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//总通道数
	ADC_Init(ADCx, &ADC_InitStructure);									//初始化ADC1
	
	/* 开启ADCx */
	ADC_Cmd(ADCx, ENABLE);
	
	/* 进行ADC校准 */
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx) == SET);
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx) == SET);	
}

/**
  * @brief  获取ADC转换后的原始数据
  * @param  ADC_Channel：	选择ADC采集通道
  * @param  ADC_SampleTime：选择ADC采样时间
  * @retval ADC转换后的原始值（12位精度）
  */
uint16_t ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime)
{
	/* 配置ADC通道 */
	ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime);
	
	ADC_SoftwareStartConvCmd(ADCx, ENABLE); 						//软件触发ADC转换
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET); 			//读取ADC转换完成标志位
	return ADC_GetConversionValue(ADCx);
}

/**
  * @brief  MQ-2传感器初始化
  * @param  None
  * @retval None
  */
void MQ2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd (MQ2_AO_GPIO_CLK, ENABLE );				// 打开 ADC IO端口时钟
	GPIO_InitStructure.GPIO_Pin = MQ2_AO_GPIO_PIN;					// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					// 设置为模拟输入
	
	GPIO_Init(MQ2_AO_GPIO_PORT, &GPIO_InitStructure);				// 初始化 ADC IO

	ADCx_Init();
}

/**
  * @brief  读取MQ-2的ADC原始值
  * @param  None
  * @retval MQ-2传感器的ADC原始读数
  */
uint16_t MQ2_ADC_Read(void)
{
	/* 设置指定ADC的规则组通道，采样时间 */
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}

/**
  * @brief  获取经过滤波处理的MQ-2数据
  * @param  None
  * @retval 多次采样平均值
  */
uint16_t MQ2_GetData(void)
{
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		Delay_ms(5);
	}

	/* 计算平均值 */
	tempData /= MQ2_READ_TIMES;
	return tempData;
}

/**
  * @brief  获取MQ-2传感器检测的PPM浓度值
  * @param  None
  * @retval 计算出的气体PPM值（浮点型）
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
  * @brief  检测气体浓度是否超标
  * @param  None
  * @retval 报警状态：
  * @arg 	0: 浓度正常
  * @arg 	1: 浓度超标（PPM > 10.00）
  */
uint8_t MQ2_GetStatus(void)
{
    float current_ppm = MQ2_GetData_PPM();
    
    /* 设置报警阈值 */
    const float ALARM_THRESHOLD = 10.00f;
    
    if(current_ppm > ALARM_THRESHOLD)
    {
		/* 触发报警 */
//		BEEP_Ctrl(BEEP_Open);
		return 1;
    }
    
    return 0;			//浓度正常
}
