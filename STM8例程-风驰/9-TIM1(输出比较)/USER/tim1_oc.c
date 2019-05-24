/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：tim1_oc.c
 * 描述    ：TIM1输出比较实验   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-10-20
 
  风驰STM8开发板硬件连接
  LED1-PD0
  TIM1输入捕获/输出比较通道1--PC1
  实验时，需要将PD0与PC1用杜邦线连接

*******************************************************************************/
/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "tim1_oc.h"
#include "stm8s_tim1.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */


/*******************************************************************************
 * 名称: Tim1_OC_Init
 * 功能: TIM1通道1输出比较初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Tim1_OC_Init()
{
	TIM1_DeInit();
	
	/*
	TIM1_Period = 50000
	TIM1_Prescaler = 0
	TIM1_CounterMode = TIM1_COUNTERMODE_UP
	TIM1_RepetitionCounter = 0
	*/
	TIM1_TimeBaseInit(23, TIM1_COUNTERMODE_UP, 49999, 0);	
	
	/*
	TIM1_OCMode = TIM1_OCMODE_TOGGLE
	TIM1_OutputState = TIM1_OUTPUTSTATE_ENABLE
	TIM1_OutputNState = TIM1_OUTPUTNSTATE_DISABLE
	TIM1_Pulse = 5000	
	TIM1_OCPolarity = TIM1_OCPOLARITY_LOW
	TIM1_OCNPolarity = TIM1_OCNPOLARITY_HIGH
	TIM1_OCIdleState = TIM1_OCIDLESTATE_SET
	TIM1_OCNIdleState = TIM1_OCIDLESTATE_RESET
	*/
	TIM1_OC3Init(TIM1_OCMODE_TOGGLE, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE,
			   4999, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET,
			   TIM1_OCNIDLESTATE_RESET);
 	
	/* TIM1 counter 使能 */
	TIM1_Cmd(ENABLE);
	TIM1_CtrlPWMOutputs(ENABLE);	/* 使能TIM1输出 */
}