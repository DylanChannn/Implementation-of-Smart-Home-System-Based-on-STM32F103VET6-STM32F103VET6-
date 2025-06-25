/**
 ****************************************************************************************************
 * @file        PWM.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-20
 * @brief       ���������ش��룬��SG90Ϊ����
 ****************************************************************************************************
 */

#include "PWM.h"
#include "Delay.h"
#include <stdio.h>
#include <math.h>

float duty_circle_A = 0.0;

/**
  * @brief		��ʼ��TIM8 PWM�������
  * @note		����TIM8ͨ��1(PC6)����50Hz PWM�źſ��ƶ��
  * @details	1.ʹ��GPIOC��TIM8ʱ��;2. ����PC6Ϊ�����������(TIM8_CH1)
  *			 	3. ����TIM8��������;4. ����PWMģʽ:
  *			 	5. ʹ��Ԥװ�ؼĴ���,6. ����TIM8��������PWM���
  * @remark		�߼���ʱ�������TIM_CtrlPWMOutputs()�����ź����
  * @warning	PWM_PERIOD/PWM_PRESCALER����ͷ�ļ�������ȷֵ
  */
void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* ʹ��ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM8, ENABLE);

    /* ����PC6Ϊ�����������(TIM8_CH1)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*��ʱ���������� */
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;					// �Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER - 1;			// Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    /* PWMģʽ���� */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;									// ��ʼռ�ձ�
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);

    /* ʹ��TIM8Ԥװ�ؼĴ��� */
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8, ENABLE);

    /* ʹ��TIM8 */
    TIM_Cmd(TIM8, ENABLE);
    TIM_CtrlPWMOutputs(TIM8, ENABLE);  // �߼���ʱ����Ҫ������
}

/**
  * @brief	����ָ���Ƕȶ�Ӧ��PWM������
  * @param	angle Ŀ��Ƕ�ֵ��0-180�ȣ�
  * @return	uint16_t PWM�����ȣ�500-2500us��
  * @note	�Ƕȵ�����ת����ʽ��500us(0��) ~ 2500us(180��)
  */
uint16_t Set_Angle(float angle)
{
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
	
	/* ���������Ȳ����� */
    uint16_t pulse = 500 + (2000 * angle) / 180;
    return 500 + (2000 * angle) / 180;
}

/**
  * @brief	���¶��PWMռ�ձ�
  * @param	pulse PWM�����ȣ���λus��
  * @note	���뷶Χ������500-2500us�����¶�ʱ���Ƚ�ֵ
  */
void Update_PWM(uint16_t pulse)
{
    /* ȷ��������500-2500us��Χ�ڣ���Ӧ0-180�ȣ� */
    if(pulse < 500) pulse = 500;
    if(pulse > 2500) pulse = 2500;
    
    /* ����ռ�ձ� */
    TIM_SetCompare1(TIMX_PWM, pulse);
	
	/* �߼���ʱ��(TIM8)��Ҫÿ�θ��º����TIM_CtrlPWMOutputs() */
    TIM_CtrlPWMOutputs(TIMX_PWM, ENABLE);
}

/**
  * @brief ƽ���ƶ���ָ���Ƕ�
  * @param targetAngle Ŀ��Ƕ�ֵ��0-180�ȣ�
  * @note �˺�������ƶ��ƽ���ƶ���ָ���Ƕ�
  */
void MoveToAngle(float targetAngle)
{
    /* �Ƕȷ�Χ���� */
    if(targetAngle < 45.0f) targetAngle = 45.0f;
    if(targetAngle > 135.0f) targetAngle = 135.0f;
    
    /* ֱ������Ŀ��Ƕ� */
    duty_circle_A = targetAngle;
    
    /* ���㲢����PWM */
    uint16_t pulse = 500 + (2000 * duty_circle_A) / 180;
    
    /* ȷ�������ڷ�Χ�� */
    if(pulse < 500) pulse = 500;
    if(pulse > 2500) pulse = 2500;
    
    /* ����ռ�ձ� */
    TIM_SetCompare1(TIM8, pulse);
}

///**
// * @brief		����Ƕ����Ӻ���
// * @param		��
// * @retval		��
// * @note		�ú����������Ӷ���Ƕȣ���menu.c��ʹ��
// *				ÿ�ε����Ƕ�Ϊ5��
// */
//void Increase_PWM_Angle(void)
//{
//    /* ÿ������5�㣬�������45��*/
//    if(duty_circle_A < 90.0 + 45.0) 
//	{
//        duty_circle_A += 5.0;
//        
//        /* ����PWM������ */
//        uint16_t pulse = Set_Angle(duty_circle_A);
//        Update_PWM(pulse);
//    } 
//}

///**
// * @brief		����Ƕȼ��ٺ���
// * @param		��
// * @retval		��
// * @note		�ú����������Ӷ���Ƕȣ���menu.c��ʹ��
// *				ÿ�εݼ��Ƕ�Ϊ5��
// */
///* ���ٶ���Ƕ� */
//void Decrease_PWM_Angle(void)
//{
//    /* ÿ�μ���5�㣬��С����45��*/
//    if(duty_circle_A > 90.0 - 45.0) 
//	{
//        duty_circle_A -= 5.0;
//        
//        /* ����PWM������ */
//        uint16_t pulse = Set_Angle(duty_circle_A);
//        Update_PWM(pulse);
//    }
//}

/**
  * @brief		����Ƕ����Ӻ���
  * @param		��
  * @retval		��
  * @note		�ú����������Ӷ���Ƕȣ���menu.c��ʹ��
  *				ÿ�ε����Ƕ�Ϊ5��ֱ������Ŀ���趨�Ƕ�
  */
void Increase_PWM_Angle(void)
{
    MoveToAngle(135.0);  // ����λ��
}

/**
  * @brief		����Ƕȼ��ٺ���
  * @param		��
  * @retval		��
  * @note		�ú������ڼ��ٶ���Ƕȣ���menu.c��ʹ��
  *				ÿ�εݼ��Ƕ�Ϊ5��ֱ������Ŀ���趨�Ƕ�
  */
void Decrease_PWM_Angle(void)
{
    MoveToAngle(45.0);  // ����λ��
}
