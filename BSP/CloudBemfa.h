/****************************************************************************************************
 * @file    cloudbemfa.h
 * @brief   �ͷ���������ƽ̨ͨ��ģ���ͷ�ļ�
 * @details ʵ�ְͷ���MQTTЭ������ӡ����ġ��������豸���ƹ���
 * @note    ����WiFi���ӡ�MQTTЭ���װ���豸״̬�ϱ�����ƹ���
 * @version V1.2
 * @date    2024-03-21 ����
 *         2024-06-21 �޸���ǿ��������ʪ��/�������������ϱ�����������߼��Ż�
 * @author  DylanChan(xiangyuzhan@foxmail.com)
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

extern uint8_t bemfaConfigured;       // �ͷ������ñ�־
extern uint32_t lastCloudTick;        // ��ƽ̨���ݸ���ʱ���
extern _CloudIOT_Connect BemfaData;   // �ͷ������ݽṹ

//˽Կ
#define Cloud_MyselfPassword 	"d8179d9f61fc4435b12d79f6ac16349b"
#define SubscribeLED0_TOPIC 	"LEDA002"
#define SubscribeTEMHUM_TOPIC 	"TemHum004"
#define SubscribeMOTOR_TOPIC 	"Motor009"
#define SubscribeGAS_TOPIC 		"Gas004"
#define SubscribeDOOR_TOPIC 	"Door006"

void Bemfa_Config(void);
void Bemfa_DataConfig(void);
uint8_t Bemfa_ConnetServer(char* TCP_IP,char* IP,uint16_t PORT);

//MQTT���API
void MQTT_ConnectBemfa(void);
uint8_t MQTT_SetSendData(void);
void MQTT_ConnectPack(void);
void MQTT_Subscribe(char *topic_name, int QoS);
void MQTT_Send2BemfaData(char *topic,char *data);

uint8_t *DealPushdata_Qs0(void);
void Cloud_Ctrl_LEDBEEP(void);
void Cloud_Ctrl_LEDBEEP_DHT11_MOTOR(void);
#endif

