/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：iwdg.c
 * 描述    ：独立看门狗实验函数   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-11-22
*******************************************************************************/

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "iwdg.h"
#include "led.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */
u32 ReloadDogTime=0;

/*******************************************************************************
 * 名称: IWDG_Conf
 * 功能: 独立看门狗初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void IWDG_Conf(void)
{
	/* 使能IWDG */
	IWDG_Enable();
	/* 写使能预分频与重载寄存器 */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/* 设置IWDG的时钟预分频 ：128KHz/256 */
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	/* 设置应狗的时间 250 * 256 / 128000 = 0.5s */
	IWDG_SetReload(0xFA);
	
	IWDG_ReloadCounter();
}

/*******************************************************************************
 * 名称: Test_IWDGReset
 * 功能: 判断独立看门狗是否复位
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Test_IWDGReset(void)
{
	FlagStatus IwdgFlag;
	
	/* 得到RST位的状态 */ 
	IwdgFlag = RST_GetFlagStatus(RST_FLAG_IWDGF);
	
	/* 判断RST位是否置位 */ 
	if (IwdgFlag)
	{
		LED_Flick();	/* LED灯闪烁进行提示 */
		RST_ClearFlag(RST_FLAG_IWDGF);	/* 清零 IWDGF 标志 */
	}
}
