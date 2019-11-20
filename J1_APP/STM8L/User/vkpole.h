
/******************************************************************************
 * 文件  ：vkpole.h
 * 描述    ：设备系统状态
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKPOLE_H
#define __VKPOLE_H

/* 包含系统头文件 */
#include "stm8l15x.h"
#include "vksofttimer.h"
#include "vkpluse.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */

#define POLE_MODE_ONLYONCE PLUSE_MODE_ONLYONCE		//单个模式
#define POLE_MODE_CONTINUE PLUSE_MODE_CONTINUE		//循环模式	

enum POLE_DEVICE
{
	POLE_A	= 1,	//电极A,对应pluse0
	POLE_B 	= 2,	//电极B,对应pluse1
};

typedef struct POLE 
{
	uint8_t device[2];			//电极A和B对应的脉冲
	uint8_t enable[2];			//电极AB使能
	uint8_t linked[2];			//电极是接上
	
	uint16_t trained[2];		//训练时间，仅静态模式
	vkTIMER timer_trained[2];	//电极训练时间定时器，仅静态模式
	
	uint16_t delayed[2];		//电极启动延时时间，    仅运动模式
	vkTIMER timer_delayed[2];	//电极启动延时定时器，仅运动模式
	uint16_t expired;			//电极运行关闭时间，仅运动模式
	vkTIMER timer_expired;		//电极运行关闭定时器，仅运动模式	

} vkPOLE;

/* 电极接口函数 */
int vkPole_Init(void);
int vkPole_Deinit(void);

int vkPoleSetMode(uint8_t pole, uint8_t mode);
int vkPoleSetExpt(uint8_t pole, uint8_t expect);
int vkPoleSetTime(uint8_t pole, uint16_t duration, uint16_t interval);
int vkPoleSetDelayed(uint8_t pole, uint16_t delay);
int vkPoleSetExpired(uint16_t expire);
int vkPoleSetTrained(uint8_t pole, uint16_t train);


int vkPoleStart(uint8_t pole);
int vkPoleStop(uint8_t pole);
int vkPoleGetIdle(void);

int vkPoleTriger(uint8_t pole);

uint8_t vkPoleGetEnable(uint8_t pole);
uint8_t vkPoleGetLinked(uint8_t pole);
uint8_t vkPoleGetExpect(uint8_t pole);
uint8_t vkPoleGetPoqlvl(uint8_t pole);

#endif
