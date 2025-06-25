/****************************************************************************************************
 * @file        key.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-18
 * @brief       按键控制模块：实现四个独立按键的初始化和扫描功能
 * @details     本文件包含四个按键(KEY1-KEY4)的初始化和状态扫描函数：
 *              - KEY1: PC2浮空输入，按下为低电平
 *              - KEY2: PC3浮空输入，按下为高电平
 *              - KEY3: PA0上拉输入，按下为低电平
 *              - KEY4: PA1下拉输入，按下为高电平
 *              使用状态机实现按键消抖，消抖时间200us
 * @note        每个按键使用独立的状态机处理，提供稳定的按键事件检测
 ****************************************************************************************************
 */
#include "key.h"
#include "Delay.h"

/**
  * @brief  KEY1(PC2)初始化：配置为浮空输入模式，点击低电平
  * @param  无
  * @note   - 使能GPIOC时钟(APB2时钟控制)
  *         - PC2配置为浮空输入模式(CNF=01,MODE=00)
  * @retval 无
  */
void Key1_Init(void)
{
    RCC->APB2ENR |= (1<<4);     			// 打开GPIOC时钟 (对应APBENR寄存器的bit4)
    
    GPIOC->CRL &= ~(0xF<<8);    			// bit8-bit11取1111，清除PC2配置
    GPIOC->CRL |= (0x4<<8);     			// 浮空输入CNF = 01，输入模式下MODE配置为00，0b0100=0x4
}

/**
  * @brief  KEY1(PC2)状态扫描
  * @param  无
  * @note   使用状态机(Key_Idle/Key_Down/Key_Up)实现：
  *         - 检测PC2引脚低电平触发按下状态
  *         - 消抖时间200us
  * @retval uint8_t 
  *         - 1: 按键有效触发(按下并释放)
  *         - 0: 无按键事件
  */
uint8_t Key1_Scan(void)
{
    static _Key_State Key1_State = Key_Idle;
    static uint16_t count = 0;
    uint8_t return_val = 0;
    
    switch(Key1_State)
    {
        case Key_Idle:
            if(!(GPIOC->IDR & (1<<2)))  	// IDR为独立数据寄存器，此处专门检查PC2的2引脚对应的电平，检测低电平(按下)
                Key1_State = Key_Down;
            break;
            
        case Key_Down:
            if(GPIOC->IDR & (1<<2))    		// 检测高电平(释放)
                Key1_State = Key_Up;
            Delay_us(1);
            count++;
            break;
            
        case Key_Up:
            if(count >= 200)   				// 消抖判定时间
                return_val = 1;
            count = 0;
            Key1_State = Key_Idle;
            break;
    }
    return return_val;
}

/**
  * @brief  KEY2(PC3)初始化：配置为浮空输入模式，点击高电平
  * @param  无
  * @note   - 使能GPIOC时钟(APB2时钟控制)
  *         - PC3配置为浮空输入模式(CNF=01,MODE=00)
  * @retval 无
  */
void Key2_Init(void)
{
    RCC->APB2ENR |= (1<<4);     			 // 打开GPIOC时钟
    
    GPIOC->CRL &= ~(0xF<<12);   			 // bit12-15取1111，清除PC3配置
    GPIOC->CRL |= (0x4<<12);    			 // 浮空输入CNF=01. 输入模式下MODE配置为00，0b0100=0x4
}

/**
  * @brief  KEY2(PC3)状态扫描
  * @param  无
  * @note   使用状态机(Key_Idle/Key_Down/Key_Up)实现：
  *         - 检测PC3引脚高电平触发按下状态
  *         - 消抖时间200us
  * @retval uint8_t 
  *         - 1: 按键有效触发(按下并释放)
  *         - 0: 无按键事件
  */
uint8_t Key2_Scan(void)
{
    static _Key_State Key2_State = Key_Idle;
    static uint16_t count = 0;
    uint8_t return_val = 0;
    
    switch(Key2_State)
    {
        case Key_Idle:
            if(GPIOC->IDR & (1<<3))  		// IDR为独立数据寄存器，此处专门检查PC3的3引脚对应的电平，检测高电平(按下)
                Key2_State = Key_Down;
            break;
            
        case Key_Down:
            if(!(GPIOC->IDR & (1<<3)))  	// 检测低电平(释放)
                Key2_State = Key_Up;
            Delay_us(1);
            count++;
            break;
            
        case Key_Up:
            if(count >= 200)
                return_val = 1;
            count = 0;
            Key2_State = Key_Idle;
            break;
    }
    return return_val;
}

/**
  * @brief  	KEY3(PA0)初始化：配置为上拉输入模式，点击低电平
  * @param  	无
  * @note   	- 使能GPIOA时钟(APB2时钟控制)
  *         	- PA0配置为上拉输入模式(CNF=10,MODE=00)
  *         	- 启用内部上拉电阻(ODR=1)
  * @retval		无
  * @autohor	CY
  */
void Key3_Init(void) 
{
    RCC->APB2ENR |= (1<<2);      			// 打开GPIOA时钟
    GPIOA->CRL &= ~(0xF<<0);   		  		// bit0-3取1111，清除PA0配置
    GPIOA->CRL |= (0x8<<0);      			// 上拉输入CNF = 10， MODE = 00，0b1000 = 0x8
    GPIOA->ODR |= (1<<0);        			// 启用 PA0 的内部上拉电阻，确保按键未按下时：引脚被拉高至稳定高电平
}

/**
  * @brief  	KEY3(PA0)状态扫描
  * @param  	无
  * @note  		使用状态机(Key_Idle/Key_Down/Key_Up)实现：
  *         	- 检测PA0引脚低电平触发按下状态
  *         	- 消抖时间200us
  *         	- 使用双取反(!!)确保电平识别
  * @retval 	uint8_t 
  *         	- 1: 按键有效触发(按下并释放)
  *         	- 0: 无按键事件
  * @autohor	CY
  */
uint8_t Key3_Scan(void)
{
	static _Key_State Key3_State = Key_Idle;
	static uint16_t count = 0;
	uint8_t return_val = 0;
	
	switch(Key3_State)
	{
		case Key_Idle:
		{
			if((!!(GPIOA->IDR & (1<<0))) == 0)
				Key3_State = Key_Down;
			break;
		}
		case Key_Down:
		{
			if((!!(GPIOA->IDR & (1<<0))) == 1)
				Key3_State = Key_Up;
			Delay_us(1);
			count++;
			break;
		}
		case Key_Up:
		{
			if(count >= 200)
				return_val = 1;
			count = 0;
			Key3_State = Key_Idle;
			break;
		}
	}
	return return_val;
}

/**
  * @brief  KEY4(PA1)初始化：配置为下拉输入模式，点击高电平
  * @param  无
  * @note   - 使能GPIOA时钟(APB2时钟控制)
  *         - PA1配置为上拉/下拉模式(CNF=10,MODE=00)
  *         - 启用内部下拉电阻(ODR=0)
  * @retval 无
  */
void Key4_Init(void)
{
    RCC->APB2ENR |= (1<<2);      		// 打开GPIOA时钟
    
    GPIOA->CRL &= ~(0xF<<4);    	 	// bit4-7取1111，清除PA1配置
    GPIOA->CRL |= (0x8<<4);      		// 上拉/下拉模式CNF = 10， MODE = 00，0b1000 = 0x8
    GPIOA->ODR &= ~(1<<1);       		// 启用 PA0 的内部下拉电阻，确保按键未按下时：引脚被拉低至稳定低电平
}

/**
  * @brief  KEY4(PA1)状态扫描
  * @param  无
  * @note   使用状态机(Key_Idle/Key_Down/Key_Up)实现：
  *         - 检测PA1引脚高电平触发按下状态
  *         - 消抖时间200us
  * @retval uint8_t 
  *         - 1: 按键有效触发(按下并释放)
  *         - 0: 无按键事件
  */
uint8_t Key4_Scan(void)
{
    static _Key_State Key4_State = Key_Idle;
    static uint16_t count = 0;
    uint8_t return_val = 0;
    
    switch(Key4_State)
    {
        case Key_Idle:
            if(GPIOA->IDR & (1<<1))  	 // 检测高电平(按下)
                Key4_State = Key_Down;
            break;
            
        case Key_Down:
            if(!(GPIOA->IDR & (1<<1)))   // 检测低电平(释放)
                Key4_State = Key_Up;
            Delay_us(1);
            count++;
            break;
            
        case Key_Up:
            if(count >= 200)
                return_val = 1;
            count = 0;
            Key4_State = Key_Idle;
            break;
    }
    return return_val;
}
