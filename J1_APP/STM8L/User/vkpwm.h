
/******************************************************************************
 * 文件  ：vkpwn.h
 * 描述    ：STM8L PWM
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKPWM_H
#define __VKPWM_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */
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


