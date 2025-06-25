#ifndef _ST7789_H_
#define _ST7789_H_

#include "stm32f10x.h"
#include "Delay.h"
#include "stdio.h"
#include "fontlib.h"

#define USE_HORIZONTAL 3  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

/************************


#define LCD_BACK_Pin GPIO_PIN_10
#define LCD_DATA_Pin GPIO_PIN_11
#define LCD_SCLK_Pin GPIO_PIN_12
#define LCD_DC_Pin GPIO_PIN_13
#define LCD_RES_Pin GPIO_PIN_14
#define LCD_CS_Pin GPIO_PIN_15

ģ��SPI SP0
LCD_Back	PD15		����										ͨ���������
LCD_DC		PD14		��������ѡ��						ͨ���������
LCD_CS		PD13		Ƭѡ										ͨ���������
LCD_CLK		PD12		ʱ����									ͨ���������
LCD_data	PD11		������MOSI							ͨ���������
LCD_Rest	PD10		��λ�ܽţ��͵�ƽ��Ч��	ͨ���������
************************/
#define LCD_BackCLK		RCC_APB2Periph_GPIOD	
#define LCD_BackPort	GPIOD
#define LCD_BackPin		GPIO_Pin_10

#define LCD_DCCLK		RCC_APB2Periph_GPIOD	
#define LCD_DCPort	GPIOD
#define LCD_DCPin		GPIO_Pin_13

#define LCD_CSCLK		RCC_APB2Periph_GPIOD	
#define LCD_CSPort	GPIOD
#define LCD_CSPin	GPIO_Pin_15

#define LCD_CLKCLK		RCC_APB2Periph_GPIOD	
#define LCD_CLKPort	GPIOD
#define LCD_CLKPin		GPIO_Pin_12

#define LCD_DataCLK		RCC_APB2Periph_GPIOD	
#define LCD_DataPort	GPIOD
#define LCD_DataPin		GPIO_Pin_11

#define LCD_RestCLK		RCC_APB2Periph_GPIOD	
#define LCD_RestPort	GPIOD
#define LCD_RestPin		GPIO_Pin_14

#define LCDBack_CTRL(x) (x)?(LCD_BackPort->ODR |= LCD_BackPin):(LCD_BackPort->ODR &= ~LCD_BackPin)
#define LCDDC_CTRL(x) 	(x)?(LCD_DCPort->ODR |= LCD_DCPin):(LCD_DCPort->ODR &= ~LCD_DCPin)
#define LCDCS_CTRL(x) 	(x)?(LCD_CSPort->ODR |= LCD_CSPin):(LCD_CSPort->ODR &= ~LCD_CSPin)
#define LCDCLK_CTRL(x) 	(x)?(LCD_CLKPort->ODR |= LCD_CLKPin):(LCD_CLKPort->ODR &= ~LCD_CLKPin)
#define LCDData_CTRL(x) (x)?(LCD_DataPort->ODR |= LCD_DataPin):(LCD_DataPort->ODR &= ~LCD_DataPin)
#define LCDRest_CTRL(x) (x)?(LCD_RestPort->ODR |= LCD_RestPin):(LCD_RestPort->ODR &= ~LCD_RestPin)

typedef enum{
	type_cmd=0,
	type_data
}_ST7789_Type;

void LCD_Config(void);
void ST7789_PortConfig(void);
void ST7789_SoftSP0(uint8_t data);
void ST7789_SendByte(_ST7789_Type Type,uint8_t data);
void ST7789_SendHarfWord(uint16_t data);
void ST7789_Config(void);
void ST7789_Clear(uint16_t rgb);
void ST7789_SetPoint(uint16_t x,uint16_t y,uint16_t rgb);

void ST7789_SetLine(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,uint16_t rgb,uint16_t back);
void ST7789_SetO(uint16_t x,uint16_t y,uint16_t r,uint16_t rgb);
void ST7789_ShowPhoto(uint16_t x,uint16_t y,uint16_t whigh,uint16_t high,const unsigned char *rgb);
void ST7789_ShowASCII0816(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,uint8_t *font);
void ST7789_ShowChinese1616(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,uint8_t *font);
void ST7789_ShowString(uint16_t x,uint16_t y,uint16_t rgb_font,uint16_t rgb_back,char *str);

#endif
