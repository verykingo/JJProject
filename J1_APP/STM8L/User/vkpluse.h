
/******************************************************************************
 * 文件  ：vkpluse.h
 * 描述    ：STM8L Pluse
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKPLUSE_H
#define __VKPLUSE_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "vkpwm.h"

#define PLUSE_TICK_TIME 40	//40us

/* 自定义数据类型 */
typedef struct pluse
{
	uint8_t  p_mode;		//模式一次/循环
	uint8_t  p_enable;		//是否使能
	uint8_t  p_expert;		//预设电流mA
	uint8_t  p_current;		//当前电流mA
	uint8_t  p_duration;	//持续时长
	uint8_t  p_interval;	//间隔时长
	uint8_t  p_poqlevel;	//上层状态
	uint8_t  p_bodlevel;	//下层状态
	uint16_t p_poqcount;	//上层计数
	uint16_t p_bodcount;	//下层计数
	
	vkPWM pwm;
} vkPLUSE;

void vkPluseTick(void);

#endif
