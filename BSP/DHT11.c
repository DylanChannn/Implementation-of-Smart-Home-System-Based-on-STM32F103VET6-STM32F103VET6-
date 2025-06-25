#include "DHT11.h"

_DHT11Data DHT11_Data;

/***********************
函数名称：DHT11_Config
函数功能：DHT11初始化
函数入口：无
函数出口：无
函数作者：CY
创建时间：2021.04.20
修改时间：2021.04.20
补充说明：
	主从结构，默认为主机输出模式
***********************/
void DHT11_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	//模式配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

/***********************
函数名称：DHT11_GetData
函数功能：DHT11获取数据
函数入口：无
函数出口：
	Data	采集温度和湿度
函数作者：CY
创建时间：2021.04.20
修改时间：2021.04.20
***********************/
uint8_t DHT11_GetData(_DHT11Data *Data)
{
	uint8_t DisPlay[20]="\0";
	uint32_t Error_timer = 0;
	uint8_t i=0,j=0;
	uint8_t Temp_buf[5] = "\0";

	//IO输出模式
	DHT11_OUT;

	//发送起始信号
	DHT11_PUT(0);
	Delay_ms(20);
	DHT11_PUT(1);

	//IO输入模式
	DHT11_IN;
	Delay_us(13);

	//接收应答信号
	while(!DHT11_GET)
	{
		Error_timer++;
		Delay_us(1);
		if(Error_timer >= 88)
			return 1;
	}
	Error_timer = 0;
	
	while(DHT11_GET)
	{
		Error_timer++;
		Delay_us(1);
		if(Error_timer >= 92)
			return 2;
	}
	//接收40位的数据包:按字节接收
	for(i=0;i<5;i++)//字节
	{
		for(j=0;j<8;j++)//位
		{
			//判断0/1
			Error_timer = 0;
			while(!DHT11_GET)
			{
				Error_timer++;
				Delay_us(1);
				if(Error_timer >= 58)
					return 3;
			}
			Error_timer = 0;
			Delay_us(30);
			
			Temp_buf[i] <<= 1;
			if(DHT11_GET)//1
			{
				Temp_buf[i] |= 1;
				while(DHT11_GET);
			}
		}
	}

	//校验数据
	if(Temp_buf[4] == (Temp_buf[0]+Temp_buf[1]+Temp_buf[2]+Temp_buf[3]))
	{
		Data->hum = Temp_buf[0];
		if(Temp_buf[3] > 9)
				Data->tem = (-1)*(Temp_buf[2]+(Temp_buf[3]-128)/10.0);
		else
				Data->tem = Temp_buf[2]+Temp_buf[3]/10.0;
		
		sprintf((char *)DisPlay,"湿度%d",Data->hum);
		ST7789_ShowString(20,40,BLACK,BLUE,(char *)DisPlay);
		sprintf((char *)DisPlay,"温度%.1f",Data->tem);
		ST7789_ShowString(20,60,BLACK,RED,(char *)DisPlay);
//		printf("Hum:%d\r\n",Data->hum);
//		printf("Tem:%f\r\n",Data->tem);
		return 0;
	}
	return 4;
}

void DHT11_RefreshData(void)
{
    DHT11_GetData(&DHT11_Data);
}
