/**
 ****************************************************************************************************
 * @file        PWM.c
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-20
 * @brief       舵机控制相关代码，以SG90为例子
 ****************************************************************************************************
 */

#include "PWM.h"
#include "Delay.h"
#include <stdio.h>
#include <math.h>

float duty_circle_A = 0.0;

/**
  * @brief		初始化TIM8 PWM输出功能
  * @note		配置TIM8通道1(PC6)产生50Hz PWM信号控制舵机
  * @details	1.使能GPIOC和TIM8时钟;2. 配置PC6为复用推挽输出(TIM8_CH1)
  *			 	3. 设置TIM8基础参数;4. 配置PWM模式:
  *			 	5. 使能预装载寄存器,6. 启用TIM8计数器及PWM输出
  * @remark		高级定时器需调用TIM_CtrlPWMOutputs()启用信号输出
  * @warning	PWM_PERIOD/PWM_PRESCALER需在头文件定义正确值
  */
void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM8, ENABLE);

    /* 配置PC6为复用推挽输出(TIM8_CH1)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*定时器基础配置 */
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;					// 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER - 1;			// 预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    /* PWM模式配置 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;									// 初始占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);

    /* 使能TIM8预装载寄存器 */
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8, ENABLE);

    /* 使能TIM8 */
    TIM_Cmd(TIM8, ENABLE);
    TIM_CtrlPWMOutputs(TIM8, ENABLE);  // 高级定时器需要此命令
}

/**
  * @brief	计算指定角度对应的PWM脉冲宽度
  * @param	angle 目标角度值（0-180度）
  * @return	uint16_t PWM脉冲宽度（500-2500us）
  * @note	角度到脉冲转换公式：500us(0°) ~ 2500us(180°)
  */
uint16_t Set_Angle(float angle)
{
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
	
	/* 计算脉冲宽度并返回 */
    uint16_t pulse = 500 + (2000 * angle) / 180;
    return 500 + (2000 * angle) / 180;
}

/**
  * @brief	更新舵机PWM占空比
  * @param	pulse PWM脉冲宽度（单位us）
  * @note	输入范围限制在500-2500us，更新定时器比较值
  */
void Update_PWM(uint16_t pulse)
{
    /* 确保脉冲在500-2500us范围内（对应0-180度） */
    if(pulse < 500) pulse = 500;
    if(pulse > 2500) pulse = 2500;
    
    /* 更新占空比 */
    TIM_SetCompare1(TIMX_PWM, pulse);
	
	/* 高级定时器(TIM8)需要每次更新后调用TIM_CtrlPWMOutputs() */
    TIM_CtrlPWMOutputs(TIMX_PWM, ENABLE);
}

/**
  * @brief 平滑移动到指定角度
  * @param targetAngle 目标角度值（0-180度）
  * @note 此函数会控制舵机平滑移动到指定角度
  */
void MoveToAngle(float targetAngle)
{
    /* 角度范围限制 */
    if(targetAngle < 45.0f) targetAngle = 45.0f;
    if(targetAngle > 135.0f) targetAngle = 135.0f;
    
    /* 直接设置目标角度 */
    duty_circle_A = targetAngle;
    
    /* 计算并更新PWM */
    uint16_t pulse = 500 + (2000 * duty_circle_A) / 180;
    
    /* 确保脉冲在范围内 */
    if(pulse < 500) pulse = 500;
    if(pulse > 2500) pulse = 2500;
    
    /* 更新占空比 */
    TIM_SetCompare1(TIM8, pulse);
}

///**
// * @brief		舵机角度增加函数
// * @param		无
// * @retval		无
// * @note		该函数用于增加舵机角度，被menu.c所使用
// *				每次递增角度为5°
// */
//void Increase_PWM_Angle(void)
//{
//    /* 每次增加5°，最大增加45°*/
//    if(duty_circle_A < 90.0 + 45.0) 
//	{
//        duty_circle_A += 5.0;
//        
//        /* 更新PWM脉冲宽度 */
//        uint16_t pulse = Set_Angle(duty_circle_A);
//        Update_PWM(pulse);
//    } 
//}

///**
// * @brief		舵机角度减少函数
// * @param		无
// * @retval		无
// * @note		该函数用于增加舵机角度，被menu.c所使用
// *				每次递减角度为5°
// */
///* 减少舵机角度 */
//void Decrease_PWM_Angle(void)
//{
//    /* 每次减少5°，最小减少45°*/
//    if(duty_circle_A > 90.0 - 45.0) 
//	{
//        duty_circle_A -= 5.0;
//        
//        /* 更新PWM脉冲宽度 */
//        uint16_t pulse = Set_Angle(duty_circle_A);
//        Update_PWM(pulse);
//    }
//}

/**
  * @brief		舵机角度增加函数
  * @param		无
  * @retval		无
  * @note		该函数用于增加舵机角度，被menu.c所使用
  *				每次递增角度为5，直至到达目标设定角度
  */
void Increase_PWM_Angle(void)
{
    MoveToAngle(135.0);  // 关门位置
}

/**
  * @brief		舵机角度减少函数
  * @param		无
  * @retval		无
  * @note		该函数用于减少舵机角度，被menu.c所使用
  *				每次递减角度为5，直至到达目标设定角度
  */
void Decrease_PWM_Angle(void)
{
    MoveToAngle(45.0);  // 开门位置
}
