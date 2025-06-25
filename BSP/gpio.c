/****************************************************************************************************
 * @file        gpio.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-17
 * @brief       GPIO外设控制模块：LED和蜂鸣器硬件初始化与控制
 * @details     本文件实现：
 *              - 4路LED指示灯控制（PB8、PB9、PE0、PE1）
 *              - 蜂鸣器控制（PC8）
 *              引脚均配置为推挽输出模式，50MHz驱动能力
 * @note        初始化后默认关闭所有设备，提供独立控制接口
 *****************************************************************************************************
 */
#include "gpio.h"

/**
  * @brief  LED指示灯初始化
  * @param  无
  * @note   配置流程：
  *         1. 开启GPIOB和GPIOE的APB2时钟
  *         2. 配置PB8、PB9为推挽输出模式(CNF=00, MODE=11)
  *         3. 配置PE0、PE1为推挽输出模式
  *         4. 初始化关闭所有LED
  * @retval 无
  */
/* LED引脚初始化 */
void LED_Init(void)
{
		RCC->APB2ENR |= (1<<3)|(1<<6);//打开GPIOB、GPIOE时钟
		
		/* 对应PB8和PB9两个引脚 */
		GPIOB->CRH &= ~(0xFF<<0);			//0-3和4-7bit都取1111，然后取反清零
		GPIOB->CRH |= (33<<0);				//0-3和4-7bit都取0011，配置为：推挽输出 -> 00, 50MHz -> 11
		
		/* 对应PE0和PE1两个引脚 */
		GPIOE->CRL &= ~(0xFF<<0);
		GPIOE->CRL |= (33<<0);
		
		LED1_Ctrl(LED_Close);
		LED2_Ctrl(LED_Close);
		LED3_Ctrl(LED_Close);
		LED4_Ctrl(LED_Close);
}

/**
  * @brief  蜂鸣器初始化
  * @param  无
  * @note   配置流程：
  *         1. 开启GPIOC的APB2时钟
  *         2. 配置PC8为推挽输出模式(CNF=00, MODE=11)
  *         3. 初始化关闭蜂鸣器
  * @retval 无
  */
void BEEP_Init(void)
{
		RCC->APB2ENR |= (1<<4);				//打开GPIOC时钟
		
		GPIOC->CRH &= ~(0xF<<0);			//清零
		GPIOC->CRH |= (3<<0);				//配置为推挽、50MHz
	
		BEEP_Ctrl(BEEP_Close);
}
