/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：wwdg.c
 * 描述    ：wwdg实验   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-11-25
*******************************************************************************/

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "wwdg.h"
#include "led.h"

/* 自定义新类型 */

/* 自定义宏 */
#define COUNTERINIT 0x7f	/* 计数初始值 */
#define WINDOW      0x77	/* 窗口上限值 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: WWDG_Conf
 * 功能: WWDG初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void WWDG_Conf(void) 
{
    /* WWDG 配置 */
    /* WWDG的时钟是跟系统时钟的
  		Watchdog Window= 0x7F step to 0x3F step
    	= (0x7F-0x3F) * 1 step
    	= 64 * 1 step
    	= 64 * (12288/2Mhz)
    	= 393,216ms
    */
    /* Non Allowed Window = (0x7F-window) * 1 step
    = (0x7F-0x77) * 1 step
    = 7 * 1 step 
    = 7 * (12288/2Mhz) 
    = 43.008ms 
    */
    /* 
     * 所以不能在 0.0ms to 43.008ms 这个时间段清计数器
     * 允许的时间段是：43.008ms to 393,216ms 
     */
    WWDG_Init(COUNTERINIT, WINDOW);
}

/*******************************************************************************
 * 名称: Test_WWDGReset
 * 功能: WWDG复位检测操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Test_WWDGReset(void)
{
	FlagStatus WwdgFlag;
	
	WwdgFlag = RST_GetFlagStatus(RST_FLAG_WWDGF);	/* 得到 WWDG Reset 标志位状态 */ 
	/* 测试是否置位 */
	if (WwdgFlag)
	{
		/* 清除 IWDGF 标志 */
		RST_ClearFlag(RST_FLAG_WWDGF);
		LED_Flick();
	}
}

/*******************************************************************************
 * 名称: Refresh_WWDG_Window
 * 功能: 重置WWDG计数寄存器
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Refresh_WWDG_Window(void)
{
	u8 CounterValue;
	CounterValue = (u8)(WWDG_GetCounter() & 0x7F);
	
	/* 
	 * 判断是否小于所设置的窗口上限值 
	 * 只有小于窗口上限值才能清零
	 */
	if(CounterValue < WINDOW)
	{
		WWDG_SetCounter(COUNTERINIT);
	}
}
/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/