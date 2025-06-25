/****************************************************************************************************
 * @file        MQ2.h
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-22
 * @brief       MQ-2气体传感器驱动头文件：提供接口定义与硬件参数配置
 * @details     本头文件定义MQ-2气体传感器所需的硬件接口和功能函数原型：
 *              - ADC模块与GPIO接口宏定义
 *              - 传感器原始数据读取接口
 *              - 气体浓度转换接口
 *              - 报警状态检测接口
 * @note        通过宏定义MQ2_PPM_ALARM_THRESHOLD可配置报警阈值（默认10.00PPM）
 ****************************************************************************************************
 */
#ifndef __MQ2_H
#define	__MQ2_H
#include "stm32f10x.h"
#include "Delay.h"
#include "math.h"
/*************** ADC部分宏定义 ****************/
#define    	ADCx							ADC1
#define    	ADC_CLK							RCC_APB2Periph_ADC1
/************************************************/

/*************** MQ-2 GPIO宏定义 ****************/
#define 	MQ2_READ_TIMES					10  //MQ-2传感器ADC循环读取次数
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
