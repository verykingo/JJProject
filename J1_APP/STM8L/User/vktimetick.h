
/******************************************************************************
 * 文件  ：vktimetick.h
 * 描述    ：STM8L节拍定时器
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKTIMETICK_H
#define __VKTIMETICK_H

/* 包含系统头文件 */

/* 包含自定义头文件 */

/* 自定义数据类型 */
typedef enum TIME
{
	TIME1 = 1,	//定时器1
	TIME2 = 2,	//定时器2
	TIME3 = 3,	//定时器3
	TIME4 = 4,	//定时器4
	TIME5 = 5	//定时器5
} vkTIME;


/* 软定时器每一个Tick等于10毫秒(最大定时时间2^32*10ms=42949000s)时间 */
#define SOFTTIMER_US_PER_TICKS	((uint32_t)(10000ul))

int vkTimeTick_Init(vkTIME TIM);
int vkTimeTick_Deinit(vkTIME TIM);
int vkTimeTickStart(vkTIME TIM);
int vkTimeTickStop(vkTIME TIM);

#endif

