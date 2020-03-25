
/******************************************************************************
 * 文件  ：vkrtclock.h
 * 描述    ：STM8L 时钟
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VRTCLOCk_H
#define __VRTCLOCk_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */
typedef struct timestamp
{
	uint32_t	ss;			/* 秒 */
	uint16_t	ms;			/* 毫秒 */
} vkTIMESTAMP;

typedef struct calendar
{
	uint16_t 	year;		/* 年 */
	uint8_t 	month;		/* 月 */
	uint8_t 	day;		/* 日 */
	uint8_t 	weekday;	/* 星期 */
	uint8_t 	hour;		/* 小时 */
	uint8_t 	minute;		/* 分钟 */
	uint8_t 	second;		/* 秒钟 */
	uint16_t 	msecond;	/* 毫秒 */
} vkCALENDAR;

vkTIMESTAMP vkTimestampAdd(vkTIMESTAMP a, vkTIMESTAMP b);
vkTIMESTAMP vkTimestampSub(vkTIMESTAMP a, vkTIMESTAMP b);
vkTIMESTAMP vkTimestampGen(uint32_t ms);

int vkTimestampCmp(vkTIMESTAMP a, vkTIMESTAMP b);
int vkTimestampCpy(vkTIMESTAMP a, vkTIMESTAMP b);


int vkRtclock_Init();

int vkRtclockSetTime(uint8_t hour, uint8_t minute, uint8_t second);
int vkRtclockGetTime(uint8_t *hour, uint8_t *minute, uint8_t *second);

int vkRtclockSetDate(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday);
int vkRtclockGetDate(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *weekday);

int vkRtclockSetMsec(uint16_t msecond);
int vkRtclockGetMsec(uint16_t *msecond);

int vkRtclockDlyHMSM(uint8_t hour, uint8_t minute, uint8_t second, uint16_t msecond);

int vkRtclockSetAlarm(uint8_t hour, uint8_t minute, uint8_t second, uint16_t msecond);
int vkRtclockStartAlarm(void);
int vkRtclockStopAlarm(void);

int vkRtclockGetCalendar(vkCALENDAR *calendar);
int vkRtclockGetTimestamp(vkTIMESTAMP* timestamp);

int vkRtclockCalendar2Timestamp(vkTIMESTAMP* timestamp, vkCALENDAR calendar);
int vkRtclockTimestamp2Calendar(vkCALENDAR* calendar, vkTIMESTAMP timestamp);


int vkRtclockRefresh();

#endif
