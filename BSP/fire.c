/****************************************************************************************************
 * @file        fire.c
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-22
 * @brief       火灾检测模块：火焰传感器初始化与状态检测
 * @details     本文件实现：
 *              - 火焰传感器硬件初始化（浮空输入模式）
 *              - 蜂鸣器实时警报控制（检测到火焰时触发）
 *              - 火灾状态获取接口
 *              硬件连接：
 *              - 火焰传感器：PC10引脚（低电平有效）
 * @note        使用主动检测方式，传感器低电平表示检测到火焰，可与蜂鸣器联动实现声光报警
 *****************************************************************************************************
 */
#include "gpio.h"
#include "fire.h"

/**
  * @brief  火焰传感器硬件初始化
  * @param  无
  * @note   配置流程：
  *         1. 使能GPIOC时钟(APB2时钟)
  *         2. 配置PC10为浮空输入模式(CNF=01,MODE=00)
  *         3. 设置GPIO速度50MHz
  * @retval 无
  */
void Fire_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(GPIO_FIRE_RCC, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;/*设置GPIO模式为浮空输入*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_FIRE_PORT, &GPIO_InitStructure);		 /*调用库函数，初始化GPIO*/	 	
}

/**
  * @brief  火灾监测与报警控制
  * @param  无
  * @note   实时检测火焰传感器状态：
  *         - 高电平(无火)：关闭蜂鸣器
  *         - 低电平(有火)：打开蜂鸣器警报
  *         注：传感器低电平表示检测到火灾
  * @retval 无
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
  * @brief  火灾状态获取
  * @param  无
  * @note   获取当前火灾检测状态：
  *         - 返回0: 无火灾 (传感器高电平)
  *         - 返回1: 检测到火灾 (传感器低电平)
  *         物理接口：PC10
  * @retval uint8_t 火灾状态指示 (1-有火, 0-无火)
  */
uint8_t Fire_GetStatus(void)
{
    /* 火焰传感器检测（假设低电平表示有火灾） */
    if(GPIO_ReadInputDataBit(GPIO_FIRE_PORT, GPIO_FIRE_PIN) == 0)
    {
        return 1;// 检测到火灾
    }
    else
    {
        return 0;// 无火灾
    }
}
