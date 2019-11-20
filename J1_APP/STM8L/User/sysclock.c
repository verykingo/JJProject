/******************************************************************************
 * 文件  ：sysclock.c
 * 描述    ：系统时钟设置
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "sysclock.h"

/*******************************************************************************
 * 名称: Sysclock_Init
 * 功能: 设置系统时钟频率，时钟选择16MHz
 * 形参: 无
 * 返回: 无
 * 说明: 系统复位后，默认选择HSI=16/2=8MHz，所有外设时钟关闭
 ******************************************************************************/
int SystemClock_Init(void)
{
	/* Deinitializes the CLK peripheral */
	CLK_DeInit();

	/* 使能时钟切换操作 */
	CLK_SYSCLKSourceSwitchCmd(ENABLE);

	/* 配置系统时钟HSI */
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

	/* 设置系统时钟分频 */
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

	/* Check the system clock source is HSI */
	while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
	{nop();}

	/* 关闭时钟切换操作 */
	CLK_SYSCLKSourceSwitchCmd(DISABLE);

	return 0;
}

/*******************************************************************************
 * 名称: Sysclock_Deinit
 * 功能: 取消系统时初始化
 * 形参: 无
 * 返回: 无
 * 说明: 时钟选择16MHz
 ******************************************************************************/
int Systemclock_Deinit(void)
{
	/* Deinitializes the CLK peripheral */
	CLK_DeInit();

	return 0;
}

/***************************************************************END OF FILE****/