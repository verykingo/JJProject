
/******************************************************************************
 * �ļ�  ��vkrtclock.h
 * ����    ��STM8L ʱ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VRTCLOCk_H
#define __VRTCLOCk_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */
typedef struct timestamp
{
	uint32_t	ss;			/* �� */
	uint16_t	ms;			/* ���� */
} vkTIMESTAMP;

typedef struct calendar
{
	uint16_t 	year;		/* �� */
	uint8_t 	month;		/* �� */
	uint8_t 	day;		/* �� */
	uint8_t 	weekday;	/* ���� */
	uint8_t 	hour;		/* Сʱ */
	uint8_t 	minute;		/* ���� */
	uint8_t 	second;		/* ���� */
	uint16_t 	msecond;	/* ���� */
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
