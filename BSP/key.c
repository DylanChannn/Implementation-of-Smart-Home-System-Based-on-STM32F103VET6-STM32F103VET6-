/****************************************************************************************************
 * @file        key.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-18
 * @brief       ��������ģ�飺ʵ���ĸ����������ĳ�ʼ����ɨ�蹦��
 * @details     ���ļ������ĸ�����(KEY1-KEY4)�ĳ�ʼ����״̬ɨ�躯����
 *              - KEY1: PC2�������룬����Ϊ�͵�ƽ
 *              - KEY2: PC3�������룬����Ϊ�ߵ�ƽ
 *              - KEY3: PA0�������룬����Ϊ�͵�ƽ
 *              - KEY4: PA1�������룬����Ϊ�ߵ�ƽ
 *              ʹ��״̬��ʵ�ְ�������������ʱ��200us
 * @note        ÿ������ʹ�ö�����״̬�������ṩ�ȶ��İ����¼����
 ****************************************************************************************************
 */
#include "key.h"
#include "Delay.h"

/**
  * @brief  KEY1(PC2)��ʼ��������Ϊ��������ģʽ������͵�ƽ
  * @param  ��
  * @note   - ʹ��GPIOCʱ��(APB2ʱ�ӿ���)
  *         - PC2����Ϊ��������ģʽ(CNF=01,MODE=00)
  * @retval ��
  */
void Key1_Init(void)
{
    RCC->APB2ENR |= (1<<4);     			// ��GPIOCʱ�� (��ӦAPBENR�Ĵ�����bit4)
    
    GPIOC->CRL &= ~(0xF<<8);    			// bit8-bit11ȡ1111�����PC2����
    GPIOC->CRL |= (0x4<<8);     			// ��������CNF = 01������ģʽ��MODE����Ϊ00��0b0100=0x4
}

/**
  * @brief  KEY1(PC2)״̬ɨ��
  * @param  ��
  * @note   ʹ��״̬��(Key_Idle/Key_Down/Key_Up)ʵ�֣�
  *         - ���PC2���ŵ͵�ƽ��������״̬
  *         - ����ʱ��200us
  * @retval uint8_t 
  *         - 1: ������Ч����(���²��ͷ�)
  *         - 0: �ް����¼�
  */
uint8_t Key1_Scan(void)
{
    static _Key_State Key1_State = Key_Idle;
    static uint16_t count = 0;
    uint8_t return_val = 0;
    
    switch(Key1_State)
    {
        case Key_Idle:
            if(!(GPIOC->IDR & (1<<2)))  	// IDRΪ�������ݼĴ������˴�ר�ż��PC2��2���Ŷ�Ӧ�ĵ�ƽ�����͵�ƽ(����)
                Key1_State = Key_Down;
            break;
            
        case Key_Down:
            if(GPIOC->IDR & (1<<2))    		// ���ߵ�ƽ(�ͷ�)
                Key1_State = Key_Up;
            Delay_us(1);
            count++;
            break;
            
        case Key_Up:
            if(count >= 200)   				// �����ж�ʱ��
                return_val = 1;
            count = 0;
            Key1_State = Key_Idle;
            break;
    }
    return return_val;
}

/**
  * @brief  KEY2(PC3)��ʼ��������Ϊ��������ģʽ������ߵ�ƽ
  * @param  ��
  * @note   - ʹ��GPIOCʱ��(APB2ʱ�ӿ���)
  *         - PC3����Ϊ��������ģʽ(CNF=01,MODE=00)
  * @retval ��
  */
void Key2_Init(void)
{
    RCC->APB2ENR |= (1<<4);     			 // ��GPIOCʱ��
    
    GPIOC->CRL &= ~(0xF<<12);   			 // bit12-15ȡ1111�����PC3����
    GPIOC->CRL |= (0x4<<12);    			 // ��������CNF=01. ����ģʽ��MODE����Ϊ00��0b0100=0x4
}

/**
  * @brief  KEY2(PC3)״̬ɨ��
  * @param  ��
  * @note   ʹ��״̬��(Key_Idle/Key_Down/Key_Up)ʵ�֣�
  *         - ���PC3���Ÿߵ�ƽ��������״̬
  *         - ����ʱ��200us
  * @retval uint8_t 
  *         - 1: ������Ч����(���²��ͷ�)
  *         - 0: �ް����¼�
  */
uint8_t Key2_Scan(void)
{
    static _Key_State Key2_State = Key_Idle;
    static uint16_t count = 0;
    uint8_t return_val = 0;
    
    switch(Key2_State)
    {
        case Key_Idle:
            if(GPIOC->IDR & (1<<3))  		// IDRΪ�������ݼĴ������˴�ר�ż��PC3��3���Ŷ�Ӧ�ĵ�ƽ�����ߵ�ƽ(����)
                Key2_State = Key_Down;
            break;
            
        case Key_Down:
            if(!(GPIOC->IDR & (1<<3)))  	// ���͵�ƽ(�ͷ�)
                Key2_State = Key_Up;
            Delay_us(1);
            count++;
            break;
            
        case Key_Up:
            if(count >= 200)
                return_val = 1;
            count = 0;
            Key2_State = Key_Idle;
            break;
    }
    return return_val;
}

/**
  * @brief  	KEY3(PA0)��ʼ��������Ϊ��������ģʽ������͵�ƽ
  * @param  	��
  * @note   	- ʹ��GPIOAʱ��(APB2ʱ�ӿ���)
  *         	- PA0����Ϊ��������ģʽ(CNF=10,MODE=00)
  *         	- �����ڲ���������(ODR=1)
  * @retval		��
  * @autohor	CY
  */
void Key3_Init(void) 
{
    RCC->APB2ENR |= (1<<2);      			// ��GPIOAʱ��
    GPIOA->CRL &= ~(0xF<<0);   		  		// bit0-3ȡ1111�����PA0����
    GPIOA->CRL |= (0x8<<0);      			// ��������CNF = 10�� MODE = 00��0b1000 = 0x8
    GPIOA->ODR |= (1<<0);        			// ���� PA0 ���ڲ��������裬ȷ������δ����ʱ�����ű��������ȶ��ߵ�ƽ
}

/**
  * @brief  	KEY3(PA0)״̬ɨ��
  * @param  	��
  * @note  		ʹ��״̬��(Key_Idle/Key_Down/Key_Up)ʵ�֣�
  *         	- ���PA0���ŵ͵�ƽ��������״̬
  *         	- ����ʱ��200us
  *         	- ʹ��˫ȡ��(!!)ȷ����ƽʶ��
  * @retval 	uint8_t 
  *         	- 1: ������Ч����(���²��ͷ�)
  *         	- 0: �ް����¼�
  * @autohor	CY
  */
uint8_t Key3_Scan(void)
{
	static _Key_State Key3_State = Key_Idle;
	static uint16_t count = 0;
	uint8_t return_val = 0;
	
	switch(Key3_State)
	{
		case Key_Idle:
		{
			if((!!(GPIOA->IDR & (1<<0))) == 0)
				Key3_State = Key_Down;
			break;
		}
		case Key_Down:
		{
			if((!!(GPIOA->IDR & (1<<0))) == 1)
				Key3_State = Key_Up;
			Delay_us(1);
			count++;
			break;
		}
		case Key_Up:
		{
			if(count >= 200)
				return_val = 1;
			count = 0;
			Key3_State = Key_Idle;
			break;
		}
	}
	return return_val;
}

/**
  * @brief  KEY4(PA1)��ʼ��������Ϊ��������ģʽ������ߵ�ƽ
  * @param  ��
  * @note   - ʹ��GPIOAʱ��(APB2ʱ�ӿ���)
  *         - PA1����Ϊ����/����ģʽ(CNF=10,MODE=00)
  *         - �����ڲ���������(ODR=0)
  * @retval ��
  */
void Key4_Init(void)
{
    RCC->APB2ENR |= (1<<2);      		// ��GPIOAʱ��
    
    GPIOA->CRL &= ~(0xF<<4);    	 	// bit4-7ȡ1111�����PA1����
    GPIOA->CRL |= (0x8<<4);      		// ����/����ģʽCNF = 10�� MODE = 00��0b1000 = 0x8
    GPIOA->ODR &= ~(1<<1);       		// ���� PA0 ���ڲ��������裬ȷ������δ����ʱ�����ű��������ȶ��͵�ƽ
}

/**
  * @brief  KEY4(PA1)״̬ɨ��
  * @param  ��
  * @note   ʹ��״̬��(Key_Idle/Key_Down/Key_Up)ʵ�֣�
  *         - ���PA1���Ÿߵ�ƽ��������״̬
  *         - ����ʱ��200us
  * @retval uint8_t 
  *         - 1: ������Ч����(���²��ͷ�)
  *         - 0: �ް����¼�
  */
uint8_t Key4_Scan(void)
{
    static _Key_State Key4_State = Key_Idle;
    static uint16_t count = 0;
    uint8_t return_val = 0;
    
    switch(Key4_State)
    {
        case Key_Idle:
            if(GPIOA->IDR & (1<<1))  	 // ���ߵ�ƽ(����)
                Key4_State = Key_Down;
            break;
            
        case Key_Down:
            if(!(GPIOA->IDR & (1<<1)))   // ���͵�ƽ(�ͷ�)
                Key4_State = Key_Up;
            Delay_us(1);
            count++;
            break;
            
        case Key_Up:
            if(count >= 200)
                return_val = 1;
            count = 0;
            Key4_State = Key_Idle;
            break;
    }
    return return_val;
}
