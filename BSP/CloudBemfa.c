/****************************************************************************************************
 * @file    cloudbemfa.c
 * @brief   巴法云物联网平台通信模块
 * @details 实现巴法云MQTT协议的连接、订阅、发布及设备控制功能
 * @note    包含WiFi连接、MQTT协议封装、设备状态上报与控制功能
 * @version V1.2
 * @date    2024-03-21 初版
 * @author  用户名
 * @data	2024-06-21 修改增强：增加温湿度/烟雾传感器数据上报、电机控制逻辑优化
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

extern MenuState currentMenu;  // 从menu.h引用的当前菜单状态

int   Fixed_len;      //固定报头长度
int   Variable_len;   //可变报头长度
int   Payload_len;    //有效负荷长度

/**
  * @brief 巴法云连接配置
  * @param 无
  * @note  执行流程：
  *        1. 配置连接参数（服务器地址、端口、客户端ID）
  *        2. 建立TCP连接
  *        3. 初始化MQTT连接并订阅主题
  * @retval 无
  */
void Bemfa_Config(void)
{
	Bemfa_DataConfig();	//链接参数
	/* 链接服务器 */
	if(Bemfa_ConnetServer("TCP",BemfaData.ServerIP,BemfaData.ServerPort))
	{
		/* 连接巴法云设备 */
		MQTT_ConnectBemfa();//发送报文--透传模式
	}
	else
		printf("link Server fail\r\n");
}

void Bemfa_DataConfig(void)
{
	memset(&BemfaData,0,sizeof(_CloudIOT_Connect));  
	
	/* 获取ClientID */
	strcpy(BemfaData.ClientID,Cloud_MyselfPassword);
	BemfaData.ClientID_len = strlen(BemfaData.ClientID);
	/* 获取服务器IP地址、端口号 */
	strcpy(BemfaData.ServerIP,"bemfa.com");                 //构建服务器域名
	BemfaData.ServerPort = 9501;

	/* 串口输出调试信息 */
	ST7789_Clear(BLACK);
	printf("服务器：%s:%d\r\n",BemfaData.ServerIP,BemfaData.ServerPort);  
	ST7789_ShowString(30, 30, BLACK, WHITE, "服务器bemfa.com 9501");
}

uint8_t Bemfa_ConnetServer(char* TCP_IP,char* IP,uint16_t PORT)
{
	uint8_t Timers = 2;
	uint8_t SendData_Buf[128] = "\0";
	
//	LED_CloseTransmission();                   //多次连接需退出透传
  Delay_ms(500);
	
	/* 连接服务器 */
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
	/* 设置数据透传 */
	if(MQTT_SetSendData())
	{
		ST7789_Clear(BLACK);
		printf("数据透传\r\n");
		ST7789_ShowString(30, 30, BLACK, WHITE, "数据透传");
		MQTT_ConnectPack();//连接服务器
		Delay_ms(2000);
		
		/* 订阅每个主题 */
		MQTT_Subscribe(SubscribeLED0_TOPIC,0);
		ST7789_ShowString(50, 40, BLACK, YELLOW, "订阅灯光中");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeTEMHUM_TOPIC,0);
		ST7789_ShowString(50, 40, WHITE, BLUE, "订阅温湿度中");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeMOTOR_TOPIC,0);
		ST7789_ShowString(50, 40, WHITE, LIGHTBLUE, " 订阅窗帘中 ");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeGAS_TOPIC,0);
		ST7789_ShowString(50, 40, WHITE, BROWN, "订阅烟雾传感器中");
		Delay_ms(2000);
		MQTT_Subscribe(SubscribeDOOR_TOPIC,0);
		ST7789_ShowString(50, 40, BLACK, LGRAYBLUE, "  订阅开关门中  ");
		Delay_ms(2000);
		ST7789_Clear(BLACK);
	}
	else
		printf("data send fail\r\n");
}

uint8_t MQTT_SetSendData(void)
{
	uint8_t Timers = 4;
	/* 设置透传模式 */
	if(OpenTransmission()==0) return 0;
	printf("OpenTransmission\r\n");
	Delay_ms(300);
	/* 开启发送状态 */
	while(Timers--)
	{ 
		ESP8266_SendStr("AT+CIPSEND\r\n");//开始处于透传发送状态
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
	Fixed_len = 2;                                                        //连接报文中，固定报头长度=2
	Variable_len = 10;                                                    //连接报文中，可变报头长度=10
	Payload_len = 2 + BemfaData.ClientID_len;       

	SendConnect_buf[0]=0x10;                              	//第1个字节 ：固定0x10	
	SendConnect_buf[1]=Variable_len + Payload_len;        	//第2个字节 ：可变报头+有效负荷的长度，长度小于127
	SendConnect_buf[2]=0x00;          					  	//第3个字节 ：固定0x00	            
	SendConnect_buf[3]=0x04;                				//第4个字节 ：固定0x04
	SendConnect_buf[4]=0x4D;								//第5个字节 ：固定0x4D
	SendConnect_buf[5]=0x51;								//第6个字节 ：固定0x51
	SendConnect_buf[6]=0x54;								//第7个字节 ：固定0x54
	SendConnect_buf[7]=0x54;								//第8个字节 ：固定0x54
	SendConnect_buf[8]=0x04;								//第9个字节 ：固定0x04
	SendConnect_buf[9]=0x02;								//第10个字节：不使用遗嘱，不保留会话
	SendConnect_buf[10]=0x00; 						  		//第11个字节：保活时间高字节 0x00
	SendConnect_buf[11]=0x64;								//第12个字节：保活时间高字节 0x64   100s
	
	/*     CLIENT_ID      */
	SendConnect_buf[12] = BemfaData.ClientID_len/256;                			  			//客户端ID长度高字节
	SendConnect_buf[13] = BemfaData.ClientID_len%256;               			  			//客户端ID长度低字节
	memcpy(&SendConnect_buf[14],BemfaData.ClientID,BemfaData.ClientID_len);                 //复制过来客户端ID字串	
	ESP8266_SendBuf(SendConnect_buf, Fixed_len + Variable_len + Payload_len);      			//加入发送数据缓冲区
	printf("连接服务器成功\r\n");
	ST7789_ShowString(30, 30, BLACK, WHITE, "连接服务器成功！");
	Delay_ms(1000);
	ST7789_Clear(BLACK);
}

/************************
函数名称：MQTT_Subscribe
函数作用：SUBSCRIBE订阅topic报文
函数入口：
		topic_name：	订阅topic报文名称
		QoS：					订阅等级 
函数出口：无
函数作者：
创建时间：2024.03.21
修改时间：2024.03.21
************************/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	uint8_t SendSubscribe_buf[512] = "\0"; 
	Fixed_len = 2;                              //SUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;                           //SUBSCRIBE报文中，可变报头长度=2	
	Payload_len = 2 + strlen(topic_name) + 1;   //计算有效负荷长度 = 2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级
	memset(SendSubscribe_buf,0,512);
	SendSubscribe_buf[0]=0x82;                                    //第1个字节 ：固定0x82                      
	SendSubscribe_buf[1]=Variable_len + Payload_len;              //第2个字节 ：可变报头+有效负荷的长度	
	SendSubscribe_buf[2]=0x00;                                    //第3个字节 ：报文标识符高字节，固定使用0x00
	SendSubscribe_buf[3]=0x01;		                              //第4个字节 ：报文标识符低字节，固定使用0x01
	SendSubscribe_buf[4]=strlen(topic_name)/256;                  //第5个字节 ：topic_name长度高字节
	SendSubscribe_buf[5]=strlen(topic_name)%256;		          //第6个字节 ：topic_name长度低字节
	memcpy(&SendSubscribe_buf[6],topic_name,strlen(topic_name));  //第7个字节开始 ：复制过来topic_name字串		
	SendSubscribe_buf[6+strlen(topic_name)]=QoS;                  //最后1个字节：订阅等级
	ESP8266_SendBuf(SendSubscribe_buf,Fixed_len + Variable_len + Payload_len);
}

/************************
函数名称：MQTT_PublishQs0
函数作用：等级0 发布消息报文
函数入口：
		topic			topic名称
		data			数据
		data_len	数据长度
函数出口：无
函数作者：
创建时间：2021.01.16
修改时间：2021.01.16
************************/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	uint8_t Send_buf[512] = "\0"; 
	Fixed_len = 2;                             //固定报头长度：2字节
	Variable_len = 2 + strlen(topic);          //可变报头长度：2字节(topic长度)+ topic字符串的长度
	Payload_len = data_len;                    //有效负荷长度：就是data_len
	memset(Send_buf,0,512);
	Send_buf[0]=0x30;                         //第1个字节 ：固定0xC0                
	Send_buf[1]=Variable_len + Payload_len;   //第2个字节 ：可变报头+有效负荷的长度	
	if(Variable_len + Payload_len>=128)
	{
		Fixed_len = 3;
		Send_buf[2]=0x01;
		Send_buf[3]=strlen(topic)/256;            //第3个字节 ：topic长度高字节
		Send_buf[4]=strlen(topic)%256;		       //第4个字节 ：topic长度低字节
		memcpy(&Send_buf[5],topic,strlen(topic)); //第5个字节开始 ：拷贝topic字符串
		
		memcpy(&Send_buf[5+strlen(topic)],data,data_len);   //拷贝data数据
	}
	else
	{
		Send_buf[2]=strlen(topic)/256;            //第3个字节 ：topic长度高字节
		Send_buf[3]=strlen(topic)%256;		       //第4个字节 ：topic长度低字节
		memcpy(&Send_buf[4],topic,strlen(topic)); //第5个字节开始 ：拷贝topic字符串	
		
		memcpy(&Send_buf[4+strlen(topic)],data,data_len);   //拷贝data数据
	}
	ESP8266_SendBuf(Send_buf, Fixed_len + Variable_len + Payload_len);  //加入发送数据缓冲区
}
	

/************************
函数名称：MQTT_Send2BemfaData
函数作用：向巴法云发送数据
函数入口：
	topic	发送的报文
	data 	发送数据
函数出口：无
函数作者：
创建时间：2024.03.21
修改时间：2024.03.21
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
  * @brief  云平台LED和蜂鸣器控制处理
  * @param  无
  * @note   功能流程：
  *         1. 从WiFi接收缓冲区解析云平台控制指令
  *         2. 识别包含LED控制主题(SubscribeLED0_TOPIC)的指令
  *         3. 执行指令：
  *            - "on": 开启所有LED和蜂鸣器
  *            - "off": 关闭所有LED和蜂鸣器
  *         4. 清理WiFi接收缓冲区
  * @retval 无
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

/**************************** 封装云平台控制灯蜂鸣器和电机 ****************************/
/**
  * @brief  云平台综合设备控制器
  * @param  无
  * @note   集成控制器，包含：
  *         1. 设备控制功能：
  *            - LED/蜂鸣器控制（通过SubscribeLED0_TOPIC指令）
  *            - 电机控制（通过SubscribeMOTOR_TOPIC指令）
  *         2. 传感器数据上报：
  *            - DHT11温湿度数据（5秒间隔）
  *            - MQ2烟雾传感器数据（5秒间隔）
  *         3. 电机智能管理：
  *            - 状态机控制（空闲/正转/反转）
  *            - 2秒自动停止保护
  *            - 状态指示灯管理（LED3/LED4）
  *         4. 串口数据处理：
  *            - 解析0xAA55协议的串口指令
  *         特殊约束：
  *            - 仅当处于云平台菜单(MENU_CLOUD)时发送传感器数据
  *            - 电机仅空闲状态接受新控制指令
  * @retval 无
  */
uint8_t Bemfa_Buf[50];
void Cloud_Ctrl_LEDBEEP_DHT11_MOTOR(void)
{
    static uint32_t cloudMotorStartTime = 0;
	static uint32_t lastDataSendTime = 0;
	
	/**************************** MQTT控制LED和BEEP以及电机 ****************************/
    uint8_t * GetCloudData = NULL;
    GetCloudData = DealPushdata_Qs0();
    if(GetCloudData)
    {
        /**************************** LED和蜂鸣器控制部分 ****************************/
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
        
       /**************************** 电机控制部分 ****************************/
        if(strstr((char*)GetCloudData, SubscribeMOTOR_TOPIC))
        {
            /* 只有在空闲状态下才接受云平台控制 */
            if(motorState == MOTOR_STATE_IDLE) 
            {
                if(strstr((char*)GetCloudData, "1")) // 电机正转
                {
                    Motor_SetSpeed(40, 1); // 40%占空比正转
                    LED3_Ctrl(LED_Open);   // LED3亮表示正转
                    LED4_Ctrl(LED_Close);  // 关闭反转指示灯
                    
                    /* 设置云平台控制状态和开始时间 */
                    motorState = MOTOR_STATE_CLOUD_FORWARD;
                    cloudMotorStartTime = Delay_GetTickCount(); // 记录开始时间
                }
                else if(strstr((char*)GetCloudData, "0")) // 电机反转
                {
                    Motor_SetSpeed(40, 0); // 40%占空比反转
                    LED4_Ctrl(LED_Open);   // LED4亮表示反转
                    LED3_Ctrl(LED_Close);  // 关闭正转指示灯
                    
                    /* 设置云平台控制状态和开始时间 */
                    motorState = MOTOR_STATE_CLOUD_BACKWARD;
                    cloudMotorStartTime = Delay_GetTickCount(); // 记录开始时间
                }
            }
        }
		
		/***************************** 处理云平台控制的电机状态 *****************************/
		if(motorState == MOTOR_STATE_CLOUD_FORWARD ||  motorState == MOTOR_STATE_CLOUD_BACKWARD)
		{
			uint32_t currentTime = Delay_GetTickCount();
			uint32_t elapsedTime = currentTime - cloudMotorStartTime;
			
			/* 如果运行时间超过2000毫秒(2秒) */
			if(elapsedTime >= 2000)
			{
				Motor_Stop();   // 停止电机
				LED3_Ctrl(LED_Close);
				LED4_Ctrl(LED_Close);
				
				/* 返回到空闲状态 */
				motorState = MOTOR_STATE_IDLE;
			}
		}
		
		/**************************** 开关门控制部分 ****************************/
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

	/***************************** DHT11温湿度采集与发送（每5秒发送一次）*****************************/
    if(Delay_GetTickCount() - lastDataSendTime > 5000) 
    {
        lastDataSendTime = Delay_GetTickCount();
        
        /* 仅当MENU在云平台菜单时才发送数据 */
        if(currentMenu == MENU_CLOUD) 
        {
			/****************************** 发送温湿度数据 ******************************/
            char bemfaBuf[50];
            sprintf(bemfaBuf, "#%.1f#%d#", DHT11_Data.tem, DHT11_Data.hum);
            MQTT_Send2BemfaData(SubscribeTEMHUM_TOPIC, bemfaBuf);
			
			/****************************** 发送烟雾数据 ******************************/
			/* 读取烟雾传感器数据 */
            uint16_t gasADC = MQ2_GetData();         			  // 原始ADC值
            float gasPPM = MQ2_GetData_PPM();       	 		  // PPM值
			char Gas_bemfaBuf[50];
			sprintf(Gas_bemfaBuf, "#%d#%.2f#", gasADC, gasPPM);
            MQTT_Send2BemfaData(SubscribeGAS_TOPIC, Gas_bemfaBuf);// 发送到云平台
        }
    }

    /* 串口数据处理（保留原有功能） */
    if(USART1_Data.flag == 1)
    {
        printf("USART1接收到数据：\r\n");
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
