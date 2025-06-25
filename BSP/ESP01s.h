#ifndef _ESP01S_H_
#define _ESP01S_H_

#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "Delay.h"

#define WIFI_SSID		"Ayuuu"
#define WIFI_PASSWORD	"09271112"

/*宏接口*/
#define WIFI_USART USART2

typedef struct{
	uint8_t Rx_buf[512];	//接收缓冲区
	uint16_t Rxbuf_Count;	//缓冲区长度
	uint16_t Flag_RxOver;	//接收完成
}_ESP8266_STR;
extern _ESP8266_STR Wifi_Data;

//函数声明
uint8_t ESP8266_Config(void);
void ESP8266_PORTConfig(uint32_t brr);
void ESP8266_SendByte(uint8_t byte);
void ESP8266_SendStr(char *str);
void ESP8266_SendBuf(uint8_t *buf,uint16_t len);
void CloseTransmission(void);
uint8_t OpenTransmission(void);
void Clear_ESP8266Buf(void);
uint8_t FindStr(char* dest,char* src,int retry_nms);

#endif
