/**
 ****************************************************************************************************
 * @file        rc522.h
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-23
 * @brief       RC522卡片管理相关代码头文件
 ****************************************************************************************************
 */
 
#ifndef __RC522_H
#define	__RC522_H
#include "stm32f10x.h"
#include "Delay.h"
#include "math.h"
 
/*********************************** RC522 引脚定义 *********************************************/
/*  引脚宏定义
 *	SDA(CS)---PD7
 *	SCK---PB3
 *	MOSI--PB5
 *	MISO--PB4
 *	空
 *	GND--GND
 *	RST---PB6
 *	3.3V-3.3V
 */
#define MFRC522_GPIO_SDA_PORT    GPIOD
#define MFRC522_GPIO_SDA_PIN     GPIO_Pin_7

#define MFRC522_GPIO_SCK_PORT    GPIOB
#define MFRC522_GPIO_SCK_PIN     GPIO_Pin_3

#define MFRC522_GPIO_MOSI_PORT   GPIOB
#define MFRC522_GPIO_MOSI_PIN    GPIO_Pin_5

#define MFRC522_GPIO_MISO_PORT   GPIOB
#define MFRC522_GPIO_MISO_PIN    GPIO_Pin_4

#define MFRC522_GPIO_RST_PORT    GPIOB
#define MFRC522_GPIO_RST_PIN     GPIO_Pin_6

/*********************************************************************************************/
#define MFRC522_SDA_L    GPIO_ResetBits(MFRC522_GPIO_SDA_PORT, MFRC522_GPIO_SDA_PIN)
#define MFRC522_SDA_H    GPIO_SetBits(MFRC522_GPIO_SDA_PORT, MFRC522_GPIO_SDA_PIN)

#define MFRC522_RST_L    GPIO_ResetBits(MFRC522_GPIO_RST_PORT, MFRC522_GPIO_RST_PIN)
#define MFRC522_RST_H    GPIO_SetBits(MFRC522_GPIO_RST_PORT, MFRC522_GPIO_RST_PIN)

#define MFRC522_SCK_L    GPIO_ResetBits(MFRC522_GPIO_SCK_PORT, MFRC522_GPIO_SCK_PIN)
#define MFRC522_SCK_H    GPIO_SetBits(MFRC522_GPIO_SCK_PORT, MFRC522_GPIO_SCK_PIN)

#define MFRC522_MOSI_L   GPIO_ResetBits(MFRC522_GPIO_MOSI_PORT, MFRC522_GPIO_MOSI_PIN)
#define MFRC522_MOSI_H   GPIO_SetBits(MFRC522_GPIO_MOSI_PORT, MFRC522_GPIO_MOSI_PIN)

#define MFRC522_MISO_READ GPIO_ReadInputDataBit(MFRC522_GPIO_MISO_PORT, MFRC522_GPIO_MISO_PIN)

/*********************************************************************************************/

void MFRC522_Init(void);

/********************************* 卡片管理相关声明 *******************************************/
#define MAX_SAVED_CARDS 5
char Save_Card(unsigned char *cardName);
char Delete_Card(unsigned char *cardName);
char Recognize_Card(void);

extern unsigned char savedCards[MAX_SAVED_CARDS][6];
extern unsigned char savedCardCount;
char RC522_DetectCard(unsigned char *serial);

/* 卡片检测状态 */
typedef enum {
    CARD_NONE,      // 未检测到卡片
    CARD_DETECTED,  // 检测到有效卡片
    CARD_ERROR      // 检测错误
} CardDetectStatus;
extern unsigned char cardSerial[4];		// 卡片序列号
extern CardDetectStatus cardStatus;		// 卡片检测状态

/* 卡片检测函数 */
void RC522_PeriodicCheck(void);
CardDetectStatus GetCardDetectStatus(void);
const char* GetCardStatusString(void);
char RC522_SingleDetectCard(unsigned char *serial);

/*********************************************************************************************/
/////////////////////////////////////////////////////////////////////
//MF522命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位

#define PCD_CALCCRC           0x03               //CRC计算

/////////////////////////////////////////////////////////////////////
//Mifare_One卡片命令字
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		  0x3F

/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define 	MI_OK                 0x26
#define 	MI_NOTAGERR           0xcc
#define 	MI_ERR                0xbb


/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////


#define          macDummy_Data              0x00

char MFRC522_Reset(void);
void Write_MFRC522(unsigned char Address, unsigned char value);
unsigned char Read_MFRC522(unsigned char Address);  
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
char MFRC522_Request(unsigned char req_code,unsigned char *pTagType);
char MFRC522_Anticoll(unsigned char *pSnr);
char MFRC522_SelectTag(unsigned char *pSnr);
char MFRC522_AuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);
char MFRC522_Read(unsigned char addr,unsigned char *pData);
char MFRC522_Halt(void);
char MFRC522_Write(unsigned char addr,unsigned char *pData);
#endif
