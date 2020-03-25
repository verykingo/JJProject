
/******************************************************************************
 * 文件  ：vktimetick.c
 * 描述    ：STM8L节拍定时器
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include <stdio.h>

/* 包含自定义头文件 */
#include "vktimetick.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: vkTimeTick_Init
 * 功能: TIM初始化操作
 * 形参: TIM定时器
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkTimeTick_Init(vkTIME TIM)
{
	/* 软定时器-定时器1: 10ms/tick */
	if(TIM == TIME1)	
	{
		printf("%s Set TIM1 [10ms]\r\n", __FUNCTION__);
		
		/* Enable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE); 

	    /* Reset TIM1 */
	    TIM1_DeInit();

	    /* Configure a 10ms tick, 16MHz/(15+1)=1MHz, 1s/MHz=1us */
		TIM1_TimeBaseInit(15, TIM1_CounterMode_Up, 9999, 0);
		TIM1_SetCounter(0);					/* 将计数器初值设为0 */
		TIM1_ARRPreloadConfig(ENABLE);		/* 预装载使能 */
		ITC_SetSoftwarePriority(TIM1_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);
		
	    /* Generate an interrupt on timer count overflow 计数溢出*/
	    TIM1_ITConfig(TIM1_IT_Update, ENABLE);

	    /* Enable TIM1 */
	    //TIM1_Cmd(ENABLE);
	}
	/* 定时器2: 10ms/tick    */
	else if(TIM == TIME2)
	{
		printf("%s Set TIM2 [10ms]\r\n", __FUNCTION__);
		
		/* Enable TIM2 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE); 

		/* Reset TIM2 */
	    TIM2_DeInit();

		/* Configure a 10ms tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, 9999);
		TIM2_SetCounter(0); 				/* 将计数器初值设为0 */
		TIM2_ARRPreloadConfig(ENABLE); 		/* 预装载使能 */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn, ITC_PriorityLevel_1);

		/* Generate an interrupt on timer count overflow 计数溢出*/
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);
		
		/* Enable TIM2 */
		//TIM2_Cmd(ENABLE);		
	}
	/* 定时器3: 10ms/tick    */
	else if(TIM == TIME3)
	{
		printf("%s Set TIM3 [10ms]\r\n", __FUNCTION__);
		
		/* Enable TIM3 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE); 

		/* Reset TIM3 */
	    TIM3_DeInit();

		/* Configure a 10ms tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM3_TimeBaseInit(TIM3_Prescaler_16, TIM3_CounterMode_Up, 9999);
		TIM3_SetCounter(0); 				/* 将计数器初值设为0 */
		TIM3_ARRPreloadConfig(ENABLE); 		/* 预装载使能 */
		ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn, ITC_PriorityLevel_1);

		/* Generate an interrupt on timer count overflow 计数溢出*/
		TIM3_ITConfig(TIM3_IT_Update, ENABLE);
		
		/* Enable TIM3 */
		//TIM3_Cmd(ENABLE);		
	}
	/* 定时器4: 1ms/tick    */
	else if(TIM == TIME4)
	{
	#if 0
		/* Enable TIM4 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); 

		/* Reset TIM4 */
	    TIM4_DeInit();

		/* Configure a 100ms tick, 16MHz/(32768)=2048us, 2048us*49~=100ms */
		/* Configure a 1ms tick, 16MHz/(4)=4us, 4us*250=1ms */
		TIM4_TimeBaseInit(TIM4_Prescaler_4, 249);
		TIM4_SetCounter(0); 				/* 将计数器初值设为0 */
		TIM4_ARRPreloadConfig(ENABLE); 		/* 预装载使能 */
		ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);

		/* Generate an interrupt on timer count overflow 计数溢出*/
		TIM4_ITConfig(TIM4_IT_Update, ENABLE);
		
		/* Enable TIM4 */
		//TIM4_Cmd(ENABLE);	
	#endif
	}
	/* 定时器5: 50us/tick    */
	else if(TIM == TIME5)
	{
		printf("%s Set TIM5 [50us]\r\n", __FUNCTION__);
		
		/* Enable TIM5 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE); 

		/* Reset TIM5 */
	    TIM5_DeInit();

		/* Configure a 50us tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM5_TimeBaseInit(TIM5_Prescaler_16, TIM5_CounterMode_Up, 49);
		TIM5_SetCounter(0); 				/* 将计数器初值设为0 */
		TIM5_ARRPreloadConfig(ENABLE); 		/* 预装载使能 */
		ITC_SetSoftwarePriority(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_3);

		/* Generate an interrupt on timer count overflow 计数溢出*/
		TIM5_ITConfig(TIM5_IT_Update, ENABLE);
		
		/* Enable TIM5 */
		//TIM5_Cmd(ENABLE);		
	}
	else
	{		
		printf("%s Failed!\r\n", __FUNCTION__);
				
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * 名称: vkTimeTick_Deinit
 * 功能: TIM销毁操作
 * 形参: TIM定时器
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkTimeTick_Deinit(vkTIME TIM)
{
	if(TIM == TIME1)
	{
		/* Disable TIM1 */
		TIM1_Cmd(DISABLE);

		/* Reset TIM1 */
	    TIM1_DeInit();	
		
		/* Disable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, DISABLE); 	
	}
	else if(TIM == TIME2)
	{
		/* Disable TIM2 */
		TIM2_Cmd(DISABLE);

		/* Reset TIM2 */
	    TIM2_DeInit();	
		
		/* Disable TIM2 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE); 
	}
	else if(TIM == TIME3)
	{
		/* Disable TIM3 */
		TIM3_Cmd(DISABLE);	

		/* Reset TIM3 */
	    TIM3_DeInit();	
		
		/* Disable TIM3 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE); 
	}
	else if(TIM == TIME4)
	{
		/* Disable TIM4 */
		TIM4_Cmd(ENABLE);

		/* Reset TIM4 */
	    TIM4_DeInit();	
		
		/* Disable TIM4 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE); 
	}
	else if(TIM == TIME5)
	{
		/* Disable TIM5 */
		TIM5_Cmd(DISABLE);	

		/* Reset TIM5 */
	    TIM5_DeInit();	
		
		/* Disable TIM5 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, DISABLE); 
	}
	else
	{		
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * 名称: vkTimeTickStart
 * 功能: TIM启动
 * 形参: TIM定时器
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkTimeTickStart(vkTIME TIM)
{
	if(TIM == TIME1)
	{
	    /* Enable TIM1 */
	    TIM1_Cmd(ENABLE);		
	}
	else if(TIM == TIME2)
	{
	    /* Enable TIM2 */
	    TIM2_Cmd(ENABLE);		
	}
	else if(TIM == TIME3)
	{
	    /* Enable TIM3 */
	    TIM3_Cmd(ENABLE);		
	}
	else if(TIM == TIME4)
	{
	    /* Enable TIM4 */
	    TIM4_Cmd(ENABLE);		
	}
	else if(TIM == TIME5)
	{
	    /* Enable TIM5 */
	    TIM5_Cmd(ENABLE);		
	}
	else
	{
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * 名称: vkTimeTickStop
 * 功能: TIM停止
 * 形参: TIM定时器
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkTimeTickStop(vkTIME TIM)
{
	if(TIM == TIME1)
	{
	    /* Disable TIM1 */
	    TIM1_Cmd(DISABLE);		
	}
	else if(TIM == TIME2)
	{
	    /* Disable TIM2 */
	    TIM2_Cmd(DISABLE);		
	}
	else if(TIM == TIME3)
	{
	    /* Disable TIM3 */
	    TIM3_Cmd(DISABLE);		
	}
	else if(TIM == TIME4)
	{
	    /* Disable TIM4 */
	    TIM4_Cmd(ENABLE);		
	}
	else if(TIM == TIME5)
	{
	    /* Disable TIM5 */
	    TIM5_Cmd(DISABLE);		
	}
	else
	{
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

