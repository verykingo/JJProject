/*****************************************************************************
 * 文件名  ：sysclock.c
 * 描述    ：系统时钟设置   
 * 修改时间 ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "sysclock.h"

/*******************************************************************************
 * 名称: Sysclock_Init
 * 功能: 设置系统时钟频率
 * 形参: 无
 * 返回: 无
 * 说明: 时钟选择
         如果选择外部24MHz主时钟的，外部不是24M的话
         把#define HSE_24M  HSE_VALUE注释掉即可,
         如果不需要切换到外部时钟，把#define __HSE_VALUE  HSE_VALUE
         注释掉即可
 ******************************************************************************/
void SystemClock_Init(void)
{
	/* Select HSI as system clock source */
	CLK_SYSCLKSourceSwitchCmd(ENABLE);
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

	/*High speed external clock prescaler: 1*/
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

	/* Check the system clock source is HSI */
	while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
	{}	
}


/***************************************************************END OF FILE****/