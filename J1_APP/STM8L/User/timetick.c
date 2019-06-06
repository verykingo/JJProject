/******************************************************************************
 * 文件名  ：timetick.c
 * 描述    ：STM8L节拍定时器
 * 平台    ：STM8L
 * 修改时间 ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "timetick.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: TimeTick_Init
 * 功能: TIM初始化操作
 * 形参: TIM定时器
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void TimeTick_Init(uint8_t TIM)
{
	/* 定时器1，功能更强，适合用于软定时器 */
	if(TIM == 1)	
	{
		/* Enable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE); 

	    /* Reset TIM1 */
	    TIM1_DeInit();

	    /* Configure a 10us tick, 16MHz/(15+1)=1MHz, 1s/MHz=1us */
		TIM1_TimeBaseInit(15, TIM1_CounterMode_Up, 9, 0);
		TIM1_SetCounter(0);					/* 将计数器初值设为0 */
		TIM1_ARRPreloadConfig(DISABLE);		/* 预装载不使能 */
		ITC_SetSoftwarePriority(TIM1_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);
		
	    /* Generate an interrupt on timer count overflow 计数溢出*/
	    TIM1_ITConfig(TIM1_IT_Update, ENABLE);

	    /* Enable TIM1 */
	    TIM1_Cmd(ENABLE);
	}
	/* 定时器2，功能相对弱，适合用于实时系统 */
	else if(TIM == 2)
	{
		/* Enable TIM2 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE); 

		/* Reset TIM2 */
	    TIM2_DeInit();

		/* Configure a 10ms tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, 9999);
		TIM2_SetCounter(0); 				/* 将计数器初值设为0 */
		TIM2_ARRPreloadConfig(DISABLE); 	/* 预装载不使能 */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn, ITC_PriorityLevel_2);

		/* Generate an interrupt on timer count overflow 计数溢出*/
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);
		
		/* Enable TIM1 */
		TIM2_Cmd(ENABLE);		
	}
}


