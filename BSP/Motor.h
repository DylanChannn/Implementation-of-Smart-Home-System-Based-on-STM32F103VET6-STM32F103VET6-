/****************************************************************************************************
 * @file        motor.h
 * @author      DylanChan(xiangyuzhan@foxmail.com)
 * @version     V1.0
 * @date        2025-06-20
 * @brief       �������ģ�飺����״̬���塢����������ϵͳʱ����ؽӿ�
 * @details     ���ļ������˵����״̬ö�١�ȫ��״̬�����Լ����������صĺ����ӿڣ�������
 *              - ��ʱ��1�жϳ�ʼ����ϵͳʱ���ȡ����
 *              - PWM��ʼ����������ƺ���
 *              - ״̬��������������
 * @note        ʹ��ʱ����main.c�е��ð�����ʼ����״̬������
 ****************************************************************************************************
 */
#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

/* ���״̬ö�� */
typedef enum {
    MOTOR_STATE_IDLE,        	// ����״̬��δ��ʼ����
    MOTOR_STATE_READY,       	// ����״̬���ѳ�ʼ����
    MOTOR_STATE_FORWARD,     	// ��ת����״̬
    MOTOR_STATE_BACKWARD,    	// ��ת����״̬
    MOTOR_STATE_STOPPING,     	// ֹͣ��״̬
	MOTOR_STATE_CLOUD_FORWARD,  // ��ƽ̨������ת
    MOTOR_STATE_CLOUD_BACKWARD, // ��ƽ̨���Ʒ�ת
	MOTOR_STATE_MENU_FORWARD,   // �˵�������ת
    MOTOR_STATE_MENU_BACKWARD   // �˵����Ʒ�ת
} MotorState;

/* ����ȫ�ֵ��״̬���� */
extern MotorState motorState;

void Motor_PWM_Init(void);
void Motor_Init(void);
void Motor_Stop(void);
void Motor_SetSpeed(uint16_t dutyCycle, uint8_t direction);

/* �������������ͬ������ʼ������KeyN_Init();һ������main.c�ļ����ü���ʵ�ְ������Ƶ��ֹͣ�����Զ���ʱֹͣ */
void Motor_RunHandler(void);	// �������ʱ״̬������
void Motor_KEY1_Handler(void);
void Motor_KEY2_Handler(void);
void Motor_KEY3_Handler(void);
void Motor_KEY4_Handler(void);

#endif
