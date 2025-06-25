#include "Delay.h"
#include "stm32f10x.h"

volatile uint32_t systemTicks = 0;	// 系统滴答计数器

void Delay_Init(void)
{
    /* 配置SysTick为1ms中断 */
    if (SysTick_Config(SystemCoreClock / 1000)) 
	{
        while (1);  // 如果配置失败则停在这里
    }
    
    /* 设置SysTick中断优先级为最低(可选) */
    NVIC_SetPriority(SysTick_IRQn, 3);
}

uint16_t LED_Run[2] = {0,500};
uint16_t DHT11_Run[2] = {0,2000};
void SysTick_Handler(void)
{
	LED_Run[0]++;
	DHT11_Run[0]++;
	systemTicks++;	// 系统滴答计数器增加
}

// 添加获取滴答数的函数
uint32_t Delay_GetTickCount(void)
{
    return systemTicks;
}

void delay_1us(void)
{
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
}

void Delay_us(uint32_t nus)
{
	while(nus--)
		delay_1us();
}

void Delay_ms(uint32_t nms)
{
    while(nms--)
    {
        Delay_us(1000); // 每次延时1毫秒
    }
}
