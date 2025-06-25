/****************************************************************************************************
 * @file    cloudbemfa.c
 * @brief   �ͷ���������ƽ̨ͨ��ģ��
 * @details ʵ�ְͷ���MQTTЭ������ӡ����ġ��������豸���ƹ���
 * @note    ����WiFi���ӡ�MQTTЭ���װ���豸״̬�ϱ�����ƹ���
 * @version V1.2
 * @date    2024-03-21 ����
 * @author  �û���
 * @data	2024-06-21 �޸���ǿ��������ʪ��/�������������ϱ�����������߼��Ż�
 * @author  DylanChan(xiangyuzhan@foxmail.com)
 *****************************************************************************************************
 */
#include "menu.h"
#include "PWM.h"
#include "Motor.h"
#include "cloudbemfa.h"
#include "Delay.h"
#include "gpio.h"
#include "key.h"
#include "usart1.h"
#include "DHT11.h"
#include "MQ2.h"
#include "st7789.h"

extern MenuState currentMenu;  // ��menu.h���õĵ�ǰ�˵�״̬

int   Fixed_len;      //�̶���ͷ����
int   Variable_len;   //�ɱ䱨ͷ����
int   Payload_len;    //��Ч���ɳ���

/**
  * @brief �ͷ�����������
  * @param ��
  * @note  ִ�����̣�
  *        1. �������Ӳ�������������ַ���˿ڡ��ͻ���ID��
  *        2. ����TCP����
  *        3. ��ʼ��MQTT���Ӳ���������
  * @retval ��
  */
void Bemfa_Config(void)
{
	Bemfa_DataConfig();	//���Ӳ���
	/* ���ӷ����� */
	if(Bemfa_ConnetServer("TCP",BemfaData.ServerIP,BemfaData.ServerPort))
	{
		/* ���Ӱͷ����豸 */
		MQTT_ConnectBemfa();//���ͱ���--͸��ģʽ
	}
	else
		printf("link Server fail\r\n");
}

void Bemfa_DataConfig(void)
{
	memset(&BemfaData,0,sizeof(_CloudIOT_Connect));  
	
	/* ��ȡClientID */
	strcpy(BemfaData.ClientID,Cloud_MyselfPassword);
	BemfaData.ClientID_len = strlen(BemfaData.ClientID);
	/* ��ȡ������IP��ַ���˿ں� */
	strcpy(BemfaData.ServerIP,"bemfa.com");                 //��������������
	BemfaData.ServerPort = 9501;

	/* �������������Ϣ */
	ST7789_Clear(BLACK);
	printf("��������%s:%d\r\n",BemfaData.ServerIP,BemfaData.ServerPort);  
	ST7789_ShowString(30, 30, BLACK, WHITE, "������bemfa.com 9501");
}

uint8_t Bemfa_ConnetServer(char* TCP_IP,char* IP,uint16_t PORT)
{
	uint8_t Timers = 2;
	uint8_t SendData_Buf[128] = "\0";
	
//	LED_CloseTransmission();                   //����������˳�͸��
  Delay_ms(500);
	
	/* ���ӷ����� */
	while(Timers--)
	{       
		//AT+CIPSTART="TCP","bemfa.com",9501\r\n
		sprintf((char*)SendData_Buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",TCP_IP,IP,PORT);
		ESP8266_SendStr((char *)SendData_Buf);
		if(FindStr((char*)Wifi_Data.Rx_buf,"CONNECT",8000) !=0)
		{
//			printf("%s\r\n",ESP8266_Rx_buf);
			return 1;
		}
	}
	return 0;
}

void MQTT_ConnectBemfa(void)
{
	/* ��������͸�� */
	if(MQTT_SetSendData())
	{
		ST7789_Clear(BLACK);
		printf("����͸��\r\n");
		ST7789_ShowString(30, 30, BLACK, WHITE, "����͸��");
		MQTT_ConnectPack();//���ӷ�����
		Delay_ms(2000);
		
		/* ����ÿ������ */
		MQTT_Subscribe(SubscribeLED0_TOPIC,0);
		ST7789_ShowString(50, 40, BLACK, YELLOW, "���ĵƹ���");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeTEMHUM_TOPIC,0);
		ST7789_ShowString(50, 40, WHITE, BLUE, "������ʪ����");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeMOTOR_TOPIC,0);
		ST7789_ShowString(50, 40, WHITE, LIGHTBLUE, " ���Ĵ����� ");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeGAS_TOPIC,0);
		ST7789_ShowString(50, 40, WHITE, BROWN, "��������������");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeDOOR_TOPIC,0);
		ST7789_ShowString(50, 40, BLACK, LGRAYBLUE, "  ���Ŀ�������  ");
		Delay_ms(2000);
		ST7789_Clear(BLACK);
	}
	else
		printf("data send fail\r\n");
}

uint8_t MQTT_SetSendData(void)
{
	uint8_t Timers = 4;
	/* ����͸��ģʽ */
	if(OpenTransmission()==0) return 0;
	printf("OpenTransmission\r\n");
	Delay_ms(300);
	/* ��������״̬ */
	while(Timers--)
	{ 
		ESP8266_SendStr("AT+CIPSEND\r\n");//��ʼ����͸������״̬
		if(FindStr((char*)Wifi_Data.Rx_buf,">",400)!=0)
		{
			printf("CIP Send OK\r\n");
			return 1;
		}
//		printf("%s\r\n",Wifi_Data.Rx_buf);
	}
	return 0;
}

void MQTT_ConnectPack(void)
{
	uint8_t SendConnect_buf[512] = "\0"; 
	Fixed_len = 2;                                                        //���ӱ����У��̶���ͷ����=2
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + BemfaData.ClientID_len;       

	SendConnect_buf[0]=0x10;                              	//��1���ֽ� ���̶�0x10	
	SendConnect_buf[1]=Variable_len + Payload_len;        	//��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ��ȣ�����С��127
	SendConnect_buf[2]=0x00;          					  	//��3���ֽ� ���̶�0x00	            
	SendConnect_buf[3]=0x04;                				//��4���ֽ� ���̶�0x04
	SendConnect_buf[4]=0x4D;								//��5���ֽ� ���̶�0x4D
	SendConnect_buf[5]=0x51;								//��6���ֽ� ���̶�0x51
	SendConnect_buf[6]=0x54;								//��7���ֽ� ���̶�0x54
	SendConnect_buf[7]=0x54;								//��8���ֽ� ���̶�0x54
	SendConnect_buf[8]=0x04;								//��9���ֽ� ���̶�0x04
	SendConnect_buf[9]=0x02;								//��10���ֽڣ���ʹ���������������Ự
	SendConnect_buf[10]=0x00; 						  		//��11���ֽڣ�����ʱ����ֽ� 0x00
	SendConnect_buf[11]=0x64;								//��12���ֽڣ�����ʱ����ֽ� 0x64   100s
	
	/*     CLIENT_ID      */
	SendConnect_buf[12] = BemfaData.ClientID_len/256;                			  			//�ͻ���ID���ȸ��ֽ�
	SendConnect_buf[13] = BemfaData.ClientID_len%256;               			  			//�ͻ���ID���ȵ��ֽ�
	memcpy(&SendConnect_buf[14],BemfaData.ClientID,BemfaData.ClientID_len);                 //���ƹ����ͻ���ID�ִ�	
	ESP8266_SendBuf(SendConnect_buf, Fixed_len + Variable_len + Payload_len);      			//���뷢�����ݻ�����
	printf("���ӷ������ɹ�\r\n");
	ST7789_ShowString(30, 30, BLACK, WHITE, "���ӷ������ɹ���");
	Delay_ms(1000);
	ST7789_Clear(BLACK);
}

/************************
�������ƣ�MQTT_Subscribe
�������ã�SUBSCRIBE����topic����
������ڣ�
		topic_name��	����topic��������
		QoS��					���ĵȼ� 
�������ڣ���
�������ߣ�
����ʱ�䣺2024.03.21
�޸�ʱ�䣺2024.03.21
************************/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	uint8_t SendSubscribe_buf[512] = "\0"; 
	Fixed_len = 2;                              //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                           //SUBSCRIBE�����У��ɱ䱨ͷ����=2	
	Payload_len = 2 + strlen(topic_name) + 1;   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�
	memset(SendSubscribe_buf,0,512);
	SendSubscribe_buf[0]=0x82;                                    //��1���ֽ� ���̶�0x82                      
	SendSubscribe_buf[1]=Variable_len + Payload_len;              //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	SendSubscribe_buf[2]=0x00;                                    //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	SendSubscribe_buf[3]=0x01;		                              //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	SendSubscribe_buf[4]=strlen(topic_name)/256;                  //��5���ֽ� ��topic_name���ȸ��ֽ�
	SendSubscribe_buf[5]=strlen(topic_name)%256;		          //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&SendSubscribe_buf[6],topic_name,strlen(topic_name));  //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�		
	SendSubscribe_buf[6+strlen(topic_name)]=QoS;                  //���1���ֽڣ����ĵȼ�
	ESP8266_SendBuf(SendSubscribe_buf,Fixed_len + Variable_len + Payload_len);
}

/************************
�������ƣ�MQTT_PublishQs0
�������ã��ȼ�0 ������Ϣ����
������ڣ�
		topic			topic����
		data			����
		data_len	���ݳ���
�������ڣ���
�������ߣ�
����ʱ�䣺2021.01.16
�޸�ʱ�䣺2021.01.16
************************/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	uint8_t Send_buf[512] = "\0"; 
	Fixed_len = 2;                             //�̶���ͷ���ȣ�2�ֽ�
	Variable_len = 2 + strlen(topic);          //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                    //��Ч���ɳ��ȣ�����data_len
	memset(Send_buf,0,512);
	Send_buf[0]=0x30;                         //��1���ֽ� ���̶�0xC0                
	Send_buf[1]=Variable_len + Payload_len;   //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	if(Variable_len + Payload_len>=128)
	{
		Fixed_len = 3;
		Send_buf[2]=0x01;
		Send_buf[3]=strlen(topic)/256;            //��3���ֽ� ��topic���ȸ��ֽ�
		Send_buf[4]=strlen(topic)%256;		       //��4���ֽ� ��topic���ȵ��ֽ�
		memcpy(&Send_buf[5],topic,strlen(topic)); //��5���ֽڿ�ʼ ������topic�ַ���
		
		memcpy(&Send_buf[5+strlen(topic)],data,data_len);   //����data����
	}
	else
	{
		Send_buf[2]=strlen(topic)/256;            //��3���ֽ� ��topic���ȸ��ֽ�
		Send_buf[3]=strlen(topic)%256;		       //��4���ֽ� ��topic���ȵ��ֽ�
		memcpy(&Send_buf[4],topic,strlen(topic)); //��5���ֽڿ�ʼ ������topic�ַ���	
		
		memcpy(&Send_buf[4+strlen(topic)],data,data_len);   //����data����
	}
	ESP8266_SendBuf(Send_buf, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}
	

/************************
�������ƣ�MQTT_Send2BemfaData
�������ã���ͷ��Ʒ�������
������ڣ�
	topic	���͵ı���
	data 	��������
�������ڣ���
�������ߣ�
����ʱ�䣺2024.03.21
�޸�ʱ�䣺2024.03.21
************************/
void MQTT_Send2BemfaData(char *topic,char *data)
{
	MQTT_PublishQs0(topic,data,strlen(data));
}

uint8_t *DealPushdata_Qs0(void)
{
	uint8_t *p = NULL;
	if(((Wifi_Data.Rxbuf_Count<8)&&(Wifi_Data.Flag_RxOver==1))||(Wifi_Data.Flag_RxOver==0))
		return NULL;
	p = Wifi_Data.Rx_buf;
	
	uint32_t i = 0;
	printf("%s\r\n",p+4);
	for(i=0;i<Wifi_Data.Rxbuf_Count;i++)
		printf("0X%x ",Wifi_Data.Rx_buf[i]);
	printf("\r\n");
	
//	p += (4 + strlen(topic));
	p += 4;
	return p;
	
}

/**
  * @brief  ��ƽ̨LED�ͷ��������ƴ���
  * @param  ��
  * @note   �������̣�
  *         1. ��WiFi���ջ�����������ƽ̨����ָ��
  *         2. ʶ�����LED��������(SubscribeLED0_TOPIC)��ָ��
  *         3. ִ��ָ�
  *            - "on": ��������LED�ͷ�����
  *            - "off": �ر�����LED�ͷ�����
  *         4. ����WiFi���ջ�����
  * @retval ��
  */
void Cloud_Ctrl_LEDBEEP(void)
{
	uint8_t * GetCloudData = NULL;
	GetCloudData = DealPushdata_Qs0();
	if(GetCloudData)
	{
		if(strstr((char*)GetCloudData,SubscribeLED0_TOPIC))
		{
			if(strstr((char*)GetCloudData,"on"))
			{
				LED1_Ctrl(LED_Open);
				LED2_Ctrl(LED_Open);
				LED3_Ctrl(LED_Open);
				LED4_Ctrl(LED_Open);
				BEEP_Ctrl(BEEP_Open);
			}
			if(strstr((char*)GetCloudData,"off"))
			{
				LED1_Ctrl(LED_Close);
				LED2_Ctrl(LED_Close);
				LED3_Ctrl(LED_Close);
				LED4_Ctrl(LED_Close);
				BEEP_Ctrl(BEEP_Close);
			}
		}
		Clear_ESP8266Buf();
	}
}

/**************************** ��װ��ƽ̨���ƵƷ������͵�� ****************************/
/**
  * @brief  ��ƽ̨�ۺ��豸������
  * @param  ��
  * @note   ���ɿ�������������
  *         1. �豸���ƹ��ܣ�
  *            - LED/���������ƣ�ͨ��SubscribeLED0_TOPICָ�
  *            - ������ƣ�ͨ��SubscribeMOTOR_TOPICָ�
  *         2. �����������ϱ���
  *            - DHT11��ʪ�����ݣ�5������
  *            - MQ2�����������ݣ�5������
  *         3. ������ܹ���
  *            - ״̬�����ƣ�����/��ת/��ת��
  *            - 2���Զ�ֹͣ����
  *            - ״ָ̬ʾ�ƹ���LED3/LED4��
  *         4. �������ݴ���
  *            - ����0xAA55Э��Ĵ���ָ��
  *         ����Լ����
  *            - ����������ƽ̨�˵�(MENU_CLOUD)ʱ���ʹ���������
  *            - ���������״̬�����¿���ָ��
  * @retval ��
  */
uint8_t Bemfa_Buf[50];
void Cloud_Ctrl_LEDBEEP_DHT11_MOTOR(void)
{
    static uint32_t cloudMotorStartTime = 0;
	static uint32_t lastDataSendTime = 0;
	
	/**************************** MQTT����LED��BEEP�Լ���� ****************************/
    uint8_t * GetCloudData = NULL;
    GetCloudData = DealPushdata_Qs0();
    if(GetCloudData)
    {
        /**************************** LED�ͷ��������Ʋ��� ****************************/
        if(strstr((char*)GetCloudData, SubscribeLED0_TOPIC))
        {
            if(strstr((char*)GetCloudData, "on"))
            {
                LED1_Ctrl(LED_Open); 
				LED2_Ctrl(LED_Open);
                LED3_Ctrl(LED_Open);
				LED4_Ctrl(LED_Open);
                BEEP_Ctrl(BEEP_Open);
            }
            if(strstr((char*)GetCloudData, "off"))
            {
                LED1_Ctrl(LED_Close); 
				LED2_Ctrl(LED_Close);
                LED3_Ctrl(LED_Close); 
				LED4_Ctrl(LED_Close);
                BEEP_Ctrl(BEEP_Close);
            }
        }
        
       /**************************** ������Ʋ��� ****************************/
        if(strstr((char*)GetCloudData, SubscribeMOTOR_TOPIC))
        {
            /* ֻ���ڿ���״̬�²Ž�����ƽ̨���� */
            if(motorState == MOTOR_STATE_IDLE) 
            {
                if(strstr((char*)GetCloudData, "1")) // �����ת
                {
                    Motor_SetSpeed(40, 1); // 40%ռ�ձ���ת
                    LED3_Ctrl(LED_Open);   // LED3����ʾ��ת
                    LED4_Ctrl(LED_Close);  // �رշ�תָʾ��
                    
                    /* ������ƽ̨����״̬�Ϳ�ʼʱ�� */
                    motorState = MOTOR_STATE_CLOUD_FORWARD;
                    cloudMotorStartTime = Delay_GetTickCount(); // ��¼��ʼʱ��
                }
                else if(strstr((char*)GetCloudData, "0")) // �����ת
                {
                    Motor_SetSpeed(40, 0); // 40%ռ�ձȷ�ת
                    LED4_Ctrl(LED_Open);   // LED4����ʾ��ת
                    LED3_Ctrl(LED_Close);  // �ر���תָʾ��
                    
                    /* ������ƽ̨����״̬�Ϳ�ʼʱ�� */
                    motorState = MOTOR_STATE_CLOUD_BACKWARD;
                    cloudMotorStartTime = Delay_GetTickCount(); // ��¼��ʼʱ��
                }
            }
        }
		
		/***************************** ������ƽ̨���Ƶĵ��״̬ *****************************/
		if(motorState == MOTOR_STATE_CLOUD_FORWARD ||  motorState == MOTOR_STATE_CLOUD_BACKWARD)
		{
			uint32_t currentTime = Delay_GetTickCount();
			uint32_t elapsedTime = currentTime - cloudMotorStartTime;
			
			/* �������ʱ�䳬��2000����(2��) */
			if(elapsedTime >= 2000)
			{
				Motor_Stop();   // ֹͣ���
				LED3_Ctrl(LED_Close);
				LED4_Ctrl(LED_Close);
				
				/* ���ص�����״̬ */
				motorState = MOTOR_STATE_IDLE;
			}
		}
		
		/**************************** �����ſ��Ʋ��� ****************************/
		if(strstr((char*)GetCloudData, SubscribeDOOR_TOPIC))
        {
            if(strstr((char*)GetCloudData, "on"))
            {
                Increase_PWM_Angle();
            }
            if(strstr((char*)GetCloudData, "off"))
            {
                Decrease_PWM_Angle();
            }
        }
		/*************************************************************************/
		Clear_ESP8266Buf();
    }

	/***************************** DHT11��ʪ�Ȳɼ��뷢�ͣ�ÿ5�뷢��һ�Σ�*****************************/
    if(Delay_GetTickCount() - lastDataSendTime > 5000) 
    {
        lastDataSendTime = Delay_GetTickCount();
        
        /* ����MENU����ƽ̨�˵�ʱ�ŷ������� */
        if(currentMenu == MENU_CLOUD) 
        {
			/****************************** ������ʪ������ ******************************/
            char bemfaBuf[50];
            sprintf(bemfaBuf, "#%.1f#%d#", DHT11_Data.tem, DHT11_Data.hum);
            MQTT_Send2BemfaData(SubscribeTEMHUM_TOPIC, bemfaBuf);
			
			/****************************** ������������ ******************************/
			/* ��ȡ������������ */
            uint16_t gasADC = MQ2_GetData();         			  // ԭʼADCֵ
            float gasPPM = MQ2_GetData_PPM();       	 		  // PPMֵ
			char Gas_bemfaBuf[50];
			sprintf(Gas_bemfaBuf, "#%d#%.2f#", gasADC, gasPPM);
            MQTT_Send2BemfaData(SubscribeGAS_TOPIC, Gas_bemfaBuf);// ���͵���ƽ̨
        }
    }

    /* �������ݴ�������ԭ�й��ܣ� */
    if(USART1_Data.flag == 1)
    {
        printf("USART1���յ����ݣ�\r\n");
        for(uint16_t i = 0; i < USART1_Data.count; i++)
		printf("%x ", USART1_Data.RxBuf[i]);
		printf("\r\n");
        
        if((USART1_Data.RxBuf[0] == 0xAA) && (USART1_Data.RxBuf[1] == 0x55))
        {
            if((USART1_Data.RxBuf[2] + 4) == USART1_Data.count)
            {
                if(Get_Add(USART1_Data.RxBuf, USART1_Data.count - 1) == USART1_Data.RxBuf[4])
                {
                    LED1_Ctrl((USART1_Data.RxBuf[3] >> 4) & 0x01);
                    LED2_Ctrl((USART1_Data.RxBuf[3] >> 3) & 0x01);
                    LED3_Ctrl((USART1_Data.RxBuf[3] >> 2) & 0x01);
                    LED4_Ctrl((USART1_Data.RxBuf[3] >> 1) & 0x01);
                    BEEP_Ctrl((USART1_Data.RxBuf[3] >> 0) & 0x01);
                }
            }
        }
        
        memset(USART1_Data.RxBuf, 0, USART1_RX_len);
        USART1_Data.count = 0;
        USART1_Data.flag = 0;
    }
}
