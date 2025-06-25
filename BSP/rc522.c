#include "rc522.h"
#include "string.h"
#include "Motor.h"
#include <stdio.h>

#define		MAXRLEN 18
#define		RC522_DELAY()  Delay_us(2)

/*  ���ź궨��
 *	SDA(CS)---PD7
 *	SCK---PB3
 *	MOSI--PB5
 *	MISO--PB4
 *	��
 *	GND--GND
 *	RST---PB6
 *	3.3V-3.3V
 */
void MFRC522_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB, ENABLE);
	
	/* ʹ��AFIOʱ����������PD7����ӳ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    /* �ͷ�PD7��JTAG���� */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	/* ���� SPI_RC522_SPI ���ţ�SDA */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_SDA_PORT, &GPIO_InitStructure);

	/* ���� SPI_RC522_SPI ���ţ�SCK */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_SCK_PORT, &GPIO_InitStructure);

	/* ���� SPI_RC522_SPI ���ţ�MOSI */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_MOSI_PORT, &GPIO_InitStructure);

	/* ���� SPI_RC522_SPI ���ţ�MISO */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MFRC522_GPIO_MISO_PORT, &GPIO_InitStructure);

	/* ���� SPI_RC522_SPI ���ţ�RST */
	GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_GPIO_RST_PORT, &GPIO_InitStructure);
	
	/* ���Ӹ�λ��ʼ�� */
    MFRC522_RST_H;			// ���߸�λ
    Delay_ms(10);
    MFRC522_Reset();
    MFRC522_AntennaOn();	// ��������
}
	

/////////////////////////////////////////////////////////////////////
//������������MFRC522��ĳһ�Ĵ�����һ���ֽ�����
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
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
//������������MFRC522��ĳһ�Ĵ���дһ���ֽ�����
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
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
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
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
	
	Write_MFRC522(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
	Write_MFRC522(TReloadRegL,30);           
	Write_MFRC522(TReloadRegH,0);
	Write_MFRC522(TModeReg,0x8D);
	Write_MFRC522(TPrescalerReg,0x3E);
	Write_MFRC522(TxAutoReg,0x40);
	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & ~mask);  // clear bit mask
} 


/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
   
    Write_MFRC522(ComIEnReg,irqEn|0x80);	//PCD_TRANSCEIVEģʽ ������ı�
    ClearBitMask(ComIrqReg,0x80);			//IRQ��©���
    Write_MFRC522(CommandReg,PCD_IDLE);  //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);		//���FIFO Flash ��ErrReg  BufferOvfl��־
    
    for (i=0; i<InLenByte; i++)
    {   
				Write_MFRC522(FIFODataReg, pInData[i]);    //�����ݴ浽FIFO
	}
    Write_MFRC522(CommandReg, Command);   //����FIFO����
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
				SetBitMask(BitFramingReg,0x80);  //����ֹͣ��ʱ��
	}
    
	n = Read_MFRC522(ComIrqReg);
    i = 1500;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
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

//��������  
//ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = Read_MFRC522(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

//�ر�����
void MFRC522_AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

//������������MF522����CRC
//���������pIndata--Ҫ����CRC�����ݪ�len--���ݳ��Ȫ�pOutData--�����CRC���
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
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Halt(void)
{
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
	
    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    /* ֱ�ӷ��ؽ�������洢���м���� */
    return MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen) == MI_OK 
           ? MI_OK 
           : MI_ERR;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
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



//��    �ܣ�����ͻ��⪡��ȡѡ�п�Ƭ�Ŀ����к�
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK 
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
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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

/*********************************** �������Լ���д�Ŀ�Ƭ�����ִ��� ***********************************/

/**
  ****************************************************************************************************
  * @file        rc522.c
  * @author      DylanChan(xiangyuzhan@foxmail.com)
  * @version     V1.0
  * @date        2025-06-23
  * @brief       RC522��Ƭ������ش���
  ****************************************************************************************************
  */
unsigned char savedCards[MAX_SAVED_CARDS][6] = {0};  // �洢��Ƭ��(1�ֽ�)+���к�(4�ֽ�)+����״̬(1�ֽ�)
unsigned char savedCardCount = 0;

/**
  * @brief     ���濨Ƭ��Ϣ
  * @param     cardName[IN]����Ƭ���ƣ����ֽ��ַ���
  * @retval    MI_OK��ʾ�ɹ����棬MI_ERR��ʾʧ�ܻ�����
  */
char Save_Card(unsigned char *cardName)
{
    unsigned char buf[20];
    unsigned char status, i;
    
    /* ����Ƿ��пռ� */
    if(savedCardCount >= MAX_SAVED_CARDS) 
        return MI_ERR;
    
    /* Ѱ�� */
    status = MFRC522_Request(PICC_REQALL, buf);
    if(status != MI_OK) return MI_ERR;
    
    /* ����ͻ��ȡ���к� */
    status = MFRC522_Anticoll(buf);
    if(status != MI_OK) return MI_ERR;
    
    /* ����Ƿ��Ѵ�����ͬ���к� */
    for(i = 0; i < savedCardCount; i++) 
	{
        if(buf[0] == savedCards[i][1] &&buf[1] == savedCards[i][2] &&
           buf[2] == savedCards[i][3] &&buf[3] == savedCards[i][4]) 
		{
            return MI_ERR; // ��Ƭ�Ѵ���
        }
    }
    
    /* ���濨Ƭ��Ϣ */
    savedCards[savedCardCount][0] = *cardName;   // ��Ƭ��
    savedCards[savedCardCount][1] = buf[0];     // ���к��ֽ�1
    savedCards[savedCardCount][2] = buf[1];     // ���к��ֽ�2
    savedCards[savedCardCount][3] = buf[2];     // ���к��ֽ�3
    savedCards[savedCardCount][4] = buf[3];     // ���к��ֽ�4
    savedCards[savedCardCount][5] = 1;          // ����״̬
    
    savedCardCount++;
    
    return MI_OK;
}

/**
  * @brief     ɾ����Ƭ��Ϣ
  * @param     cardName[IN]����Ƭ���ƣ����ֽ��ַ���
  * @retval    MI_OK��ʾ�ɹ�ɾ����MI_ERR��ʾδ�ҵ�
  */
char Delete_Card(unsigned char *cardName)
{
    unsigned char i;
    
    for(i = 0; i < savedCardCount; i++) 
	{
        if(savedCards[i][0] == *cardName) 
		{
            /* ����ɾ����־ */
            savedCards[i][5] = 0;
            
            /* ���ɾ���������һ�������ټ��� */
            if(i == savedCardCount - 1) 
			{
                savedCardCount--;
            }
            return MI_OK;
        }
    }
    return MI_ERR; // ��Ƭδ�ҵ�
}

/**
  * @brief     ʶ��Ƭ
  * @param     ��
  * @retval    MI_OK��ʾ�ɹ�ʶ��MI_ERR��ʾδʶ��
  */
char Recognize_Card(void)
{
    unsigned char buf[20];
    unsigned char status, i;
    
    /* Ѱ�� */
    status = MFRC522_Request(PICC_REQALL, buf);
    if(status != MI_OK) return MI_ERR;
    
    /* ����ͻ��ȡ���к� */
    status = MFRC522_Anticoll(buf);
    if(status != MI_OK) return MI_ERR;
    
    /* ����Ƿ��ڱ���Ŀ�Ƭ�� */
    for(i = 0; i < savedCardCount; i++) 
	{
		/* ��鼤��״̬ */
        if(savedCards[i][5] == 1 && buf[0] == savedCards[i][1] && buf[1] == savedCards[i][2] &&
           buf[2] == savedCards[i][3] && buf[3] == savedCards[i][4]) 
		{
            return MI_OK;
        }
    }
    
    /* δʶ��Ŀ�Ƭ */
    return MI_ERR;
}

/**
  * @brief      ��⿨Ƭ�Ƿ���ڼ�״̬
  * @param[out] serial ���ڴ洢��⵽�Ŀ�Ƭ���кŵĻ�����
  * @retval     MI_OK       - �ɹ���⵽��Ч��ע�ῨƬ
  *             MI_NOTAGERR - δ��⵽��Ƭ��Ƭδע��
  *             MI_ERR      - ͨ�Ŵ���
  * @note       �ú����ڲ�ʵ��ʱ�������ƣ�����Ƶ����ѯ
  */
char RC522_DetectCard(unsigned char *serial)
{
    unsigned char temp_serial[4];       // ��ʱ�洢��ǰ���������к�
    unsigned char base_serial[4] = {0}; // �洢��׼���к�
    unsigned char same_count = 0;       // ��ͬ���кż���
    
    for (int i = 0; i < 5; i++) 
	{
        /* ��⿨Ƭ������ */
        char status = MFRC522_Request(PICC_REQALL, temp_serial);
        if (status != MI_OK) {
            /* ����ʧ�ܣ��������޿���ͨ�Ŵ��� */
            Delay_ms(200);
            continue;
        }
        
        /* ��ȡ��ȷ���к� */
        status = MFRC522_Anticoll(temp_serial);
        if (status != MI_OK) {
            /* ����ͻʧ�ܣ��������� */
            continue;
        }
        
        /* ���кűȽ��߼� */
        if (i == 0) 
		{
            /* ��һ�γɹ���ȡ������Ϊ��׼���к� */
            memcpy(base_serial, temp_serial, 4);
            same_count = 1;
        } 
        else if (memcmp(base_serial, temp_serial, 4) == 0) 
		{
            /* ���׼���к���ͬ */
            same_count++;
        } 
        else 
		{
            /* ���кű仯�����ü��� */
            memcpy(base_serial, temp_serial, 4);
            same_count = 1;
        }
        
        /* ����Ƿ�����5����ͬ */
        if (same_count >= 5) 
		{
            memcpy(serial, base_serial, 4);
            return MI_OK; // �ɹ���⵽�ȶ�����
        }
    }
    /* δ��⵽�ȶ����� */
    return MI_NOTAGERR; 
}

/**
  * @brief ���ڼ�⿨Ƭ״̬
  * @note  ����ѭ���ж��ڵ��ã�ÿ200-500ms��
  */
unsigned char cardSerial[4] = {0};
CardDetectStatus cardStatus = CARD_NONE;

void RC522_PeriodicCheck(void)
{
    static uint32_t lastCheck = 0;
    const uint32_t checkInterval = 300;		// ÿ300ms���һ��
    
    if(Delay_GetTickCount() - lastCheck < checkInterval) 
	{
        return; // δ�����ʱ��
    }
    
    lastCheck = Delay_GetTickCount();
    
	static uint8_t stableCount = 0;
    const uint8_t requireStableCount = 3;	// ��Ҫ����3�μ��һ��
	
    /* ��ʱ�洢���μ���� */
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
    
    /* ״̬�ȶ��ж� */
    if(currentStatus == cardStatus) 
    {
        stableCount++;
    } 
    else 
    {
        stableCount = 0; // ״̬�仯�������ȶ�������
    }
    
    /* �ﵽ�ȶ�Ҫ��Ÿ���ȫ��״̬ */
    if(stableCount >= requireStableCount) 
    {
        cardStatus = currentStatus;
        memcpy(cardSerial, tempSerial, 4);
    }
	
}

/**
  * @brief  ��ȡ��Ƭ���״̬
  * @retval CardDetectStatus ��Ƭ״̬
  */
CardDetectStatus GetCardDetectStatus(void)
{
    return cardStatus;
}

/**
  * @brief  ��ȡ��Ƭ״̬����
  * @retval ״̬�����ַ���
  */
const char* GetCardStatusString(void)
{
    switch(cardStatus) 
	{
        case CARD_DETECTED: return "��⵽��Ƭ";
        case CARD_NONE:     return "��Ƭδ�ҵ�";
        case CARD_ERROR:    return "��Ƭ������";
        default:            return "δ֪״̬";
    }
}

/**
  * @brief ���ο�Ƭ���
  * @param serial ���ڴ洢��⵽�Ŀ�Ƭ���кŵĻ�����
  * @retval MI_OK: ��⵽��Ƭ, MI_NOTAGERR: δ��⵽��Ƭ, MI_ERR: ͨ�Ŵ���
  */
char RC522_SingleDetectCard(unsigned char *serial)
{
    unsigned char temp_serial[4];	// ��ʱ�洢��ǰ���������к�
    
    /* ��⿨Ƭ������ */
    char status = MFRC522_Request(PICC_REQALL, temp_serial);
    if(status != MI_OK) {
        return MI_NOTAGERR;			// ����ʧ�ܣ��������޿���ͨ�Ŵ���
    }
    
    /* ��ȡ��ȷ���к� */
    status = MFRC522_Anticoll(temp_serial);
    if(status != MI_OK) {
        return MI_NOTAGERR;			// ����ͻʧ��
    }
    
    /* �������к� */
    memcpy(serial, temp_serial, 4);
    return MI_OK;
}
