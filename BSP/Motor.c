/**
 ****************************************************************************************************
 * @file        Motor.c
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-20
 * @brief       电机控制模块：包含PWM初始化、速度控制、状态机及按键处理功能
 ****************************************************************************************************
 */
#include "Motor.h"
#include "key.h"
#include "gpio.h"
#include "stm32f10x.h"
#include "Delay.h"

volatile uint32_t tickCount = 0;  									 // 系统滴答计数器
MotorState motorState; 

/**
  * @brief	 配置直流电机AB相的PWM输出（使用高级定时器TIM8）
  * @param	 无
  * @note	 使用TIM3通道2(PC7)和通道4(PC9)输出
  *			 配置20kHz频率PWM（预分频72-1，周期50-1）
  *			 初始化后定时器禁用输出（占空比0）
  * @version V2.0
  * @note    修改：使用TIM3_CH2（PC7）、TIM3_CH4（PC9）来配置电机的定时器
  * @date    2025-06-24
  */
void Motor_PWM_Init(void)
{
    /* 开启时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	// 使能TIM3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// 使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// 重映射TIM3使用
    
    /* GPIO配置重映射 - PC7(TIM8_CH2), PC9(TIM8_CH4) */
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, DISABLE);		// 先禁用任何可能存在的TIM3重映射配置
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);		// 启用重映射（TIM3_CH2→PC7, TIM3_CH4→PC9）
	
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  		// 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* 时基单元配置 - 20kHz PWM */
	TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;			//定义结构体变量
	/* 配置结构体成员 */
    TIM_TimeBaseStructure.TIM_Period = 50 - 1;				// 50个计数周期 = 20kHz
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;			// 72MHz/72 = 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;		// 不分频
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    /* 输出比较配置 - TIM3 CH2(PC7) */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;						// CCR
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    /* 输出比较配置 - TIM3 CH4(PC9) */
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    /* 使能定时器 */
    TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

/* 电机状态变量 */
static MotorState motorState = MOTOR_STATE_IDLE;
static uint32_t runStartTime = 0;        // 运行开始时间（毫秒）

/**
  * @brief	电机模块全局初始化
  * @param	无
  * @note	初始化PWM输出、系统计时器，重置状态机到IDLE状态
  */
void Motor_Init(void)
{
    Motor_PWM_Init();
    Motor_Stop();
    motorState = MOTOR_STATE_IDLE;
    runStartTime = 0;
}

/**
 * @brief	立即停止电机运行
 * @param	无
 * @note	将TIM3通道2和通道4占空比置零
 */
void Motor_Stop(void)
{
    TIM_SetCompare2(TIM3, 0);
    TIM_SetCompare4(TIM3, 0);
}

/**
  * @brief		设置电机速度和方向
  * @param		direction: 转动方向，1 → 正转 | 0 → 反转
  * @param 		dutyCycle: 占空比值 (0 ~ ARR)，有效范围：0（0%）到 49（100%）
  * @note		采用单通道PWM控制方案：
  *				正转：TIM3_CH2输出PWM，TIM3_CH4输出低电平
  * 			反转：TIM3_CH4输出PWM，TIM3_CH2输出低电平
  * @note		ARR = TIM_Period = 49，经检测，电机在占空比低于34时电压不支持其转动，因此当值<34时会自动修正为34
  * @retval		无
  */

void Motor_SetSpeed(uint16_t dutyCycle, uint8_t direction)
{
    /* 确保占空比在有效范围内 */
    if (dutyCycle < 34) 
	{
        dutyCycle = 34;                     // 达到最小启动阈值
    } 
	else if (dutyCycle > 49) 
	{
        dutyCycle = 49;                     // 不超过最大允许值
    }
    
	/* 正转 */
    if(direction == 1)
    {
        TIM_SetCompare2(TIM3, dutyCycle);   // PC7 PWM
        TIM_SetCompare4(TIM3, 0);           // PC9 低电平
    }
	/* 反转 */
    else
    {
        TIM_SetCompare2(TIM3, 0);           // PC7 低电平
        TIM_SetCompare4(TIM3, dutyCycle);   // PC9 PWM
    }
}

/******************************* 以下为按键控制电机部分 *******************************/
/**
  * @brief  处理按键1操作，设置电机为就绪状态
  * @note   当检测到按键按下时：
  *         - 停止电机运行
  *         - 点亮LED1指示灯
  *         - 熄灭LED3和LED4
  *         - 更新电机状态为MOTOR_STATE_READY（就绪状态）
  *         - 重置运行开始时间
  * @retval 无
  */
void Motor_KEY1_Handler(void)
{
    if(Key1_Scan())  			// 检测到按键按下
    {
        Motor_Stop();           // 停止电机
        LED1_Ctrl(LED_Open);    // 点亮LED1
        LED3_Ctrl(LED_Close);   // 确保LED3关闭
        LED4_Ctrl(LED_Close);   // 确保LED4关闭
			
        /* 更新状态为已初始*/
        motorState = MOTOR_STATE_READY;
        runStartTime = 0;
    }
}

/**
  * @brief  处理按键2操作，停止电机运行
  * @note   当检测到按键按下且电机不处于空闲状态时：
  *         - 停止电机运行
  *         - 关闭所有LED指示灯
  *         - 根据当前运行状态设置过渡状态：
  *           * 运行中 → MOTOR_STATE_STOPPING（停止中）
  *           * 其他状态 → MOTOR_STATE_IDLE（空闲）
  * @retval 无
  */
void Motor_KEY2_Handler(void)
{
    if(Key2_Scan())						// 检测按键
    {
        /* 只有在电机就绪或运行中状态下才能停止 */
        if(motorState != MOTOR_STATE_IDLE)
        {
            Motor_Stop();				// 停止电机
            LED1_Ctrl(LED_Close);		// 熄灭LED1
            LED3_Ctrl(LED_Close);		// 确保LED3关闭
            LED4_Ctrl(LED_Close);		// 确保LED4关闭
            
            /* 如果电机正在运行，设置为停止中状态（稍后会切换到空闲）*/
            if(motorState == MOTOR_STATE_FORWARD || motorState == MOTOR_STATE_BACKWARD) 
			{
                motorState = MOTOR_STATE_STOPPING;
            }
						
			else 
			{
                motorState = MOTOR_STATE_IDLE;
            }
        }
    }
}

/**
  * @brief  处理按键3操作，启动电机正转
  * @param  无
  * @note   仅在电机就绪状态(MOTOR_STATE_READY)有效：
  *         - 点亮LED3指示灯（正转指示）
  *         - 熄灭LED4指示灯
  *         - 设置电机40%占空比正转
  *         - 更新状态为MOTOR_STATE_FORWARD（正转）
  *         - 记录运行开始时间戳
  * @retval 无
  */
void Motor_KEY3_Handler(void)
{
    if(Key3_Scan() && motorState == MOTOR_STATE_READY)
    {
        LED3_Ctrl(LED_Open);							// 点亮LED3
        LED4_Ctrl(LED_Close);							// 确保LED4关闭
        Motor_SetSpeed(40, 1);							// 正转40%
        
        /* 开始运行并记录开始时间 */
        motorState = MOTOR_STATE_FORWARD;
        runStartTime = Delay_GetTickCount();			// 需要使用系统时间函数	
    }
}

/**
  * @brief  处理按键4操作，启动电机反转
  * @param  无
  * @note   仅在电机就绪状态(MOTOR_STATE_READY)有效：
  *         - 点亮LED4指示灯（反转指示）
  *         - 熄灭LED3指示灯
  *         - 设置电机40%占空比反转
  *         - 更新状态为MOTOR_STATE_BACKWARD（反转）
  *         - 记录运行开始时间戳
  * @retval 无
  */
void Motor_KEY4_Handler(void)
{
    if(Key4_Scan() && motorState == MOTOR_STATE_READY)	// 按键按下且电机就绪
    {
        LED4_Ctrl(LED_Open);							// 点亮LED4
        LED3_Ctrl(LED_Close);							// 确保LED3关闭
        Motor_SetSpeed(40, 0);							// 反转40%
        
        /* 开始运行并记录开始时间 */
        motorState = MOTOR_STATE_BACKWARD;
        runStartTime = Delay_GetTickCount();			// 需要使用系统时间函数
    }
}

/**
  * @brief  电机运行时状态监控（需主循环定期调用）
  * @note   功能包含：
  *         - 检测正转/反转/菜单运行状态
  *         - 当运行时间≥7000ms时自动停止电机
  *         - 状态机转换处理：
  *           * 完成停止操作后切换为MOTOR_STATE_IDLE（空闲）
  *           * 超时停止时关闭所有LED指示灯
  *         - 使用GetTickCount()获取系统时间
  * @retval 无
  */
void Motor_RunHandler(void)
{
    /* 如果处于运行状态，检查是否达到延时时间，在多级菜单中也被使用到 */
    if(motorState == MOTOR_STATE_FORWARD || motorState == MOTOR_STATE_BACKWARD || motorState == MOTOR_STATE_MENU_FORWARD || motorState == MOTOR_STATE_MENU_BACKWARD)
    {
        uint32_t currentTime = Delay_GetTickCount();
        uint32_t elapsedTime = currentTime - runStartTime;
        
        /* 如果运行时间超过7000毫秒(7秒) */
        if(elapsedTime >= 7000)
        {
            Motor_Stop();   // 停止电机
			
            /* 返回到停止状态*/
            motorState = MOTOR_STATE_STOPPING;
			LED1_Ctrl(LED_Close);
			LED2_Ctrl(LED_Close);
        }
    }
    /* 如果处于停止中状态，完成后转换到空闲状态 */
    else if(motorState == MOTOR_STATE_STOPPING)
    {
        /* 延时一小段时间确保操作完成 */
        Delay_ms(50);
        motorState = MOTOR_STATE_IDLE;
    }
}
