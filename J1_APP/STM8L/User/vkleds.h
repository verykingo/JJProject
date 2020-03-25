
/******************************************************************************
 * 文件  ：vkleds.h
 * 描述    ：led指示灯
 * 平台    ：STM8L
 * 时间  ：2019-09-04

*******************************************************************************/

#ifndef __VKLEDS_H
#define __VKLEDS_H

/* 包含系统头文件 */
#include "vksofttimer.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */

typedef void (*vkLEDS_MODE_CB)(void *);

typedef struct
{
	GPIO_TypeDef* GPIOx; 
	uint8_t GPIO_Pin;
} vkLEDGPIO;

enum LED
{
	LED0 = 0,
	LED1,
	LEDMAX
};

#define LEDG LED0
#define LEDR LED1

enum LEDSTATE
{
	LED_STATE_RED_OFF 		= 0x00,
	LED_STATE_RED_ON 		= 0x01,
	LED_STATE_GREEN_OFF 	= 0x02,
	LED_STATE_GREEN_ON 		= 0x03,
	LED_STATE_ALL_OFF 		= 0x04,
	LED_STATE_ALL_ON 		= 0x05,
};
	
/* LED状态结构体 */
typedef struct
{
	uint16_t 	led_stat;	//led状态
	uint16_t 	led_time;	//led时间
} vkLEDSTAT;

enum LEDMODE
{
	LED_MODE_CONNECTED = 0,	//蓝牙已连接：绿灯常亮
	LED_MODE_UNCONNECT,		//蓝牙未连接：绿灯闪烁，1s
	LED_MODE_CHARGING,		//电池正充电：红灯闪烁，1s
	LED_MODE_LOWBATT,		//电池低电量：红灯闪烁，0.5s
	LED_MODE_POWERON,		//开机:	红绿灯常量	
	LED_MODE_SHUTDOWN,		//关机：红绿灯灭
	LED_MODE_RUNERROR,		//运行出错：红绿灯交替闪烁，0.5s
	LED_MODE_TEST,			//测试：红绿灯交替闪烁,1s
	LED_MODE_COUNT			//模式总数
};

typedef struct LEDLEVEL
{
	uint8_t 	mode;		//模式
	uint8_t 	enable;		//使能
} vkLEDLEVEL;

typedef struct
{
	uint8_t	  mode;			//模式
	vkLEDSTAT stat[3];		//状态
} vkLEDMSTAT;


/* LED状态结构体 */
typedef struct
{
	vkLEDGPIO	led_gpio[LEDMAX];
	
	vkLEDMSTAT 	led_mtable[LED_MODE_COUNT];
	vkLEDLEVEL	led_mlevel[LED_MODE_COUNT];
	
	uint8_t		led_mode;
	uint8_t 	led_index;

	vkTIMER		led_timer;
} vkLEDS;

int vkLeds_Init();
int vkLedsSet(uint8_t led, uint8_t state);

int vkLedsStart();
int vkLedsStop();

int vkLedsSetMode(uint8_t mode, int16_t duration);
int vkLedsGetMode(uint8_t mode);

int vkLedsChekMode();
int vkLedsNextMode();
int vkLedsNextBout();

int vkLedsGetStat(uint16_t *led_stat, uint16_t *led_time);

#endif
