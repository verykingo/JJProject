
/******************************************************************************
 * �ļ�  ��vkpwn.h
 * ����    ��STM8L PWM
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKPWM_H
#define __VKPWM_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */
typedef enum pwm
{
	PWM1 = 0,	//PWM1
	PWM2 = 1,	//PWM2
} vkPWM;

int vkPWM_Init(vkPWM pwm);
int vkPWM_Deinit(vkPWM pwm);
int vkPWMStart(vkPWM pwm, uint32_t period, uint8_t ration);
int vkPWMStop(vkPWM pwm);

#endif


