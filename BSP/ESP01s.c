#include "esp01s.h"
#include "usart1.h"

_ESP8266_STR Wifi_Data = {"\0"};

void USART2_IRQHandler(void)
{
	uint8_t Temp_data = 0;
	if(USART_GetITStatus(WIFI_USART,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(WIFI_USART,USART_IT_RXNE);
		Wifi_Data.Rx_buf[Wifi_Data.Rxbuf_Count++] = WIFI_USART->DR;
			
		USART1->DR = WIFI_USART->DR;
	}
	if(USART_GetITStatus(WIFI_USART,USART_IT_IDLE))
	{
		Temp_data = WIFI_USART->SR;
		Temp_data = WIFI_USART->DR;
		Temp_data = Temp_data;
		
		Wifi_Data.Flag_RxOver = 1;
	}
}


/************************
函数名称：ESP8266_Config
函数作用：ESP8266 串口初始化
函数入口：无
函数出口：无
函数作者：CY
创建时间：2021.01.03
修改时间：2021.01.03
补充说明：
	ESP_EN PE6
************************/
uint8_t ESP8266_Config(void)
{
	uint8_t Ret = 0;
	char EspConfig_Buf[128] = "\0";
	ESP8266_PORTConfig(115200);
	
	Delay_ms(2000);
	printf("WIFI Start\r\n");
	CloseTransmission();
	Delay_ms(100);
	ESP8266_SendStr("AT\r\n");			//测试
	Delay_ms(100);
	ESP8266_SendStr("AT\r\n");			//测试
	Ret=FindStr((char *)Wifi_Data.Rx_buf,"OK",2000);
	if(Ret == 0)
	{
		printf("ESP8266不存在\r\n");
		return 0;
	}
	Delay_ms(100);
	ESP8266_SendStr("AT+CWMODE=1\r\n");
	Ret=FindStr((char *)Wifi_Data.Rx_buf,"OK",200);
	if(Ret==0)
	{
		printf("Station模式设置失败\r\n");
		return 0;
	}
	Delay_ms(100);
	ESP8266_SendStr("AT+CIPSTATUS\r\n");
	Ret = FindStr((char *)Wifi_Data.Rx_buf,"STATUS:2",200);
	if(Ret != 0)
	{
		printf("WIFI连接成功\r\n");
		return 1;
	}
	
	memset(EspConfig_Buf,0,128);
	Delay_ms(100);
	sprintf(EspConfig_Buf,"AT+CWJAP=\"%s\",\"%s\"\r\n ",WIFI_SSID,WIFI_PASSWORD);
	ESP8266_SendStr(EspConfig_Buf);
	Ret = FindStr((char *)Wifi_Data.Rx_buf,"OK",9000);
	if(Ret != 0)
	{
		printf("WIFI连接成功\r\n");
		return 1;
	}
	else
	{
		printf("WIFI连接失败\r\n");
		return 0;
	}	
}

/************************
函数名称：ESP8266_USARTConfig
函数作用：ESP8266 串口初始化
函数入口：无
函数出口：无
函数作者：CY
创建时间：2021.01.03
修改时间：2021.01.03
补充说明：
	USART2
************************/
void ESP8266_PORTConfig(uint32_t brr)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	USART_InitTypeDef USART_InitStructure = {0};
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//GPIO初始化：PA2 PA3
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//USART2配置
	USART_InitStructure.USART_BaudRate = brr;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(WIFI_USART,&USART_InitStructure);
	//中断
	USART_ITConfig(WIFI_USART,USART_IT_RXNE,ENABLE);
	USART_ITConfig(WIFI_USART,USART_IT_IDLE,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(WIFI_USART,ENABLE);
}

/************************
函数名称：ESP8266_SendByte
函数作用：USART2发送字节
函数入口：byte 发送数据
函数出口：无
函数作者：CY
创建时间：2021.01.03
修改时间：2021.01.03
************************/
void ESP8266_SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC) == RESET);
	USART_SendData(WIFI_USART,byte);
}

/************************
函数名称：ESP8266_SendStr
函数作用：ESP8266发送字符串
函数入口：str 发送数据
函数出口：无
函数作者：CY
创建时间：2021.01.03
修改时间：2021.01.03
************************/
void ESP8266_SendStr(char *str)
{
	Clear_ESP8266Buf();
	while(*str != '\0')
	{
		ESP8266_SendByte(*str);
		str++;
	}
}
/************************
函数名称：ESP8266_SendStr
函数作用：ESP8266发送缓冲区
函数入口：
	buf 发送缓冲区
	len 发送缓冲区长度
函数出口：无
函数作者：CY
创建时间：2021.01.03
修改时间：2021.01.03
************************/
void ESP8266_SendBuf(uint8_t *buf,uint16_t len)
{
	while(len--)
		ESP8266_SendByte(*buf++);
}

/************************
函数名称：ESP8266_RecvByte
函数作用：USART2接收字节
函数入口：无
函数出口：byte 接收数据
函数作者：CY
创建时间：2021.01.03
修改时间：2021.01.03
************************/
uint8_t ESP8266_RecvByte(void)
{
	uint8_t byte;
	while(USART_GetFlagStatus(WIFI_USART,USART_FLAG_RXNE) == RESET);
	byte =  USART_ReceiveData(WIFI_USART);
	return byte;
}

/****************************
函数名称：CloseTransmission
函数作用：退出透传
函数参数：无
函数返回值：无
函数作者：CY
创建时间：2020.09.21
修改时间：2020.09.21
****************************/
void CloseTransmission(void)
{
	ESP8266_SendStr("+++"); Delay_ms(50);
	ESP8266_SendStr("+++"); Delay_ms(50);
}

/****************************
函数名称：OpenTransmission
函数作用：打开透传
函数参数：无
函数返回值：
	非0 ESP8266正常
	0 	ESP8266有问题 
函数作者：CY
创建时间：2020.09.21
修改时间：2020.09.21
****************************/
uint8_t OpenTransmission(void)
{
	//设置透传模式
	uint8_t cnt=2;
	while(cnt--)
	{
		memset(Wifi_Data.Rx_buf,0,512);    
		ESP8266_SendStr("AT+CIPMODE=1\r\n");  
		if(FindStr((char*)Wifi_Data.Rx_buf,"OK",200)!=0)
		{	
			return 1;
		}			
	}
	return 0;
}

/****************************
函数名称：FindStr
函数作用：查找字符串中是否包含另一个字符串
函数参数：
		dest：		待查找目标字符串
		src：			待查找内容
		retry_ms:	查询超时时间
函数返回值：非0为查找成功,0为失败
函数作者：CY
创建时间：2020.09.21
修改时间：2021.04.29
****************************/
void Clear_ESP8266Buf(void)
{
	Wifi_Data.Flag_RxOver = 0;
	Wifi_Data.Rxbuf_Count = 0;
	memset(Wifi_Data.Rx_buf,0,512);
}
uint8_t FindStr(char* dest,char* src,int retry_nms)
{		
	while(retry_nms--)
	{
		if(Wifi_Data.Flag_RxOver == 1)//数据接收完成
		{
			if(strstr(dest,src) != NULL)//得到数据直接结束
			{
				Clear_ESP8266Buf();
				return 1;
			}
			else//得到一帧数据，没超时，但数据不对，继续接收数据
				Wifi_Data.Flag_RxOver = 0;
		}
		Delay_ms(1);
	}

	Clear_ESP8266Buf();
	return 0;
}
