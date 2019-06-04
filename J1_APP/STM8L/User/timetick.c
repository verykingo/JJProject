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
 * 功能: TIM1初始化操作
 * 形参: TIM定时器
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void TimeTick_Init(uint8_t TIM)
{
	if(TIM == 1)
	{
		/* Enable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE); 

	    /* Reset TIM1 */
	    TIM1_DeInit();

	    /* Configure a 10ms tick, 16MHz/(15+1)=1MHz, 1s/MHz=1us */
		TIM1_TimeBaseInit(15, TIM1_CounterMode_Up, 9999, 0);
		TIM1_SetCounter(0);					/* 将计数器初值设为0 */
		TIM1_ARRPreloadConfig(DISABLE);		/* 预装载不使能 */
		ITC_SetSoftwarePriority(TIM1_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);
		
	    /* Generate an interrupt on timer count overflow 计数溢出*/
	    TIM1_ITConfig(TIM1_IT_Update, ENABLE);

	    /* Enable TIM1 */
	    TIM1_Cmd(ENABLE);
	}
}


