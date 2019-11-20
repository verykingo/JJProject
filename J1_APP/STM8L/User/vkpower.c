/******************************************************************************
 * 文件  ：vkpower.c
 * 描述    ：FootDrop电源管理
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "vkpower.h"
#include "vksofttimer.h"
#include "vktimetick.h"
#include "vksysrun.h"
#include "vkleds.h"
#include "vkbattery.h"
#include "BltProtocol.h"
#include "vkpole.h"
#include "vktly.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */
static vkTIMER gPower_Timer;	//开机按键长按计时定时器

/*******************************************************************************
 * 名称: vkPower_Init
 * 功能: 陀螺仪模块、蓝牙模块、电极A和电极B脉冲发生模块供电开关
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无 
 ******************************************************************************/
int vkPower_Init(void)
{
	/* 模块GPIO初始化 GPIO_Mode_Out_PP_Low_Fast */
	GPIO_Init(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);	
	GPIO_Init(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);

	/* 模块不供电 */
	GPIO_ResetBits(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN);
	GPIO_ResetBits(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN);	
	GPIO_ResetBits(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN);
	GPIO_ResetBits(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN);

	/* 开机按键初始化 */
	GPIO_Init(BTN_SWITCH_IO_PORT,BTN_SWITCH_PIN,GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);
	ITC_SetSoftwarePriority(EXTI2_IRQn, ITC_PriorityLevel_3);	

	return 0;
}

/*******************************************************************************
 * 名称: vkPowerBLESet
 * 功能: 设置蓝牙模块供电
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无PC5 
 ******************************************************************************/
int vkPowerBLESet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN):GPIO_SetBits(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * 名称: vkPowerTlySet
 * 功能: 设置陀螺仪供电
 * 形参: 
 * 返回: 成功0
 * 说明: 无 
 ******************************************************************************/
int vkPowerTLYSet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN):GPIO_SetBits(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * 名称: vkPowerPoleASet
 * 功能: 设置电极A供电
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无
 ******************************************************************************/
int vkPowerPoleASet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN):GPIO_SetBits(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * 名称: vkPowerPoleBSet
 * 功能: 设置电极B供电
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无
 ******************************************************************************/
int vkPowerPoleBSet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN):GPIO_SetBits(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * 名称: vkPowerBTNGet
 * 功能: 获取开关按键状态
 * 形参: 
 * 返回: 按键状态
 * 说明: 无 
 ******************************************************************************/
uint8_t vkPowerBTNGet()
{	
	return (GPIO_ReadInputDataBit(BTN_SWITCH_IO_PORT, BTN_SWITCH_PIN) == RESET)?0:1;
}


/*
 * 电源开关长按定时器回调函数
 */
void vkPower_Timer_Callback(void *pdata)
{
	/* 此时已退出halt模式 */
	
	/* 持续按下开机按键2秒后 */
	if(vkPowerBTNGet() == 0)
	{
		/* 关机状态下，则开机 */
		if(vkDeviceWorkGet() == 0)
		{
			/* 设置开机状态 */
			vkDeviceWorkSet(1);

			/* LED指示灯亮 */
			vkLedsSet(LED0, 1);
			vkLedsSet(LED1, 1);	
			vkLedsStart();

			/* 蓝牙模块供电 */
			vkPowerBLESet(1);
			DelayMS(10);
			
			/* 蓝牙模块初始化 */
			if(0 != Blt_Init())
			{
				/* 蓝牙模块初始化失败，检测串口连接 */
				/* 指示灯提示并系统关机 */	
				vkPowerInitError(0, 10);
				WWDG_SWReset();				
			}			
			/* 启动蓝牙模块通信 */
			Blt_CommunicationStart();
			
			/* 设备未连接蓝牙APP */
			vkDeviceBlueSet(DEVICE_BLUE_UNCONNECT);

			/* 启动陀螺仪供电 */
			vkPowerTLYSet(1);
			DelayMS(10);
			
			/* 陀螺仪初始化 */
			if(0 != vkTly_Init())
			{
				/* 指示灯提示 */
				vkPowerInitError(1, 10);
			}
			
			/* 启动电池管理 */
			vkBatteryStart();
		}
		/* 开机状态下。则关机 */
		else
		{
			/* 设置关机状态 */
			vkDeviceWorkSet(0);
			
			/* 蓝牙模块供电 */
			vkPowerBLESet(0);

			/* 停止电极A和B脉冲 */
			vkPoleStop(POLE_A);
			vkPoleStop(POLE_B);

			/* LED指示灯灭 */
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);
			
			/* 没有充电，系统复位重启 */
			if(vkBatteryCharge() == 0)
			{
				WWDG_SWReset();
			}
			//系统正在充电，则不系统复位重启，等到充电插头拔出再系统复位重启
			else
			{
				//do nothing
			}			
		}
	}
	/* 持续按下开机按键小于2秒，则无任何操作 */
	else
	{
		//do nothing
	}
}

/*
 * 电源开关按下中断回调处理函数
 */
void vkPower_Interrupt_Callback()
{	
	/* 先删除当前定时器 */
	vkTimerCancel(&gPower_Timer);
	
	gPower_Timer.timer_name = &gPower_Timer;
	gPower_Timer.cb_func 	= vkPower_Timer_Callback;
	gPower_Timer.cb_data 	= NULL;
	gPower_Timer.cb_ticks 	= vkSS_TO_TICKS(2);
	
	vkTimerInsert(&gPower_Timer);

}

int vkPowerInitError(uint8_t error, uint16_t duration)
{
	/* 红绿灯交替0.5s闪烁 */
	if(error == 0)
	{
		for(int i=0; i<duration; i++)
		{
			vkLedsSet(LED0, i%2);
			vkLedsSet(LED1, (i+1)%2);
			DelayMS(500);
			
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);		
			DelayMS(500);
		}
	}
	else if(error == 1)
	{
		for(int i=0; i<duration/2; i++)
		{
			vkLedsSet(LED0, i%2);
			vkLedsSet(LED1, (i+1)%2);
			DelayMS(500);
			
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);		
			DelayMS(500);

			vkLedsSet(LED0, i%2);
			vkLedsSet(LED1, (i+1)%2);
			DelayMS(500);
			
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);		
			DelayMS(500);
		}		
	}
	
	return 0;
}

