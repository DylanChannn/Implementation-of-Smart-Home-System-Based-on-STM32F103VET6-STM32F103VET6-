#include "usart1.h"

_USART_Data USART1_Data;

void USART1_IRQHandler(void)
{
		if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET)
		{
				USART_ClearFlag(USART1,USART_FLAG_RXNE);
				USART1_Data.RxBuf[USART1_Data.count++]=USART1->DR;
		}
		if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE) != RESET)
		{
				USART1->SR;
				USART1->DR;
				USART1_Data.flag = 1;
		}
}

//
//@简介：USART1初始化
//PA9 -USART1_TX-复用推挽输出
//PA10-USART1_RX-浮空输入
//
void USART1_Init(uint32_t BRR)
{
		//时钟控制
		RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN,ENABLE);
		
		//模式配置
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		//USART1配置
		USART_InitTypeDef USART1_Struct = {0};
		USART1_Struct.USART_BaudRate = BRR;
		USART1_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART1_Struct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
		USART1_Struct.USART_Parity = USART_Parity_No;
		USART1_Struct.USART_StopBits = USART_StopBits_1;
		USART1_Struct.USART_WordLength = USART_WordLength_8b;
		USART_Init(USART1,&USART1_Struct);
		
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
		USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
		NVIC_SetPriority(USART1_IRQn,0);
		NVIC_EnableIRQ(USART1_IRQn);
		
		USART_Cmd(USART1,ENABLE);
}

void USART1_SendByte(uint8_t byte)
{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
		USART1->DR = byte;
}

void USART1_SendBuf(uint8_t* buf,uint16_t len)
{
		for(uint16_t i=0;i<len;i++)
			USART1_SendByte(buf[i]);
}

void USART1_SendStr(char* str)
{
		while(*str != '\0')
			USART1_SendByte(*str++);
}

int fputc(int c, FILE * stream)
{
		USART1_SendByte(c);
		return c;
}

//
//@简介：计算和校验
//@参数1：buf 数组第0个成员的地址
//@参数2：len 计算和校验的长度
//
uint8_t Get_Add(uint8_t * buf,uint8_t len)
{
	uint8_t sum = 0;
	for(uint8_t i=0;i<len;i++)
		sum += buf[i];
	return sum;
}
