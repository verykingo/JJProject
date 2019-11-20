/******************************************************************************
 * 文件  ：vkleds.c
 * 描述    ：led指示灯
 * 平台    ：STM8L
 * 时间  ：2019-09-04

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include <stdio.h>
#include <stdlib.h>

/* 包含自定义头文件 */
#include "vkleds.h"
#include "vktimetick.h"

/* 自定义新类型 */

/* 自定义宏 */
#define LED_REPEAT_END(x) 	{x + LED_MODE_COUNT-1, 0}
#define LED_NEXT_MODE(x)	{x, 0}
#define LED_ALIGN3			{0, 0}, {0, 0}, {0, 0}
#define LED_ALIGN2			{0, 0}, {0, 0}
#define LED_ALIGN1			{0, 0}

/* 全局变量定义 */
static vkLEDS gLeds = {

	.led_gpio = {
		{GPIOE, GPIO_Pin_0},
 		{GPIOE, GPIO_Pin_1}
	},
	
	.led_mtable = {
		/*Connected*/	
		{.mode = LED_MODE_CONNECTED,.stat = {{LED_STATE_GREEN_ON, 1000},	LED_NEXT_MODE(LED_MODE_CONNECTED),	LED_ALIGN1}},
		/*Unconnect*/	
		{.mode = LED_MODE_UNCONNECT,.stat = {{LED_STATE_GREEN_ON, 1000},	{LED_STATE_GREEN_OFF, 1000},		LED_NEXT_MODE(LED_MODE_UNCONNECT)}},
		/*Charging*/
		{.mode = LED_MODE_CHARGING,	.stat = {{LED_STATE_RED_ON, 1000},		{LED_STATE_RED_OFF, 1000},			LED_NEXT_MODE(LED_MODE_CHARGING)}},
		/*Lowbatt*/ 	
		{.mode = LED_MODE_LOWBATT,	.stat = {{LED_STATE_RED_ON, 500},		{LED_STATE_RED_OFF, 500},			LED_NEXT_MODE(LED_MODE_LOWBATT)}},
		/*Poweron */
		{.mode = LED_MODE_POWERON,	.stat = {{LED_STATE_ALL_ON, 1000},		LED_NEXT_MODE(LED_MODE_POWERON), 	LED_ALIGN1}},
		/*Shutdown*/
		{.mode = LED_MODE_SHUTDOWN,	.stat = {{LED_STATE_ALL_OFF, 1000}, 	LED_NEXT_MODE(LED_MODE_SHUTDOWN),	LED_ALIGN1}},
		/*Runerror*/
		{.mode = LED_MODE_RUNERROR,	.stat = {{LED_STATE_ALL_ON, 500},		{LED_STATE_ALL_OFF, 500},			LED_NEXT_MODE(LED_MODE_SHUTDOWN)}},
		/*Test*/
		{.mode = LED_MODE_TEST,		.stat = {{LED_STATE_ALL_ON, 1000},		{LED_STATE_ALL_OFF, 1000},			LED_NEXT_MODE(LED_MODE_TEST)}}		
	},
	
	.led_mlevel = {
		{LED_MODE_SHUTDOWN,	0},			
		{LED_MODE_RUNERROR,	0},			
		{LED_MODE_LOWBATT,	0},
		{LED_MODE_CHARGING,	0},	
		{LED_MODE_UNCONNECT,0},
		{LED_MODE_CONNECTED,0},
		{LED_MODE_POWERON,	0}
	},
	.led_mode = 0
};

/*******************************************************************************
 * 名称: vkLeds_Init
 * 功能: Led灯初始化函数
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkLeds_Init()
{
	/* GPIO引脚初始化 */
	for(int i=0; i<LEDMAX; i++)
	{
		GPIO_Init(gLeds.led_gpio[i].GPIOx, gLeds.led_gpio[i].GPIO_Pin, GPIO_Mode_Out_PP_High_Slow);
	}

	return 0;
}

/*******************************************************************************
 * 名称: vkLedsSet
 * 功能: 设置Led状态
 * 形参: state=0灯灭，state=1灯亮
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkLedsSet(uint8_t led, uint8_t state)
{
	if(led >=LEDMAX)
	{
		return -1;
	}
	
	int i = (int)led;
	
	(state == 0)?(GPIO_SetBits(gLeds.led_gpio[i].GPIOx, gLeds.led_gpio[i].GPIO_Pin)):(GPIO_ResetBits(gLeds.led_gpio[i].GPIOx, gLeds.led_gpio[i].GPIO_Pin));
	
	return 0;
}

int vkLedsSetState(uint8_t state)
{
	switch (state)
	{
		case LED_STATE_RED_OFF:
		{
			vkLedsSet(LEDR, 0);					
			break;
		}
		case LED_STATE_RED_ON:
		{
			vkLedsSet(LEDR, 1);
			vkLedsSet(LEDG, 0);
			break;
		}
		case LED_STATE_GREEN_OFF:
		{
			vkLedsSet(LEDG, 0);					
			break;
		}
		case LED_STATE_GREEN_ON:
		{
			vkLedsSet(LEDG, 1);
			vkLedsSet(LEDR, 0);
			break;
		}
		case LED_STATE_ALL_OFF: 
		{
			vkLedsSet(LEDG, 0);
			vkLedsSet(LEDR, 0);
			break;
		}
		case LED_STATE_ALL_ON:
		{
			vkLedsSet(LEDG, 1);
			vkLedsSet(LEDR, 1);
			break;
		}
		default:
			break;
	}

	return 0;
}

/*
 * 			设置LED指示灯模式
 *          time=-1 一直保持
 *			time=0 停止
 *			time>0 保持time秒时间
 */
int vkLedsSetMode(uint8_t mode, int16_t duration)
{
	int ret = 0;
	int i = 0;
	
	if(duration < -1)
	{
		return -1;
	}
	

	/* 优先级表 */
	for(i=0; i<LED_MODE_COUNT; i++)
	{
		if(mode == gLeds.led_mlevel[i].mode)
		{
			if(gLeds.led_mlevel[i].enable == 1 && (duration==-1 || duration>0))
				return -1;
			if(gLeds.led_mlevel[i].enable == 0 && duration==0)
				return -1;
			break;
		}
	}

	/* 暂停定时器 */
	vkTimerCancel(&gLeds.led_timer);

	/* 设置优先级表 */

	if(duration == -1 || duration > 0)		//一直保持
	{
		gLeds.led_mlevel[i].enable = 1;
	}
	else if(duration == 0)	//停止
	{
		gLeds.led_mlevel[i].enable = 0;
	}

	/* 设置模式表 */
	if(duration == -1 || duration == 0)
	{		
		for(int i=0; i<LED_MODE_COUNT; i++)
		{
			if(mode == gLeds.led_mtable[i].mode)
			{
				switch(mode)
				{
					case LED_MODE_CONNECTED:
					case LED_MODE_POWERON:
					case LED_MODE_SHUTDOWN:
					{
						gLeds.led_mtable[i].stat[1].led_stat = mode;
						gLeds.led_mtable[i].stat[1].led_time = 0;
						break;
					}
					case LED_MODE_UNCONNECT:
					case LED_MODE_CHARGING:
					case LED_MODE_LOWBATT:
					case LED_MODE_RUNERROR:
					case LED_MODE_TEST:
					{
						gLeds.led_mtable[i].stat[2].led_stat = mode;
						gLeds.led_mtable[i].stat[2].led_time = 0;
						break;
					}
				}
				
				break;
			}
		}
	}
	else if(duration > 0)
	{
		for(int i=0; i<LED_MODE_COUNT; i++)
		{
			if(mode == gLeds.led_mtable[i].mode)
			{
				switch(mode)
				{
					case LED_MODE_CONNECTED:
					case LED_MODE_POWERON:
					case LED_MODE_SHUTDOWN:
					{
						gLeds.led_mtable[i].stat[1].led_stat = duration + LED_MODE_COUNT;
						gLeds.led_mtable[i].stat[1].led_time = 0;
						break;
					}
					case LED_MODE_LOWBATT:
					case LED_MODE_RUNERROR:
					{
						gLeds.led_mtable[i].stat[2].led_stat = duration + LED_MODE_COUNT;
						gLeds.led_mtable[i].stat[2].led_time = 0;
						break;
					}					
					case LED_MODE_UNCONNECT:
					case LED_MODE_CHARGING:
					case LED_MODE_TEST:
					{
						gLeds.led_mtable[i].stat[2].led_stat = duration/2 + LED_MODE_COUNT;
						gLeds.led_mtable[i].stat[2].led_time = 0;
						break;
					}			
				}

				break;
			}
		}
	}

	/* 检查获取优先级最高模式 */
	vkLedsChekMode();
	vkLedsNextBout();

	/* 获取LED状态 */
	uint16_t led_stat, led_time;
	vkLedsGetStat(&led_stat, &led_time);

	/* 设置LED灯状态 */
	vkLedsSetState(led_stat);

	/* 重启定时器 */
	gLeds.led_timer.cb_ticks = vkMS_TO_TICKS(led_time);
	vkTimerInsert(&gLeds.led_timer);
	
	return ret;
}

int vkLedsGetMode(uint8_t mode)
{

	return 0;	
}

/*周期管理电池状态定时器回调函数 */
void vkLeds_Timer_Callback(void *pdata)
{
	uint16_t led_stat, led_time;
		
	/* 检查获取优先级最高模式 */
	vkLedsChekMode();
	
	/* 获取LED状态 */
	vkLedsGetStat(&led_stat, &led_time);

	/* 一个LED状态循环结束，无线循环/定时时间到/定时间未到 */
	if(led_time == 0)
	{		
		if(led_stat < LED_MODE_COUNT)
		{
			/* 无限循环，下一个循环 */
			vkLedsNextBout();

			/* 获取LED状态 */
			vkLedsGetStat(&led_stat, &led_time);

			/* 设置LED灯状态 */
			vkLedsSetState(led_stat);			
		}		
		/* 模式结束，下一个模式 */
		else if(led_stat == LED_MODE_COUNT)
		{	
			/* 切换模式 */
			vkLedsNextMode();

			vkLedsNextBout();
			
			/* 获取LED状态 */
			vkLedsGetStat(&led_stat, &led_time);

			/* 设置LED灯状态 */
			vkLedsSetState(led_stat);
		}
		else if(led_stat > LED_MODE_COUNT)
		{
			/* 定时内，下一个循环 */
			vkLedsNextBout();
			
			/* 获取LED状态 */
			vkLedsGetStat(&led_stat, &led_time);

			/* 设置LED灯状态 */
			vkLedsSetState(led_stat);			
		}
	}
	else if(led_time > 0)
	{
		/* 设置LED灯状态 */
		vkLedsSetState(led_stat);
	}
	
	gLeds.led_timer.cb_ticks = vkMS_TO_TICKS(led_time);
	vkTimerInsert(&gLeds.led_timer);
}

int vkLedsStart()
{
	uint16_t led_stat, led_time;
		
	/* 停止软定时器 */
	vkTimerCancel(&gLeds.led_timer);

	/* 检查获取优先级最高模式 */
	vkLedsChekMode();
	vkLedsNextBout();
	
	/* 获取LED状态 */
	vkLedsGetStat(&led_stat, &led_time);

	/* 设置LED灯状态 */
	vkLedsSetState(led_stat);

	/* 设置定时器回调，1000ms执行一次回调函数 */
	gLeds.led_timer.timer_name = (void *)&gLeds.led_timer;
	gLeds.led_timer.cb_func 	= vkLeds_Timer_Callback;
	gLeds.led_timer.cb_data 	= NULL;
	gLeds.led_timer.cb_ticks	= vkMS_TO_TICKS(led_time);

	/* 启动软定时器 */	
	if(vkTIMER_OK != vkTimerInsert(&gLeds.led_timer))
	{
		return -1;
	}

	return 0;	
}

/* 检查获取最高优先级模式 */
int vkLedsChekMode()
{	
	uint8_t mode_pre = gLeds.led_mode;
	
	for(int i=0; i<LED_MODE_COUNT; i++)
	{
		if(gLeds.led_mlevel[i].enable == 1)
		{
			gLeds.led_mode = gLeds.led_mlevel[i].mode;	
		
			goto EXIT;
		}
	}

	/* 不存在任何模式，强制转换成开机模式 */
	gLeds.led_mode = LED_MODE_POWERON;

EXIT:
	if(mode_pre != gLeds.led_mode)
	{
		/* 切换了模式 */
		vkLedsNextBout();
	}

	return 0;
}

int vkLedsNextMode()
{
	/* 关闭模式 */
	vkLedsSetMode(gLeds.led_mode, 0);

	/* 检查获取最高优先级模式 */
	vkLedsChekMode();

	/* 状态索引置0 */	
	vkLedsNextBout();
	
    return 0;
}

/* 同种模式下一个轮回 */
int vkLedsNextBout()
{
	/* 状态索引置0 */	
	gLeds.led_index = 0;
    
    return 0;
}

int vkLedsGetStat(uint16_t *led_stat, uint16_t *led_time)
{
	for(int i=0; i<LED_MODE_COUNT; i++)
	{
		if(gLeds.led_mode == gLeds.led_mtable[i].mode)
		{
			*led_stat = gLeds.led_mtable[i].stat[gLeds.led_index].led_stat;
			*led_time = gLeds.led_mtable[i].stat[gLeds.led_index].led_time;
			if(*led_time == 0 && *led_stat > LED_MODE_COUNT)
			{
				gLeds.led_mtable[i].stat[gLeds.led_index].led_stat--;
			}

			gLeds.led_index++;
			
			return 0;
		}
	}

	*led_stat = 0;
	*led_time = 0;
		
	return -1;
}

