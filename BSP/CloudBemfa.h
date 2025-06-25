/****************************************************************************************************
 * @file    cloudbemfa.h
 * @brief   巴法云物联网平台通信模块的头文件
 * @details 实现巴法云MQTT协议的连接、订阅、发布及设备控制功能
 * @note    包含WiFi连接、MQTT协议封装、设备状态上报与控制功能
 * @version V1.2
 * @date    2024-03-21 初版
 *         2024-06-21 修改增强：增加温湿度/烟雾传感器数据上报、电机控制逻辑优化
 * @author  DylanChan
 ****************************************************************************************************/
#ifndef _CLOUDBEMFA_H_
#define _CLOUDBEMFA_H_

#include "esp01s.h"
#include "gpio.h"

typedef struct{
	char ClientID[64];
	uint16_t ClientID_len;
	char ServerIP[64];
	uint16_t ServerPort;
}_CloudIOT_Connect;

extern uint8_t bemfaConfigured;       // 巴法云配置标志
extern uint32_t lastCloudTick;        // 云平台数据更新时间戳
extern _CloudIOT_Connect BemfaData;   // 巴法云数据结构

//私钥
#define Cloud_MyselfPassword 	"d8179d9f61fc4435b12d79f6ac16349b"
#define SubscribeLED0_TOPIC 	"LEDA002"
#define SubscribeTEMHUM_TOPIC 	"TemHum004"
#define SubscribeMOTOR_TOPIC 	"Motor009"
#define SubscribeGAS_TOPIC 		"Gas004"
#define SubscribeDOOR_TOPIC 	"Door006"

void Bemfa_Config(void);
void Bemfa_DataConfig(void);
uint8_t Bemfa_ConnetServer(char* TCP_IP,char* IP,uint16_t PORT);

//MQTT相关API
void MQTT_ConnectBemfa(void);
uint8_t MQTT_SetSendData(void);
void MQTT_ConnectPack(void);
void MQTT_Subscribe(char *topic_name, int QoS);
void MQTT_Send2BemfaData(char *topic,char *data);

uint8_t *DealPushdata_Qs0(void);
void Cloud_Ctrl_LEDBEEP(void);
void Cloud_Ctrl_LEDBEEP_DHT11_MOTOR(void);
#endif

