/******************************************************************************
 * 文件  ：vkpwm.c
 * 描述    ：STM8L PWM
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "vkpwm.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: vkPWM_Init
 * 功能: PWM初始化操作
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无 
 ******************************************************************************/
int vkPWM_Init(vkPWM pwm)
{
	if(pwm == PWM1)
	{
		/* Enable TIM2 clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
		
		/* TIM2 Channel1 configuration: PB0 */
		GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);

		/* TIM2 Channel1, PWM Mode 2,  16MHz/(16*(0+1)) = 1MHz = 1us */
    	TIM2_DeInit();
    	TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up,0);
		//TIM2_SetCounter(0); 				/* 将计数器初值设为0 */
		//TIM2_ARRPreloadConfig(ENABLE);		/* 预装载使能 */
		TIM2_OC1Init(TIM2_OCMode_PWM2,TIM2_OutputState_Enable,0,TIM2_OCPolarity_High,TIM2_OCIdleState_Reset);		
	}
	else if(pwm == PWM2)
	{
		/* Enable TIM3 clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

		/* TIM3 Channel2 configuration: PD0 */
		GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);

		/* TIM2 Channel2, PWM Mode 2,  16MHz/(16*(0+1)) = 1MHz = 1us */
    	TIM3_DeInit();
    	TIM3_TimeBaseInit(TIM3_Prescaler_16,TIM3_CounterMode_Up,0);
		//TIM3_SetCounter(0); 				/* 将计数器初值设为0 */
		//TIM3_ARRPreloadConfig(ENABLE);		/* 预装载使能 */
    	TIM3_OC2Init(TIM3_OCMode_PWM2,TIM3_OutputState_Enable,0,TIM3_OCPolarity_High,TIM3_OCIdleState_Reset);
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*******************************************************************************
 * 名称: vkPWMStart
 * 功能: PWM启动
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无 
 ******************************************************************************/

inline int vkPWMStart(vkPWM pwm, uint32_t period, uint8_t ration)
{
	if(pwm!=PWM1 && pwm!=PWM2)
	{
		return -1;
	}

	if(pwm == PWM1)
	{
		/* Disable TME2 PWM */
		TIM2_CtrlPWMOutputs(DISABLE);
		TIM2_Cmd(DISABLE);	

		/* 将计数器初值设为0 */
		TIM2_SetCounter(0);
		TIM2_SetAutoreload(period);

		/* Set Compare */
		uint16_t compare = period-((uint32_t)period*ration)/100;
		TIM2_SetCompare1(compare);
		
		/* Enable PWM output and timer */
		TIM2_CtrlPWMOutputs(ENABLE);
		TIM2_Cmd(ENABLE);
	}
	else if(pwm == PWM2)
	{
		/* Disable TME3 PWM */
		TIM3_CtrlPWMOutputs(DISABLE);
		TIM3_Cmd(DISABLE);

		/* 将计数器初值设为0 */
		TIM2_SetCounter(0);
		TIM3_SetAutoreload(period);

		/* Set Compare */
		uint16_t compare = period-((uint32_t)period*ration)/100;
		TIM3_SetCompare2(compare);
		
		/* Enable PWM output and timer */
		TIM3_CtrlPWMOutputs(ENABLE);
		TIM3_Cmd(ENABLE);
	}

	return 0;
}

/*******************************************************************************
 * 名称: vkPWM停止
 * 功能: PWM停止
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无 
 ******************************************************************************/
inline int vkPWMStop(vkPWM pwm)
{
	if(pwm!=PWM1 && pwm!=PWM2)
	{
		return -1;
	}

	if(pwm == PWM1)
	{
		/* Disable PWM output and timer */
		TIM2_CtrlPWMOutputs(ENABLE);
		TIM2_Cmd(ENABLE);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	}
	else if(pwm == PWM2)
	{
		/* Disable PWM output and timer */
		TIM3_CtrlPWMOutputs(ENABLE);
		TIM3_Cmd(ENABLE);
		GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	}

	return 0;
}

