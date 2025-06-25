#include "st7789.h"

/******************************
函数名称：ST7789_PortConfig
函数功能：ST7789管脚初始化
函数入口：无
函数出口：无
******************************/
void LCD_Config(void)
{
		ST7789_PortConfig();	//底层接口初始化
		ST7789_Config();		//LCD驱动初始化-ST7789
		ST7789_Clear(BLACK);	//屏幕清屏--白色
}

/******************************
函数名称：ST7789_PortConfig
函数功能：ST7789管脚初始化
函数入口：无
函数出口：无
******************************/
void ST7789_PortConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};
	
	//时钟使能
	RCC_APB2PeriphClockCmd(LCD_BackCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_DCCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_CSCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_CLKCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_DataCLK,ENABLE);
	RCC_APB2PeriphClockCmd(LCD_RestCLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //速度50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
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
	
	LCDBack_CTRL(0);	//背光板关灯
	LCDCLK_CTRL(0);		//SP0 时钟线空闲状态低电平
	LCDRest_CTRL(1);	//复位拉高，屏幕不重启 
	LCDCS_CTRL(1);		//CS初始化高电平，代表SPI设备从机不选中
}

/******************************
函数名称：ST7789_SoftSP0
函数功能：模式SPI SP0
函数入口：无
函数出口：无
	空闲状态CPOL=0 低电平
	时钟相位CPHA=0 时钟前沿发送数据
	高位先发

	0XAA
	0X80 = 10000000
	
	10101010	IDR
	10000000

	<<之后
	01010100
	10000000
******************************/
void ST7789_SoftSP0(uint8_t data) 
{	
	uint8_t i = 0;
	for(i=0;i<8;i++)		//一次发送八位数据
	{			  
		LCDCLK_CTRL(0);
		
		if(data & 0x80)		//判断最高位是0还是1
			 LCDData_CTRL(1);
		else
			 LCDData_CTRL(0);
		
		LCDCLK_CTRL(1);
		
		data <<= 1;		//高位先发
	}	
}

/******************************
函数名称：ST7789_SendByte
函数功能：发送字节
函数入口：无
函数出口：无
******************************/
void ST7789_SendByte(_ST7789_Type Type,uint8_t data)
{
	LCDCS_CTRL(0);			//CS拉低--数据开始传输
	LCDDC_CTRL(Type);//选择发送数据是cmd还是data
	ST7789_SoftSP0(data);
	LCDCS_CTRL(1);
}

/******************************
函数名称：ST7789_SendByte
函数功能：发送半字
函数入口：无
函数出口：无
******************************/
void ST7789_SendHarfWord(uint16_t data)
{
	LCDDC_CTRL(1);//选择发送数据是cmd还是data
	LCDCS_CTRL(0);			//CS拉低--数据开始传输
	ST7789_SoftSP0(data>>8);
	ST7789_SoftSP0(data);
	LCDCS_CTRL(1);
}

/******************************
函数名称：ST7789_Config
函数功能：ST7789驱动初始化
函数入口：无
函数出口：无
******************************/
void ST7789_Config(void)
{
	//驱动写入之前硬件复位
	LCDRest_CTRL(0);
	Delay_ms(100);
	LCDRest_CTRL(1);
	Delay_ms(100);
	//打开背光
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
	//	ST7789_SendByte(type_data,0xA1);   //sleep in后，gate输出为GND

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
	//	ST7789_SendByte(type_data,0x1D);   //使用240根gate  (N+1)*8
	//	ST7789_SendByte(type_data,0x00);   //设定gate起点位置
	//	ST7789_SendByte(type_data,0x00);   //当gate没有用完时，bit4(TMG)设为0
	ST7789_SendByte(type_cmd,0x21);     
	ST7789_SendByte(type_cmd,0x29);
}

/******************************
函数名称：ST7789_SetAddr
函数功能：ST7789范围设置
函数入口：无
函数出口：无
******************************/
void ST7789_SetAddr(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
	if(USE_HORIZONTAL==0)
	{
		ST7789_SendByte(type_cmd,0x2a);//列地址设置
		ST7789_SendHarfWord(x_start+52);
		ST7789_SendHarfWord(x_end+52);
		ST7789_SendByte(type_cmd,0x2b);//行地址设置
		ST7789_SendHarfWord(y_start+40);
		ST7789_SendHarfWord(y_end+40);
		ST7789_SendByte(type_cmd,0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		ST7789_SendByte(type_cmd,0x2a);//列地址设置
		ST7789_SendHarfWord(x_start+53);
		ST7789_SendHarfWord(x_end+53);
		ST7789_SendByte(type_cmd,0x2b);//行地址设置
		ST7789_SendHarfWord(y_start+40);
		ST7789_SendHarfWord(y_end+40);
		ST7789_SendByte(type_cmd,0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		ST7789_SendByte(type_cmd,0x2a);//列地址设置
		ST7789_SendHarfWord(x_start+40);
		ST7789_SendHarfWord(x_end+40);
		ST7789_SendByte(type_cmd,0x2b);//行地址设置
		ST7789_SendHarfWord(y_start+53);
		ST7789_SendHarfWord(y_end+53);
		ST7789_SendByte(type_cmd,0x2c);//储存器写
	}
	else
	{
		ST7789_SendByte(type_cmd,0x2a);//列地址设置
		ST7789_SendHarfWord(x_start+40);
		ST7789_SendHarfWord(x_end+40);
		ST7789_SendByte(type_cmd,0x2b);//行地址设置
		ST7789_SendHarfWord(y_start+52);
		ST7789_SendHarfWord(y_end+52);
		ST7789_SendByte(type_cmd,0x2c);//储存器写
	}
}

/******************************
函数名称：ST7789_SetAddr
函数功能：ST7789范围设置
函数入口：无
函数出口：无
******************************/
void ST7789_Clear(uint16_t rgb)
{          
	uint16_t i=0,j=0; 
	ST7789_SetAddr(0,0,240-1,135-1);//设置显示范围
	for(i=0;i<240;i++)											   	 	
		for(j=0;j<135;j++)
			ST7789_SendHarfWord(rgb);			
}

/******************************
函数名称：ST7789_SetPoint
函数功能：打点函数
函数入口：无
函数出口：无
******************************/
void ST7789_SetPoint(uint16_t x,uint16_t y,uint16_t rgb)
{
	ST7789_SetAddr(x,y,x,y);//设置显示范围
	ST7789_SendHarfWord(rgb);		
}

/***********************************应用层函数-图形****************************************/
/******************************
函数名称：ST7789_SetLine
函数功能：画线函数
函数入口：
		x_start,y_start	起始点
		x_end,y_end			终止点
		rgb							画线颜色
函数出口：无
	bug:
		先传参了后面的点，再传参前面的点
******************************/
void ST7789_SetLine(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,uint16_t rgb,uint16_t back)
{
	uint32_t i = 0,j = 0;
	
	//横线	y_start==y_end
	if(y_start==y_end)
	{
		printf("横线\r\n");
		ST7789_SetAddr(x_start,y_start,x_end,y_end);//设置显示范围
		for(i=x_start;i<=x_end;i++)
				ST7789_SendHarfWord(rgb);	
	}
	//竖线	x_start==x_end
	else if(x_start==x_end)
	{
		printf("竖线\r\n");
		ST7789_SetAddr(x_start,y_start,x_end,y_end);//设置显示范围
		for(i=y_start;i<=y_end;i++)
				ST7789_SendHarfWord(rgb);	
	}
	//斜线
	else
	{
		printf("斜线\r\n");
		for(j=y_start;j<=y_end;j++)
		{
			for(i=x_start;i<=x_end;i++)
			{
				//判断当前点是否在斜线上
				if(((x_end - x_start)/(y_end - y_start) == ((x_end - i)*1.0)/((y_end - j)*1.0)))
					ST7789_SetPoint(i,j,rgb);
//								else
//									ST7789_SetPoint(i,j,back);
			}
		}
	}
}

/**********************************************
画矩形函数
	1）只画边框--调用画线函数
	2）填充所有像素点--参考清屏函数
画圆函数
	设置范围
	判断每一个点是否在圆上
**********************************************/
/******************************
函数名称：ST7789_SetO
函数功能：画圆函数
函数入口：
		x,y	圆心
		r		半径
		rgb	画线颜色
函数出口：无
******************************/
void ST7789_SetO(uint16_t x,uint16_t y,uint16_t r,uint16_t rgb)
{
	uint32_t i = 0,j = 0;
	
	for(j=y-r;j<=y+r;j++)
	{
		for(i=x-r;i<=x+r;i++)
		{
			//判断点是否在圆上
			//当前点到圆心的距离>半径 当前点在圆外
			//当前点到圆心的距离<半径 当前点在圆内
			//当前点到圆心的距离=半径 当前点在圆边
			if((r*r - ((i-x)*(i-x)+(j-y)*(j-y))) < 200)
//					if((i-x)*(i-x)+(j-y)*(j-y) < r*r)
				ST7789_SetPoint(i,j,rgb);
		}
	}
}

/******************************
函数名称：ST7789_ShowPhoto
函数功能：显示图片函数
函数入口：
	x,y		起始点坐标
	whigh	图片宽
	high	图片高
	rgb		图片RGB模型数组
函数出口：无
	设置好范围
	将每个像素点的颜色，按规律写入LCD

借助工具：图片取模软件
	水平扫描 高位在前
******************************/
void ST7789_ShowPhoto(uint16_t x,uint16_t y,uint16_t whigh,uint16_t high,const unsigned char *rgb)
{
	uint32_t i = 0,j = 0;
	uint16_t TempRGB = 0;
	 
	ST7789_SetAddr(x,y,x+whigh-1,y+high-1);//设置显示范围
	for(j=y;j<y+high;j++)
	{
		for(i=x;i<x+whigh;i++)
		{
			TempRGB = ((*rgb<<8)|*(rgb+1));//高位在前
			ST7789_SendHarfWord(TempRGB);
			rgb += 2;
		}
	}
}

/******************************
函数名称：ST7789_ShowASCII0816
函数功能：显示字符 8*16
函数入口：
	x,y				起始点坐标
	rgb_font	字体色
	rgb_back	背景色
	font			字模数组
函数出口：无
借助工具：字符取模软件
	阴码 逐行 顺向 C51
******************************/
void ST7789_ShowASCII0816(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,uint8_t *font)
{
	uint32_t i = 0,j = 0;
	 
	ST7789_SetAddr(x,y,x+7,y+15);//设置显示范围
	for(j=0;j<16;j++)	//字模 字节总数
	{
		for(i=0;i<8;i++)//一个字节 八位数据依次取出
		{
			//0X38 00111000
			/*
				0		1<<7
				1		1<<6
				2		1<<5
			*/
			if(font[j]&(1 << (7-i)))//本像素点需要描点 字体颜色
				ST7789_SendHarfWord(rgb_font);
			else
				ST7789_SendHarfWord(rgb_back);
		}
	}
}

/******************************
函数名称：ST7789_ShowChinese1616
函数功能：显示汉字 16*16
函数入口：
	x,y				起始点坐标
	rgb_font	字体色
	rgb_back	背景色
	font			字模数组
函数出口：无
借助工具：字符取模软件
	阴码 逐行 顺向 C51
******************************/
void ST7789_ShowChinese1616(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,uint8_t *font)
{
	uint32_t i = 0,j = 0;
	 
	ST7789_SetAddr(x,y,x+15,y+15);//设置显示范围
	for(j=0;j<32;j++)
	{
		for(i=0;i<8;i++)
		{
			if(font[j]&(1 << (7-i)))//本像素点需要描点 字体颜色
				ST7789_SendHarfWord(rgb_font);
			else
				ST7789_SendHarfWord(rgb_back);
		}
	}
}

/******************************
函数名称：ST7789_ShowString0816
函数功能：显示字符串
函数入口：
	x,y				起始点坐标
	rgb_font	字体色
	rgb_back	背景色
	font			字模数组
函数出口：无
借助工具：字符取模软件
	阴码 逐行 顺向 C51
	
	英文字符	1字节 					范围在ASCII表（0~127）
	汉字字符	2字节（GB2312）	区位码->国标码->机内码
		机内码 区码机内码和位码机内码 > 0XA0
	(记住：后面做了解)
******************************/
void ST7789_ShowString(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,char *str)
{
	uint16_t Tempx=0,Tempy=0;
	int FontOffset = 0;
	Tempx = x;
	Tempy = y;
	
	while(*str != '\0')//字符串没到末尾
	{
		if(*str < 127)//字符
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
		else//汉字
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
