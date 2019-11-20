/******************************************************************************
 * 文件  ：vkpole.c
 * 描述    ：设备电极
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include "stdio.h"
#include "string.h"

/* 包含自定义头文件 */
#include "vkpole.h"
#include "vksysrun.h"
#include "vkpower.h"
#include "vkpluse.h"
#include "vkpole.h"
#include "vktimetick.h"
#include "vktly.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */
static vkPOLE gPole =
{
	.device = {PLUSE0, PLUSE1},
	.enable = {0, 0},
	.linked = {0, 0},
	.trained = {0, 0},
	.delayed = {0, 0},
	.expired = 0
};

/*******************************************************************************
 * 名称: vkPole_Init
 * 功能: 电极初始化
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkPole_Init(void)
{
	return vkPluse_Init();
}

/*******************************************************************************
 * 名称: vkPole_Deinit
 * 功能: 电极初销毁
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkPole_Deinit(void)
{
	return vkPluse_Deinit();
}

/*******************************************************************************
 * 名称: vkPoleSetMode
 * 功能: 设置电极模式
 * 形参: mode单个模式，循环模式
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkPoleSetMode(uint8_t pole, uint8_t mode)
{
	int index = pole-1;
	return vkPluseSetMode(gPole.device[index], mode);
}

/*******************************************************************************
 * 名称: vkPoleSetExpt
 * 功能: 设置电极电流强度mA
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkPoleSetExpt(uint8_t pole, uint8_t expect)
{
	int index = pole-1;
	return vkPluseSetExpt(gPole.device[index], expect);
}

/*******************************************************************************
 * 名称: vkPoleSetTime
 * 功能: 设置电极脉冲持续时间和间隔时间
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 设置成功后，立即生效，时间单位是ms
 ******************************************************************************/
int vkPoleSetTime(uint8_t pole, uint16_t duration, uint16_t interval)
{
	int index = pole-1;
	return vkPluseSetTime(gPole.device[index], duration, interval);
}

/*******************************************************************************
 * 名称: vkPoleSetTrained
 * 功能: 静态模式下设置训练时间
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 时间单位是s，5min~30min（5min/step）（300s~1800s）
 ******************************************************************************/
int vkPoleSetTrained(uint8_t pole, uint16_t train)
{
	int index = pole-1;

	gPole.trained[index] = train;
	
	return 0;
}

/*******************************************************************************
 * 名称: vkPoleSetDelayed
 * 功能: 动态模式下设置触发后延时时间
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 时间单位是ms，0ms~1000ms（100ms/step）
 ******************************************************************************/
int vkPoleSetDelayed(uint8_t pole, uint16_t delay)
{
	int index = pole-1;

	gPole.delayed[index] = delay;
	
	return 0;
}

/*******************************************************************************
 * 名称: vkPoleSetExpired
 * 功能: 动态模式下设置停止时间
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 时间单位是s，0.5h/1h/2h/3h/4h，（180s~14400s）
 ******************************************************************************/
int vkPoleSetExpired(uint16_t expire)
{
	gPole.expired = expire;
	
	return 0;
}

/* 电极下层脉冲都处于低电平 */
int vkPoleGetIdle(void)
{
	return vkPluseGetIdle();
}

void vkPole_Timer_Delayed_Callback(void *pdata)
{
    uint8_t pluse = *((uint8_t *)pdata);

	/* 先判是否是正在脉冲刺激 */
	if(PLUSE_LEVEL_LOW == vkPluseGetPoqL(pluse))
	{
		vkPluseStart(pluse);
	}
}

/*******************************************************************************
 * 名称: vkPoleTriger
 * 功能: 单个模式下，触发一次脉冲刺激
 * 形参: 电极pole
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
inline int vkPoleTriger(uint8_t pole)
{
	int index = pole-1;

	/* 是否使能 */
	if(gPole.enable[index] == 0)
	{
		return -1;
	}	

	/* 是否有延时 */
	if(gPole.delayed[index] == 0)
	{
		/* 先判是否是正在脉冲刺激 */
		if(PLUSE_LEVEL_LOW ==vkPoleGetPoqlvl(pole))
		{
			/* 触发条件满足后，立即产生脉冲刺激 */
			vkPluseStart(gPole.device[index]);
		}
	}
	else
	{
		/* 触发条件满足后，延时产生脉冲刺激 */
		/* 启动定时器 */
		vkTimerCancel(&gPole.timer_delayed[index]);
		
		gPole.timer_delayed[index].timer_name = &gPole.timer_delayed[index];
		gPole.timer_delayed[index].cb_func    = vkPole_Timer_Delayed_Callback;
		gPole.timer_delayed[index].cb_data    = (void *)&gPole.device[index];
		gPole.timer_delayed[index].cb_ticks   = vkMS_TO_TICKS(gPole.delayed[index]);
		
		vkTimerInsert(&gPole.timer_delayed[index]);		
	}	
    
    return 0;
}

/* 电极是否工作 */
uint8_t vkPoleGetEnable(uint8_t pole)
{
	int index = pole -1;

	return gPole.enable[index];
}

/* 电极是否接上 */
uint8_t vkPoleGetLinked(uint8_t pole)
{
	int index = pole -1;

	return gPole.linked[index];
}

uint8_t vkPoleGetExpect(uint8_t pole)
{
	int index = pole-1;
	
	return vkPluseGetExpt(gPole.device[index]);
}

uint8_t vkPoleGetPoqlvl(uint8_t pole)
{
	int index = pole-1;

	return vkPluseGetPoqL(gPole.device[index]);
}

void vkPole_Timer_expired_Callback(void * pdata)
{
	vkPoleStop(POLE_A);
	vkPoleStop(POLE_B);
}

void vkPole_Timer_Trained_Callback(void * pdata)
{
	uint8_t pole = *((uint8_t *)pdata) + 1;
		
	vkPoleStop(pole);
}

int vkPoleStart(uint8_t pole)
{
	int index = pole-1;

	uint8_t device_mode = vkDeviceModeGet();
	
	/* 运动模式 */
	if(device_mode == DEVICE_MODE_WALKING)
	{
		/*启动脉冲Tick */
		vkTimeTickStart(TIME5);
		
		/* 打开电极A和B供电 */
		vkPowerPoleASet(1);
		vkPowerPoleBSet(1);

		/* 启动陀螺仪 */
		vkTlyStart();

		/* 延时10ms，保证陀螺仪到正确读数 */
		DelayMS(10);

		/* 电极使能 */
		gPole.enable[0] = 1;
		gPole.enable[1] = 1;

		/* 启动生命时间定时器 */
		if(gPole.expired != 0)
		{
			/* 启动定时器 */
			vkTimerCancel(&gPole.timer_expired);
			
			gPole.timer_expired.timer_name = &gPole.timer_expired;
			gPole.timer_expired.cb_func    = vkPole_Timer_expired_Callback;
			gPole.timer_expired.cb_data	   = NULL;
			gPole.timer_expired.cb_ticks   = vkSS_TO_TICKS(gPole.expired);

			vkTimerInsert(&gPole.timer_expired);
		}
		
		nop();
	}
	/* 静态模式 */
	else if(device_mode == DEVICE_MODE_TRAINING)	
	{
		if(gPole.enable[0]==0 && gPole.enable[1]==0)
		{
			/*启动脉冲Tick */
			vkTimeTickStart(TIME5);			
		}
		
		/* 打开电极模块供电 */
		if(pole == POLE_A)
		{
			vkPowerPoleASet(1);
		}
		else if(pole == POLE_B)
		{
			vkPowerPoleBSet(1);
		}

		/* 启动脉冲，静态模式下无延时时间，有训练时间 */
		gPole.enable[index] = 1;		
		vkPluseStart(gPole.device[index]);

		/* 启动训练时间定时器 */
		if(gPole.trained[index] != 0)
		{
			/* 启动定时器 */
			vkTimerCancel(&gPole.timer_trained[index]);
			
			gPole.timer_trained[index].timer_name = &gPole.timer_trained[index];
			gPole.timer_trained[index].cb_func    = vkPole_Timer_Trained_Callback;
			gPole.timer_trained[index].cb_data	  = (void *)&gPole.device[index];
			gPole.timer_trained[index].cb_ticks   = vkSS_TO_TICKS(gPole.trained[index]);

			vkTimerInsert(&gPole.timer_trained[index]);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int vkPoleStop(uint8_t pole)
{
	int index = pole-1;
		
	uint8_t device_mode = vkDeviceModeGet();

	/* 运动模式 */
	if(device_mode == DEVICE_MODE_WALKING)
	{
		/* 停止脉冲输出 */
		gPole.enable[0] = 0;
		gPole.enable[1] = 0;
		vkPluseStop(gPole.device[0]);
		vkPluseStop(gPole.device[1]);

		/* 关闭电极模块供电 */
		vkPowerPoleASet(0);
		vkPowerPoleBSet(0);		

		/* 停止脉冲Tick */
		vkTimeTickStop(TIME5);
		
		/* 停止陀螺仪 */
		vkTlyStop();

		/* 停止定延时/截止时器 */
		vkTimerCancel(&gPole.timer_delayed[0]);
		vkTimerCancel(&gPole.timer_delayed[1]);
		vkTimerCancel(&gPole.timer_expired);
	}
	/* 静态模式 */
	else if(device_mode == DEVICE_MODE_TRAINING)
	{
		/* 设置电极停止，停止脉冲 */
		gPole.enable[index] = 0;
		vkPluseStop(gPole.device[index]);

		/* 关闭电极模块供电 */
		if(pole == POLE_A)
		{
			vkPowerPoleASet(0);
		}
		else if(pole == POLE_B)
		{
			vkPowerPoleBSet(0);
		}

		/* 停止训练定时器 */
		vkTimerCancel(&gPole.timer_trained[0]);
		vkTimerCancel(&gPole.timer_trained[1]);

		/* 如果两个电极都停止          */
		if(gPole.enable[0] == 0 && gPole.enable[1] == 0)
		{
			/* 停止脉冲Tick */
			vkTimeTickStop(TIME5);
			nop();
		}
	}
	else
	{
		return -1;
	}

	return 0;
}


