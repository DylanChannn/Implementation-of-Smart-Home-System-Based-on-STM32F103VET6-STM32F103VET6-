#include "DHT11.h"

_DHT11Data DHT11_Data;

/***********************
�������ƣ�DHT11_Config
�������ܣ�DHT11��ʼ��
������ڣ���
�������ڣ���
�������ߣ�CY
����ʱ�䣺2021.04.20
�޸�ʱ�䣺2021.04.20
����˵����
	���ӽṹ��Ĭ��Ϊ�������ģʽ
***********************/
void DHT11_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	//ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	//ģʽ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

/***********************
�������ƣ�DHT11_GetData
�������ܣ�DHT11��ȡ����
������ڣ���
�������ڣ�
	Data	�ɼ��¶Ⱥ�ʪ��
�������ߣ�CY
����ʱ�䣺2021.04.20
�޸�ʱ�䣺2021.04.20
***********************/
uint8_t DHT11_GetData(_DHT11Data *Data)
{
	uint8_t DisPlay[20]="\0";
	uint32_t Error_timer = 0;
	uint8_t i=0,j=0;
	uint8_t Temp_buf[5] = "\0";

	//IO���ģʽ
	DHT11_OUT;

	//������ʼ�ź�
	DHT11_PUT(0);
	Delay_ms(20);
	DHT11_PUT(1);

	//IO����ģʽ
	DHT11_IN;
	Delay_us(13);

	//����Ӧ���ź�
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
	//����40λ�����ݰ�:���ֽڽ���
	for(i=0;i<5;i++)//�ֽ�
	{
		for(j=0;j<8;j++)//λ
		{
			//�ж�0/1
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

	//У������
	if(Temp_buf[4] == (Temp_buf[0]+Temp_buf[1]+Temp_buf[2]+Temp_buf[3]))
	{
		Data->hum = Temp_buf[0];
		if(Temp_buf[3] > 9)
				Data->tem = (-1)*(Temp_buf[2]+(Temp_buf[3]-128)/10.0);
		else
				Data->tem = Temp_buf[2]+Temp_buf[3]/10.0;
		
		sprintf((char *)DisPlay,"ʪ��%d",Data->hum);
		ST7789_ShowString(20,40,BLACK,BLUE,(char *)DisPlay);
		sprintf((char *)DisPlay,"�¶�%.1f",Data->tem);
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
