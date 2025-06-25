#include "st7789.h"

/******************************
�������ƣ�ST7789_PortConfig
�������ܣ�ST7789�ܽų�ʼ��
������ڣ���
�������ڣ���
******************************/
void LCD_Config(void)
{
		ST7789_PortConfig();	//�ײ�ӿڳ�ʼ��
		ST7789_Config();		//LCD������ʼ��-ST7789
		ST7789_Clear(BLACK);	//��Ļ����--��ɫ
}

/******************************
�������ƣ�ST7789_PortConfig
�������ܣ�ST7789�ܽų�ʼ��
������ڣ���
�������ڣ���
******************************/
void ST7789_PortConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};
	
	//ʱ��ʹ��
	RCC_APB2PeriphClockCmd(LCD_BackCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_DCCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_CSCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_CLKCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_DataCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_RestCLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //�ٶ�50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Pin = LCD_BackPin;	 
	GPIO_Init(LCD_BackPort,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_DCPin;	 
	GPIO_Init(LCD_DCPort,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_CSPin;	 
	GPIO_Init(LCD_CSPort,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_CLKPin;	 
	GPIO_Init(LCD_CLKPort,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_DataPin;	 
	GPIO_Init(LCD_DataPort,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_RestPin;	 
	GPIO_Init(LCD_RestPort,&GPIO_InitStructure);
	
	LCDBack_CTRL(0);	//�����ص�
	LCDCLK_CTRL(0);		//SP0 ʱ���߿���״̬�͵�ƽ
	LCDRest_CTRL(1);	//��λ���ߣ���Ļ������ 
	LCDCS_CTRL(1);		//CS��ʼ���ߵ�ƽ������SPI�豸�ӻ���ѡ��
}

/******************************
�������ƣ�ST7789_SoftSP0
�������ܣ�ģʽSPI SP0
������ڣ���
�������ڣ���
	����״̬CPOL=0 �͵�ƽ
	ʱ����λCPHA=0 ʱ��ǰ�ط�������
	��λ�ȷ�

	0XAA
	0X80 = 10000000
	
	10101010	IDR
	10000000

	<<֮��
	01010100
	10000000
******************************/
void ST7789_SoftSP0(uint8_t data) 
{	
	uint8_t i = 0;
	for(i=0;i<8;i++)		//һ�η��Ͱ�λ����
	{			  
		LCDCLK_CTRL(0);
		
		if(data & 0x80)		//�ж����λ��0����1
			 LCDData_CTRL(1);
		else
			 LCDData_CTRL(0);
		
		LCDCLK_CTRL(1);
		
		data <<= 1;		//��λ�ȷ�
	}	
}

/******************************
�������ƣ�ST7789_SendByte
�������ܣ������ֽ�
������ڣ���
�������ڣ���
******************************/
void ST7789_SendByte(_ST7789_Type Type,uint8_t data)
{
	LCDCS_CTRL(0);			//CS����--���ݿ�ʼ����
	LCDDC_CTRL(Type);//ѡ����������cmd����data
	ST7789_SoftSP0(data);
	LCDCS_CTRL(1);
}

/******************************
�������ƣ�ST7789_SendByte
�������ܣ����Ͱ���
������ڣ���
�������ڣ���
******************************/
void ST7789_SendHarfWord(uint16_t data)
{
	LCDDC_CTRL(1);//ѡ����������cmd����data
	LCDCS_CTRL(0);			//CS����--���ݿ�ʼ����
	ST7789_SoftSP0(data>>8);
	ST7789_SoftSP0(data);
	LCDCS_CTRL(1);
}

/******************************
�������ƣ�ST7789_Config
�������ܣ�ST7789������ʼ��
������ڣ���
�������ڣ���
******************************/
void ST7789_Config(void)
{
	//����д��֮ǰӲ����λ
	LCDRest_CTRL(0);
	Delay_ms(100);
	LCDRest_CTRL(1);
	Delay_ms(100);
	//�򿪱���
	LCDBack_CTRL(1);
	Delay_ms(100);
	
	//************* Start Initial Sequence **********//
	ST7789_SendByte(type_cmd,0x11); 				//Sleep out 
	Delay_ms(120);           							  //Delay 120ms 
	//************* Start Initial Sequence **********// 
	ST7789_SendByte(type_cmd,0x36);//9.1.28
	if(USE_HORIZONTAL==0)
		ST7789_SendByte(type_data,0x00);
	
	else if(USE_HORIZONTAL==1)
		ST7789_SendByte(type_data,0xC0);
	
	else if(USE_HORIZONTAL==2)
		ST7789_SendByte(type_data,0x70);
	
	else ST7789_SendByte(type_data,0xA0);

	ST7789_SendByte(type_cmd,0x3A);     
	ST7789_SendByte(type_data,0x05);   

	ST7789_SendByte(type_cmd,0xB2);     
	ST7789_SendByte(type_data,0x0C);   
	ST7789_SendByte(type_data,0x0C);   
	ST7789_SendByte(type_data,0x00);   
	ST7789_SendByte(type_data,0x33);   
	ST7789_SendByte(type_data,0x33);   

	ST7789_SendByte(type_cmd,0xB7);     
	ST7789_SendByte(type_data,0x35);   

	ST7789_SendByte(type_cmd,0xBB);     
	ST7789_SendByte(type_data,0x19); 

	ST7789_SendByte(type_cmd,0xC0);     
	ST7789_SendByte(type_data,0x2C);   

	ST7789_SendByte(type_cmd,0xC2);     
	ST7789_SendByte(type_data,0x01);   

	ST7789_SendByte(type_cmd,0xC3);     
	ST7789_SendByte(type_data,0x12);   

	ST7789_SendByte(type_cmd,0xC4);     
	ST7789_SendByte(type_data,0x20);   //VDV, 0x20:0v

	ST7789_SendByte(type_cmd,0xC6);     
	ST7789_SendByte(type_data,0x0F);   //0x13:60Hz   

	ST7789_SendByte(type_cmd,0xD0);     
	ST7789_SendByte(type_data,0xA4);   
	ST7789_SendByte(type_data,0xA1);   

	//	ST7789_SendByte(type_cmd,0xD6);     
	//	ST7789_SendByte(type_data,0xA1);   //sleep in��gate���ΪGND

	ST7789_SendByte(type_cmd,0xE0);     
	ST7789_SendByte(type_data,0xD0);   
	ST7789_SendByte(type_data,0x04);   
	ST7789_SendByte(type_data,0x0D);   
	ST7789_SendByte(type_data,0x11);   
	ST7789_SendByte(type_data,0x13);   
	ST7789_SendByte(type_data,0x2B);   
	ST7789_SendByte(type_data,0x3F);   
	ST7789_SendByte(type_data,0x54);   
	ST7789_SendByte(type_data,0x4C);   
	ST7789_SendByte(type_data,0x18);   
	ST7789_SendByte(type_data,0x0D);   
	ST7789_SendByte(type_data,0x0B);   
	ST7789_SendByte(type_data,0x1F);   
	ST7789_SendByte(type_data,0x23);   

	ST7789_SendByte(type_cmd,0xE1);     
	ST7789_SendByte(type_data,0xD0);   
	ST7789_SendByte(type_data,0x04);   
	ST7789_SendByte(type_data,0x0C);   
	ST7789_SendByte(type_data,0x11);   
	ST7789_SendByte(type_data,0x13);   
	ST7789_SendByte(type_data,0x2C);   
	ST7789_SendByte(type_data,0x3F);   
	ST7789_SendByte(type_data,0x44);   
	ST7789_SendByte(type_data,0x51);   
	ST7789_SendByte(type_data,0x2F);   
	ST7789_SendByte(type_data,0x1F);   
	ST7789_SendByte(type_data,0x1F);   
	ST7789_SendByte(type_data,0x20);   
	ST7789_SendByte(type_data,0x23);
	
	//	ST7789_SendByte(type_cmd,0xE4);     
	//	ST7789_SendByte(type_data,0x1D);   //ʹ��240��gate  (N+1)*8
	//	ST7789_SendByte(type_data,0x00);   //�趨gate���λ��
	//	ST7789_SendByte(type_data,0x00);   //��gateû������ʱ��bit4(TMG)��Ϊ0
	ST7789_SendByte(type_cmd,0x21);     
	ST7789_SendByte(type_cmd,0x29);
}

/******************************
�������ƣ�ST7789_SetAddr
�������ܣ�ST7789��Χ����
������ڣ���
�������ڣ���
******************************/
void ST7789_SetAddr(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
	if(USE_HORIZONTAL==0)
	{
		ST7789_SendByte(type_cmd,0x2a);//�е�ַ����
		ST7789_SendHarfWord(x_start+52);
		ST7789_SendHarfWord(x_end+52);
		ST7789_SendByte(type_cmd,0x2b);//�е�ַ����
		ST7789_SendHarfWord(y_start+40);
		ST7789_SendHarfWord(y_end+40);
		ST7789_SendByte(type_cmd,0x2c);//������д
	}
	else if(USE_HORIZONTAL==1)
	{
		ST7789_SendByte(type_cmd,0x2a);//�е�ַ����
		ST7789_SendHarfWord(x_start+53);
		ST7789_SendHarfWord(x_end+53);
		ST7789_SendByte(type_cmd,0x2b);//�е�ַ����
		ST7789_SendHarfWord(y_start+40);
		ST7789_SendHarfWord(y_end+40);
		ST7789_SendByte(type_cmd,0x2c);//������д
	}
	else if(USE_HORIZONTAL==2)
	{
		ST7789_SendByte(type_cmd,0x2a);//�е�ַ����
		ST7789_SendHarfWord(x_start+40);
		ST7789_SendHarfWord(x_end+40);
		ST7789_SendByte(type_cmd,0x2b);//�е�ַ����
		ST7789_SendHarfWord(y_start+53);
		ST7789_SendHarfWord(y_end+53);
		ST7789_SendByte(type_cmd,0x2c);//������д
	}
	else
	{
		ST7789_SendByte(type_cmd,0x2a);//�е�ַ����
		ST7789_SendHarfWord(x_start+40);
		ST7789_SendHarfWord(x_end+40);
		ST7789_SendByte(type_cmd,0x2b);//�е�ַ����
		ST7789_SendHarfWord(y_start+52);
		ST7789_SendHarfWord(y_end+52);
		ST7789_SendByte(type_cmd,0x2c);//������д
	}
}

/******************************
�������ƣ�ST7789_SetAddr
�������ܣ�ST7789��Χ����
������ڣ���
�������ڣ���
******************************/
void ST7789_Clear(uint16_t rgb)
{          
	uint16_t i=0,j=0; 
	ST7789_SetAddr(0,0,240-1,135-1);//������ʾ��Χ
	for(i=0;i<240;i++)											   	 	
		for(j=0;j<135;j++)
			ST7789_SendHarfWord(rgb);			
}

/******************************
�������ƣ�ST7789_SetPoint
�������ܣ���㺯��
������ڣ���
�������ڣ���
******************************/
void ST7789_SetPoint(uint16_t x,uint16_t y,uint16_t rgb)
{
	ST7789_SetAddr(x,y,x,y);//������ʾ��Χ
	ST7789_SendHarfWord(rgb);		
}

/***********************************Ӧ�ò㺯��-ͼ��****************************************/
/******************************
�������ƣ�ST7789_SetLine
�������ܣ����ߺ���
������ڣ�
		x_start,y_start	��ʼ��
		x_end,y_end			��ֹ��
		rgb							������ɫ
�������ڣ���
	bug:
		�ȴ����˺���ĵ㣬�ٴ���ǰ��ĵ�
******************************/
void ST7789_SetLine(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,uint16_t rgb,uint16_t back)
{
	uint32_t i = 0,j = 0;
	
	//����	y_start==y_end
	if(y_start==y_end)
	{
		printf("����\r\n");
		ST7789_SetAddr(x_start,y_start,x_end,y_end);//������ʾ��Χ
		for(i=x_start;i<=x_end;i++)
				ST7789_SendHarfWord(rgb);	
	}
	//����	x_start==x_end
	else if(x_start==x_end)
	{
		printf("����\r\n");
		ST7789_SetAddr(x_start,y_start,x_end,y_end);//������ʾ��Χ
		for(i=y_start;i<=y_end;i++)
				ST7789_SendHarfWord(rgb);	
	}
	//б��
	else
	{
		printf("б��\r\n");
		for(j=y_start;j<=y_end;j++)
		{
			for(i=x_start;i<=x_end;i++)
			{
				//�жϵ�ǰ���Ƿ���б����
				if(((x_end - x_start)/(y_end - y_start) == ((x_end - i)*1.0)/((y_end - j)*1.0)))
					ST7789_SetPoint(i,j,rgb);
//								else
//									ST7789_SetPoint(i,j,back);
			}
		}
	}
}

/**********************************************
�����κ���
	1��ֻ���߿�--���û��ߺ���
	2������������ص�--�ο���������
��Բ����
	���÷�Χ
	�ж�ÿһ�����Ƿ���Բ��
**********************************************/
/******************************
�������ƣ�ST7789_SetO
�������ܣ���Բ����
������ڣ�
		x,y	Բ��
		r		�뾶
		rgb	������ɫ
�������ڣ���
******************************/
void ST7789_SetO(uint16_t x,uint16_t y,uint16_t r,uint16_t rgb)
{
	uint32_t i = 0,j = 0;
	
	for(j=y-r;j<=y+r;j++)
	{
		for(i=x-r;i<=x+r;i++)
		{
			//�жϵ��Ƿ���Բ��
			//��ǰ�㵽Բ�ĵľ���>�뾶 ��ǰ����Բ��
			//��ǰ�㵽Բ�ĵľ���<�뾶 ��ǰ����Բ��
			//��ǰ�㵽Բ�ĵľ���=�뾶 ��ǰ����Բ��
			if((r*r - ((i-x)*(i-x)+(j-y)*(j-y))) < 200)
//					if((i-x)*(i-x)+(j-y)*(j-y) < r*r)
				ST7789_SetPoint(i,j,rgb);
		}
	}
}

/******************************
�������ƣ�ST7789_ShowPhoto
�������ܣ���ʾͼƬ����
������ڣ�
	x,y		��ʼ������
	whigh	ͼƬ��
	high	ͼƬ��
	rgb		ͼƬRGBģ������
�������ڣ���
	���ú÷�Χ
	��ÿ�����ص����ɫ��������д��LCD

�������ߣ�ͼƬȡģ���
	ˮƽɨ�� ��λ��ǰ
******************************/
void ST7789_ShowPhoto(uint16_t x,uint16_t y,uint16_t whigh,uint16_t high,const unsigned char *rgb)
{
	uint32_t i = 0,j = 0;
	uint16_t TempRGB = 0;
	 
	ST7789_SetAddr(x,y,x+whigh-1,y+high-1);//������ʾ��Χ
	for(j=y;j<y+high;j++)
	{
		for(i=x;i<x+whigh;i++)
		{
			TempRGB = ((*rgb<<8)|*(rgb+1));//��λ��ǰ
			ST7789_SendHarfWord(TempRGB);
			rgb += 2;
		}
	}
}

/******************************
�������ƣ�ST7789_ShowASCII0816
�������ܣ���ʾ�ַ� 8*16
������ڣ�
	x,y				��ʼ������
	rgb_font	����ɫ
	rgb_back	����ɫ
	font			��ģ����
�������ڣ���
�������ߣ��ַ�ȡģ���
	���� ���� ˳�� C51
******************************/
void ST7789_ShowASCII0816(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,uint8_t *font)
{
	uint32_t i = 0,j = 0;
	 
	ST7789_SetAddr(x,y,x+7,y+15);//������ʾ��Χ
	for(j=0;j<16;j++)	//��ģ �ֽ�����
	{
		for(i=0;i<8;i++)//һ���ֽ� ��λ��������ȡ��
		{
			//0X38 00111000
			/*
				0		1<<7
				1		1<<6
				2		1<<5
			*/
			if(font[j]&(1 << (7-i)))//�����ص���Ҫ��� ������ɫ
				ST7789_SendHarfWord(rgb_font);
			else
				ST7789_SendHarfWord(rgb_back);
		}
	}
}

/******************************
�������ƣ�ST7789_ShowChinese1616
�������ܣ���ʾ���� 16*16
������ڣ�
	x,y				��ʼ������
	rgb_font	����ɫ
	rgb_back	����ɫ
	font			��ģ����
�������ڣ���
�������ߣ��ַ�ȡģ���
	���� ���� ˳�� C51
******************************/
void ST7789_ShowChinese1616(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,uint8_t *font)
{
	uint32_t i = 0,j = 0;
	 
	ST7789_SetAddr(x,y,x+15,y+15);//������ʾ��Χ
	for(j=0;j<32;j++)
	{
		for(i=0;i<8;i++)
		{
			if(font[j]&(1 << (7-i)))//�����ص���Ҫ��� ������ɫ
				ST7789_SendHarfWord(rgb_font);
			else
				ST7789_SendHarfWord(rgb_back);
		}
	}
}

/******************************
�������ƣ�ST7789_ShowString0816
�������ܣ���ʾ�ַ���
������ڣ�
	x,y				��ʼ������
	rgb_font	����ɫ
	rgb_back	����ɫ
	font			��ģ����
�������ڣ���
�������ߣ��ַ�ȡģ���
	���� ���� ˳�� C51
	
	Ӣ���ַ�	1�ֽ� 					��Χ��ASCII��0~127��
	�����ַ�	2�ֽڣ�GB2312��	��λ��->������->������
		������ ����������λ������� > 0XA0
	(��ס���������˽�)
******************************/
void ST7789_ShowString(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,char *str)
{
	uint16_t Tempx=0,Tempy=0;
	int FontOffset = 0;
	Tempx = x;
	Tempy = y;
	
	while(*str != '\0')//�ַ���û��ĩβ
	{
		if(*str < 127)//�ַ�
		{
			//239
			if(Tempx > 231)
			{
				Tempx = x;
				Tempy += 16;
			}
			
			FontOffset = GetOffset_ASCII(*str);
			if(FontOffset != -1)
				ST7789_ShowASCII0816(Tempx,Tempy,rgb_font,rgb_back,&FontLib_ASCII0816[FontOffset*16]);
			str++;
			Tempx+=8;
		}
		else//����
		{
			//239
			if(Tempx > 223)
			{
				Tempx = x;
				Tempy += 16;
			}
			
			FontOffset = GetOffset_Chinese1616(*str,*(str+1));
//			printf("%d",FontOffset);
			if(FontOffset != -1)
				ST7789_ShowChinese1616(Tempx,Tempy,rgb_font,rgb_back,&FontLib_Chinese1616[FontOffset*32]);
			str+=2;
			Tempx+=16;
		}
	}
}
