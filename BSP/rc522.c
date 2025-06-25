#include "rc522.h"
#include "string.h"
#include "Motor.h"
#include <stdio.h>

#define		MAXRLEN 18
#define		RC522_DELAY()  Delay_us(2)

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
void MFRC522_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB, ENABLE);
	
	/* 使能AFIO时钟用于引脚PD7的重映射 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    /* 释放PD7的JTAG功能 */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	/* 配置 SPI_RC522_SPI 引脚：SDA */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_SDA_PORT, &GPIO_InitStructure);

	/* 配置 SPI_RC522_SPI 引脚：SCK */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_SCK_PORT, &GPIO_InitStructure);

	/* 配置 SPI_RC522_SPI 引脚：MOSI */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_MOSI_PORT, &GPIO_InitStructure);

	/* 配置 SPI_RC522_SPI 引脚：MISO */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MFRC522_GPIO_MISO_PORT, &GPIO_InitStructure);

	/* 配置 SPI_RC522_SPI 引脚：RST */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_RST_PORT, &GPIO_InitStructure);
	
	/* 增加复位初始化 */
    MFRC522_RST_H;			// 拉高复位
    Delay_ms(10);
    MFRC522_Reset();
    MFRC522_AntennaOn();	// 启用天线
}
	

/////////////////////////////////////////////////////////////////////
//功能描述从MFRC522的某一寄存器读一个字节数据
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char Read_MFRC522(unsigned char Address)
{
     unsigned char i, ucAddr;
     unsigned char ucResult=0;

     MFRC522_SCK_L;
	 RC522_DELAY();
     MFRC522_SDA_L;
	 RC522_DELAY();
     ucAddr = ((Address<<1)&0x7E)|0x80;

     for(i=8;i>0;i--)
     {
			 
         if(ucAddr&0x80)
		 {
			MFRC522_MOSI_H;
		 }
		 else
		 {
			MFRC522_MOSI_L;
		 }
		 RC522_DELAY();
         MFRC522_SCK_H;
		 RC522_DELAY();
         ucAddr <<= 1;
         MFRC522_SCK_L;
		 RC522_DELAY();
     }

     for(i=8;i>0;i--)
     {
         MFRC522_SCK_H;
		 RC522_DELAY();
         ucResult <<= 1;
         ucResult|= MFRC522_MISO_READ;
         MFRC522_SCK_L;
		 RC522_DELAY();
     }

     MFRC522_SCK_H;
	 RC522_DELAY();
     MFRC522_SDA_H;
	 RC522_DELAY();
     return 
		ucResult;
}
/////////////////////////////////////////////////////////////////////
//功能描述向MFRC522的某一寄存器写一个字节数据
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void Write_MFRC522(unsigned char Address, unsigned char value)
{  
    unsigned char i, ucAddr;

     MFRC522_SCK_L;
     MFRC522_SDA_L;
    ucAddr = ((Address<<1)&0x7E);

    for(i=8;i>0;i--)
    {
		if(ucAddr&0x80)
		 {
			MFRC522_MOSI_H;
		 }
		 else
		 {
			MFRC522_MOSI_L;
		 }
		 RC522_DELAY();
         MFRC522_SCK_H;
		 RC522_DELAY();
         ucAddr <<= 1;
         MFRC522_SCK_L;
		 RC522_DELAY();
    }

	for(i=8;i>0;i--)
	{
		if(value&0x80)
		{
			MFRC522_MOSI_H;
		}
		else
		{
			MFRC522_MOSI_L;
		}
		 RC522_DELAY();
         MFRC522_SCK_H;
		 RC522_DELAY();
         value <<= 1;
         MFRC522_SCK_L;
		 RC522_DELAY();
    }
	MFRC522_SCK_H;
	RC522_DELAY();
	MFRC522_SDA_H;
	RC522_DELAY();
}


/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Reset(void) 
{
	//unsigned char i;
    MFRC522_RST_H;
	Delay_us (1);             
    MFRC522_RST_L;
	Delay_us (1);                         
    MFRC522_RST_H;
	Delay_us (1);        	

    //MFRC522_RST_H;
    Write_MFRC522(CommandReg,0x0F); //soft reset
    while(Read_MFRC522(CommandReg) & 0x10); //wait chip start ok

	Delay_us (1);
	
	Write_MFRC522(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
	Write_MFRC522(TReloadRegL,30);           
	Write_MFRC522(TReloadRegH,0);
	Write_MFRC522(TModeReg,0x8D);
	Write_MFRC522(TPrescalerReg,0x3E);
	Write_MFRC522(TxAutoReg,0x40);
	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & ~mask);  // clear bit mask
} 


/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char MFRC522_ToCard(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    Write_MFRC522(ComIEnReg,irqEn|0x80);	//PCD_TRANSCEIVE模式 无命令改变
    ClearBitMask(ComIrqReg,0x80);			//IRQ开漏输出
    Write_MFRC522(CommandReg,PCD_IDLE);  //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);		//清除FIFO Flash 内ErrReg  BufferOvfl标志
    
    for (i=0; i<InLenByte; i++)
    {   
				Write_MFRC522(FIFODataReg, pInData[i]);    //把数据存到FIFO
	}
    Write_MFRC522(CommandReg, Command);   //发送FIFO内容
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
				SetBitMask(BitFramingReg,0x80);  //立即停止定时器
	}
    
	n = Read_MFRC522(ComIrqReg);
    i = 1500;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do 
    {
         n = Read_MFRC522(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(Read_MFRC522(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = Read_MFRC522(FIFOLevelReg);
              	lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = Read_MFRC522(FIFODataReg);    }
            }
         }
         else
         {   
			status = MI_ERR;   
		 }
        
   }
   SetBitMask(ControlReg,0x80);           // stop timer now
   Write_MFRC522(CommandReg,PCD_IDLE); 
   return status;
}

//开启天线  
//每次启动或关闭天线发射之间应至少有1ms的间隔
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = Read_MFRC522(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

//关闭天线
void MFRC522_AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

//功能描述用MF522计算CRC
//输入参数pIndata--要读数CRC的数据len--数据长度pOutData--计算的CRC结果
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    Write_MFRC522(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   Write_MFRC522(FIFODataReg, *(pIndata+i));   }
    Write_MFRC522(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Halt(void)
{
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
	
    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    /* 直接返回结果，不存储到中间变量 */
    return MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen) == MI_OK 
           ? MI_OK 
           : MI_ERR;
}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Request(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08);
   Write_MFRC522(BitFramingReg,0x07);
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
   
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
	{   status = MI_ERR;  

	}
   
   return status;
}



//功    能：防冲突检测读取选中卡片的卡序列号
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK 
char MFRC522_Anticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    Write_MFRC522(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];

         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_SelectTag(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}



/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char MFRC522_AuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = MFRC522_ToCard(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char MFRC522_Read(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char MFRC522_Write(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/*********************************** 以下是自己编写的卡片管理部分代码 ***********************************/

/**
  ****************************************************************************************************
  * @file        rc522.c
  * @author      DylanChan
  * @version     V1.0
  * @date        2025-06-23
  * @brief       RC522卡片管理相关代码
  ****************************************************************************************************
  */
unsigned char savedCards[MAX_SAVED_CARDS][6] = {0};  // 存储卡片名(1字节)+序列号(4字节)+激活状态(1字节)
unsigned char savedCardCount = 0;

/**
  * @brief     保存卡片信息
  * @param     cardName[IN]：卡片名称（单字节字符）
  * @retval    MI_OK表示成功保存，MI_ERR表示失败或已满
  */
char Save_Card(unsigned char *cardName)
{
    unsigned char buf[20];
    unsigned char status, i;
    
    /* 检查是否有空间 */
    if(savedCardCount >= MAX_SAVED_CARDS) 
        return MI_ERR;
    
    /* 寻卡 */
    status = MFRC522_Request(PICC_REQALL, buf);
    if(status != MI_OK) return MI_ERR;
    
    /* 防冲突获取序列号 */
    status = MFRC522_Anticoll(buf);
    if(status != MI_OK) return MI_ERR;
    
    /* 检查是否已存在相同序列号 */
    for(i = 0; i < savedCardCount; i++) 
	{
        if(buf[0] == savedCards[i][1] &&buf[1] == savedCards[i][2] &&
           buf[2] == savedCards[i][3] &&buf[3] == savedCards[i][4]) 
		{
            return MI_ERR; // 卡片已存在
        }
    }
    
    /* 保存卡片信息 */
    savedCards[savedCardCount][0] = *cardName;   // 卡片名
    savedCards[savedCardCount][1] = buf[0];     // 序列号字节1
    savedCards[savedCardCount][2] = buf[1];     // 序列号字节2
    savedCards[savedCardCount][3] = buf[2];     // 序列号字节3
    savedCards[savedCardCount][4] = buf[3];     // 序列号字节4
    savedCards[savedCardCount][5] = 1;          // 激活状态
    
    savedCardCount++;
    
    return MI_OK;
}

/**
  * @brief     删除卡片信息
  * @param     cardName[IN]：卡片名称（单字节字符）
  * @retval    MI_OK表示成功删除，MI_ERR表示未找到
  */
char Delete_Card(unsigned char *cardName)
{
    unsigned char i;
    
    for(i = 0; i < savedCardCount; i++) 
	{
        if(savedCards[i][0] == *cardName) 
		{
            /* 设置删除标志 */
            savedCards[i][5] = 0;
            
            /* 如果删除的是最后一个，减少计数 */
            if(i == savedCardCount - 1) 
			{
                savedCardCount--;
            }
            return MI_OK;
        }
    }
    return MI_ERR; // 卡片未找到
}

/**
  * @brief     识别卡片
  * @param     无
  * @retval    MI_OK表示成功识别，MI_ERR表示未识别
  */
char Recognize_Card(void)
{
    unsigned char buf[20];
    unsigned char status, i;
    
    /* 寻卡 */
    status = MFRC522_Request(PICC_REQALL, buf);
    if(status != MI_OK) return MI_ERR;
    
    /* 防冲突获取序列号 */
    status = MFRC522_Anticoll(buf);
    if(status != MI_OK) return MI_ERR;
    
    /* 检查是否在保存的卡片中 */
    for(i = 0; i < savedCardCount; i++) 
	{
		/* 检查激活状态 */
        if(savedCards[i][5] == 1 && buf[0] == savedCards[i][1] && buf[1] == savedCards[i][2] &&
           buf[2] == savedCards[i][3] && buf[3] == savedCards[i][4]) 
		{
            return MI_OK;
        }
    }
    
    /* 未识别的卡片 */
    return MI_ERR;
}

/**
  * @brief      检测卡片是否存在及状态
  * @param[out] serial 用于存储检测到的卡片序列号的缓冲区
  * @retval     MI_OK       - 成功检测到有效的注册卡片
  *             MI_NOTAGERR - 未检测到卡片或卡片未注册
  *             MI_ERR      - 通信错误
  * @note       该函数内部实现时间间隔控制，避免频繁轮询
  */
char RC522_DetectCard(unsigned char *serial)
{
    unsigned char temp_serial[4];       // 临时存储当前读到的序列号
    unsigned char base_serial[4] = {0}; // 存储基准序列号
    unsigned char same_count = 0;       // 相同序列号计数
    
    for (int i = 0; i < 5; i++) 
	{
        /* 检测卡片存在性 */
        char status = MFRC522_Request(PICC_REQALL, temp_serial);
        if (status != MI_OK) {
            /* 请求失败，可能是无卡或通信错误 */
            Delay_ms(200);
            continue;
        }
        
        /* 获取精确序列号 */
        status = MFRC522_Anticoll(temp_serial);
        if (status != MI_OK) {
            /* 防冲突失败，跳过本次 */
            continue;
        }
        
        /* 序列号比较逻辑 */
        if (i == 0) 
		{
            /* 第一次成功读取，设置为基准序列号 */
            memcpy(base_serial, temp_serial, 4);
            same_count = 1;
        } 
        else if (memcmp(base_serial, temp_serial, 4) == 0) 
		{
            /* 与基准序列号相同 */
            same_count++;
        } 
        else 
		{
            /* 序列号变化，重置计数 */
            memcpy(base_serial, temp_serial, 4);
            same_count = 1;
        }
        
        /* 检查是否连续5次相同 */
        if (same_count >= 5) 
		{
            memcpy(serial, base_serial, 4);
            return MI_OK; // 成功检测到稳定卡号
        }
    }
    /* 未检测到稳定卡号 */
    return MI_NOTAGERR; 
}

/**
  * @brief 定期检测卡片状态
  * @note  在主循环中定期调用（每200-500ms）
  */
unsigned char cardSerial[4] = {0};
CardDetectStatus cardStatus = CARD_NONE;

void RC522_PeriodicCheck(void)
{
    static uint32_t lastCheck = 0;
    const uint32_t checkInterval = 300;		// 每300ms检查一次
    
    if(Delay_GetTickCount() - lastCheck < checkInterval) 
	{
        return; // 未到检查时间
    }
    
    lastCheck = Delay_GetTickCount();
    
	static uint8_t stableCount = 0;
    const uint8_t requireStableCount = 3;	// 需要连续3次检测一致
	
    /* 临时存储本次检测结果 */
    CardDetectStatus currentStatus;
    unsigned char tempSerial[4];
    
    char detectResult = RC522_SingleDetectCard(tempSerial);
    
    if(detectResult == MI_OK) 
    {
        currentStatus = CARD_DETECTED;
    } 
    else 
    {
        currentStatus = CARD_NONE;
    }
    
    /* 状态稳定判断 */
    if(currentStatus == cardStatus) 
    {
        stableCount++;
    } 
    else 
    {
        stableCount = 0; // 状态变化，重置稳定计数器
    }
    
    /* 达到稳定要求才更新全局状态 */
    if(stableCount >= requireStableCount) 
    {
        cardStatus = currentStatus;
        memcpy(cardSerial, tempSerial, 4);
    }
	
}

/**
  * @brief  获取卡片检测状态
  * @retval CardDetectStatus 卡片状态
  */
CardDetectStatus GetCardDetectStatus(void)
{
    return cardStatus;
}

/**
  * @brief  获取卡片状态描述
  * @retval 状态描述字符串
  */
const char* GetCardStatusString(void)
{
    switch(cardStatus) 
	{
        case CARD_DETECTED: return "检测到卡片";
        case CARD_NONE:     return "卡片未找到";
        case CARD_ERROR:    return "卡片检测错误";
        default:            return "未知状态";
    }
}

/**
  * @brief 单次卡片检测
  * @param serial 用于存储检测到的卡片序列号的缓冲区
  * @retval MI_OK: 检测到卡片, MI_NOTAGERR: 未检测到卡片, MI_ERR: 通信错误
  */
char RC522_SingleDetectCard(unsigned char *serial)
{
    unsigned char temp_serial[4];	// 临时存储当前读到的序列号
    
    /* 检测卡片存在性 */
    char status = MFRC522_Request(PICC_REQALL, temp_serial);
    if(status != MI_OK) {
        return MI_NOTAGERR;			// 请求失败，可能是无卡或通信错误
    }
    
    /* 获取精确序列号 */
    status = MFRC522_Anticoll(temp_serial);
    if(status != MI_OK) {
        return MI_NOTAGERR;			// 防冲突失败
    }
    
    /* 复制序列号 */
    memcpy(serial, temp_serial, 4);
    return MI_OK;
}
