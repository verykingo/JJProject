
/******************************************************************************
 * �ļ�  ��vkrtclock.c
 * ����    ��RTCʵʱʱ�ӣ�ʹ���ڲ�LSIʱ��38KHz�������ṩ��ȡ���뺯��vkRtclockMktime()
           ����ʱ����vkRtclockDlyHMSM()��      ϵͳ��ʼ��ʱ��1970-01-01:00:00:00 �����ġ�
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01
*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

/* �����Զ���ͷ�ļ� */
#include "vkrtclock.h"

/* �Զ��������� */

/* �Զ���� */
#define ASYNCH_PREDIV			37		//�첽��Ƶ��������RTC����Ƶ���й�
#define SYNCH_PREDIV			999		//ͬ����Ƶ��������RTC����Ƶ���й�
#define SUBSECOND_FREQUENCY		(SYNCH_PREDIV+1)	//����Ƶ��	

#define SINDAY		24ul*60ul*60ul	//һ������
#define SINHOUR		60ul*60ul		//Сʱ����
#define SINMINUTE	60ul			//��������


/* ȫ�ֱ������� */
static uint8_t rtclock_century = 19;	//ϵͳĬ��19����	

/* ĳ���е����� */
const static uint8_t rtclock_dim[2][12] = 
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},		//ƽ��2��28
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},		//����2��29
};

/* ĳ���е����� */
const static uint32_t rtclock_sim[2][12] = 
{
	{31ul*SINDAY, 28ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY},		//ƽ��2��28
	{31ul*SINDAY, 29ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY},		//����2��29
};

/* ƽ��365������366�� */
const static uint16_t rtclock_diy[2] = 
{365, 366};

/* ƽ��365������366������� */
const static uint32_t rtclock_siy[2] = 
{365ul*SINDAY, 366ul*SINDAY};


/* ƽ��/���굽ĳ�µ��������� */
const static uint16_t rtclock_dtm[2][12] = 
{
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},	//ƽ��2��28/365
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},	//����2��29/366
};

/* ���� */
const static uint8_t rtclock_week[7] = 
{4, 5, 6, 7, 1, 2, 3};


/* �ж��Ƿ������꣬���귵��1��ƽ�귵��0 */
static inline uint8_t is_leap_year(uint16_t year)
{
    return ((!(year % 4) && (year % 100)) || !(year % 400));
}

/* ����ĳ��ĳ�µ�������month=1~12 */
static inline uint8_t days_in_month(uint8_t month, uint16_t year)
{
    return rtclock_dim[is_leap_year(year)][month-1];
}

/* ��ֹĳ���ܹ������������������� */
static inline uint32_t days_to_year(uint16_t year)
{	
	/* year��ǰһ�� */
	year = year - 1;

	/* ������ */
    uint16_t leap_year_num = year/4 - year/100 + year/400;

	/* �ܼ����� */
    uint32_t days = year*365ul + leap_year_num;

	return days;
}

/* ��year���꣬��ֹmonth�µ�������month=1~12 */
static inline uint32_t days_to_month(uint16_t year, uint8_t month)
{	
	return rtclock_dtm[is_leap_year(year)][month-1];
}

/*******************************************************************************
 * ����: vkRtclock_Init
 * ����: ʵʱʱ�ӳ�ʼ�����ڲ�ʱ��LSI=38KHz��ͬ��ʱ��Ƶ��1000Hz
 * �β�: ��
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclock_Init()
{
	/* Configures the RTC clock (38KHz) */
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);

	/* Enable the RTC clock */
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
	 
	/* Deinitializes the RTC */
	RTC_DeInit();

	/* 1Hz = 1MHz/(AsynchPrediv+1)*(SynchPrediv+1) */
	RTC_InitTypeDef init = 
	{
		.RTC_HourFormat 	= RTC_HourFormat_24,
		.RTC_AsynchPrediv	= ASYNCH_PREDIV,	//�첽��Ƶ38=37+1
		.RTC_SynchPrediv 	= SYNCH_PREDIV		//ͬ����Ƶ1000=999+1
	};
		
	if(SUCCESS != RTC_Init(&init))
	{	
		printf("%s rtc init!\r\n", __FUNCTION__);
		return -1;
	}

	RTC_WriteProtectionCmd(DISABLE);
	RTC_EnterInitMode();

	/* ����ʱ��00:00:00 */
	RTC_TimeTypeDef time =
	{
		.RTC_Hours 		= 0,
		.RTC_Minutes 	= 0,
		.RTC_Seconds 	= 0,
		.RTC_H12 		= RTC_H12_AM,
	};
	RTC_SetTime(RTC_Format_BIN, &time);

	/* ��������1970-01-01     ������*/
	RTC_DateTypeDef date =
	{
		.RTC_WeekDay 	= RTC_Weekday_Thursday,
		.RTC_Month 		= RTC_Month_January,
		.RTC_Date 		= 1,
		.RTC_Year 		= 70,
	};
  	RTC_SetDate(RTC_Format_BIN, &date);

	RTC_ExitInitMode();
	RTC_WriteProtectionCmd(ENABLE);
	
	printf("%s Set Date:1970-01-01:00:00:00 \r\n", __FUNCTION__);

	/* ͬ��ʱ�� */
	RTC_WaitForSynchro();

	#if 0	//�����⣬���haltģʽ����
	/****************�����������*************************/
	/* Disable RTC alarm */
	RTC_AlarmCmd(DISABLE);

	/* ��ʼ����ʱ��00h:00mn:00sec���ر��������ڹ��� */
	RTC_AlarmTypeDef alarm =
	{
		.RTC_AlarmTime = 
		{
			.RTC_H12 = RTC_H12_AM,
			.RTC_Hours = 0,
			.RTC_Minutes = 0,
			.RTC_Seconds = 0,
		},
		/* ��Ч���� */
		.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date,
  		.RTC_AlarmDateWeekDay 		= 1,
		/* ���ʱ��Ч������������Ч */
		.RTC_AlarmMask 	= RTC_AlarmMask_DateWeekDay,
	};

	/* ���ò��� */
	RTC_SetAlarm(RTC_Format_BIN, &alarm);

	/* ��ʼ������(����)0ssec */
	RTC_AlarmSubSecondConfig(0, RTC_AlarmSubSecondMask_None);

	/* ʹ�������ж� */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	#endif
	
	return 0;
}

/*******************************************************************************
 * ����: vkRtclockSetTime
 * ����: ����ʱ��
 * �β�: Сʱhour=0~23������minute=0~59����second=0~59
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockSetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	if(hour > 24 || minute > 60 || second > 60)
	{
		/* 0~23 */ /* 0~59 */ /* 0~59 */
		return -1;
	}
	
	RTC_WriteProtectionCmd(DISABLE);
	RTC_EnterInitMode();

	/* ����ʱ�� */
	RTC_TimeTypeDef time =
	{
		.RTC_Hours 		= hour,
		.RTC_Minutes 	= minute,
		.RTC_Seconds 	= second,
		.RTC_H12 		= RTC_H12_AM,
	};
	RTC_SetTime(RTC_Format_BIN, &time);

	RTC_ExitInitMode();
	RTC_WriteProtectionCmd(ENABLE);
	
	/* ͬ��ʱ�� */
	RTC_WaitForSynchro();

	return 0;
}

/*******************************************************************************
 * ����: vkRtclockGetTime
 * ����: ��ȡʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockGetTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	RTC_TimeTypeDef time;
	
	RTC_GetTime(RTC_Format_BIN, &time);
	
	*hour   = time.RTC_Hours;
	*minute = time.RTC_Minutes;
	*second = time.RTC_Seconds;

	return 0;
}

/*******************************************************************************
 * ����: vkRtclockSetDate
 * ����: ����ʱ��
 * �β�: ��year����month=1~12����day=1~31������weekday=1~7
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockSetDate(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday)
{
	rtclock_century = year/100;
	
	if(month > 13 || month == 0)
		return -1;
	if(day > days_in_month(month, year) || day == 0)
		return -1;
	if(weekday > 8 || weekday == 0)
		return -1;
	
	RTC_WriteProtectionCmd(DISABLE);
	RTC_EnterInitMode();

	/* �������� */
	RTC_DateTypeDef date =
	{
		.RTC_WeekDay 	= (RTC_Weekday_TypeDef)weekday,
		.RTC_Month 		= (RTC_Month_TypeDef)month,
		.RTC_Date 		= day,
		.RTC_Year 		= year%100,
	};
  	RTC_SetDate(RTC_Format_BIN, &date);


	RTC_ExitInitMode();
	RTC_WriteProtectionCmd(ENABLE);
	
	/* ͬ��ʱ�� */
	RTC_WaitForSynchro();

	return 0;
}

/*******************************************************************************
 * ����: vkRtclockGetDate
 * ����: ��ȡ����
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockGetDate(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *weekday)
{
	/* �������� */
	RTC_DateTypeDef date;
	
  	RTC_GetDate(RTC_Format_BIN, &date);

	*year = date.RTC_Year + rtclock_century*100ul;
	*month = date.RTC_Month;
	*day = date.RTC_Date;
	*weekday = date.RTC_WeekDay;
	
	return 0;
}

/*******************************************************************************
 * ����: vkRtclockGetMsec
 * ����: ��ȡ����
 * �β�: ����ָ��
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockGetMsec(uint16_t *msecond)
{
	*msecond = ((SYNCH_PREDIV-RTC_GetSubSecond())*1000ul)/SUBSECOND_FREQUENCY;
	
	return 0;
}

/*******************************************************************************
 * ����: vkRtclockDlyHMSM
 * ����: ʵʱʱ��ʵ����ʱ
 * �β�: Сʱh������m����s������ms
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockDlyHMSM(uint8_t hour, uint8_t minute, uint8_t second, uint16_t msecond)
{
	if(minute>59 || second>59 || msecond>999)
	{
		printf("%s param error!\r\n", __FUNCTION__);
		return 0;
	}

	vkTIMESTAMP t;
	vkRtclockGetTimestamp(&t);

	vkTIMESTAMP d = vkTimestampAdd(t,vkTimestampGen((hour*SINHOUR+minute*SINMINUTE+second)*1000ul+msecond));

	while(vkTimestampCmp(t,d)==-1)
	{
		vkRtclockGetTimestamp(&t);
	}
	
	return 0;
}

/*******************************************************************************
 * ����: vkRtclockRefresh
 * ����: ˢ��ʵʱʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockRefresh()
{
	/* haltģʽ��������ˢ��ʱ�� */
	RTC_WaitForSynchro();
	
	return 0;
}

/*******************************************************************************
 * ����: vkRtclockSetAlarm
 * ����: ��������ʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockSetAlarm(uint8_t hour, uint8_t minute, uint8_t second, uint16_t msecond)
{
	RTC_AlarmTypeDef alarm;
	/* ʱ�� */
	alarm.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	alarm.RTC_AlarmTime.RTC_Hours = hour;
	alarm.RTC_AlarmTime.RTC_Minutes = minute;
	alarm.RTC_AlarmTime.RTC_Seconds = second;

	/* ����/���� */
  	alarm.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  	alarm.RTC_AlarmDateWeekDay = 1;

	/* ���롡*/
  	alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	/* ���ò��� */
	RTC_SetAlarm(RTC_Format_BIN, &alarm);

	/* ��ʼ������(����) */
	RTC_AlarmSubSecondConfig(msecond, RTC_AlarmSubSecondMask_None);

	return 0;
}

/*******************************************************************************
 * ����: vkRtclockStartAlarm
 * ����: ��������
 * �β�: 
 * ����: �ɹ�0
 * ˵��: 
 ******************************************************************************/
int vkRtclockStartAlarm(void)
{
	/* ʹ������ */
	RTC_AlarmCmd(ENABLE);	
    
    return 0;
}

/*******************************************************************************
 * ����: vkRtclockStopAlarm
 * ����: ֹͣ����
 * �β�: 
 * ����: �ɹ�0
 * ˵��: 
 ******************************************************************************/
int vkRtclockStopAlarm(void)
{
	/* ʹ������ */
	RTC_AlarmCmd(DISABLE);	
    
    return 0;
}

vkTIMESTAMP vkTimestampAdd(vkTIMESTAMP a, vkTIMESTAMP b)
{
	vkTIMESTAMP sum={0,0};

	sum.ms = (a.ms+b.ms)%1000;

	sum.ss = a.ss+b.ss+(a.ms+b.ms)/1000;

	return sum;
}

vkTIMESTAMP vkTimestampSub(vkTIMESTAMP a, vkTIMESTAMP b)
{
	vkTIMESTAMP sub={0,0};

	if(a.ss < b.ss)
	{
		return sub;
	}

	if(a.ms < b.ms)		
	{
		if(a.ss <= b.ss)
		{
			return sub;
		}
		else
		{
			a.ss = a.ss-1;
			a.ms = a.ms+1000;
		}
	}

	sub.ss = a.ss-b.ss;
	sub.ms = a.ms-b.ms;		

	return sub;
}

vkTIMESTAMP vkTimestampGen(uint32_t ms)
{
	vkTIMESTAMP timestamp;
	timestamp.ss = ms/1000;
	timestamp.ms = ms%1000;
	
	return timestamp;
}

int vkTimestampCmp(vkTIMESTAMP a, vkTIMESTAMP b)
{
	if(a.ss>b.ss)
	{
		return 1;
	}
	else if(a.ss<b.ss)
	{
		return -1;
	}
	else if(a.ss==b.ss && a.ms==b.ms)
	{
		return 0;
	}
	else if(a.ss==b.ss && a.ms>b.ms)
	{
		return 1;
	}
	else if(a.ss==b.ss && a.ms<b.ms)
	{
		return -1;
	}

	return 0;
}

int vkTimestampCpy(vkTIMESTAMP a, vkTIMESTAMP b)
{
	a.ss = b.ss;
	a.ms = b.ms;

	return 0;
}

int vkRtclockCalendar2Timestamp(vkTIMESTAMP* timestamp, vkCALENDAR calendar)
{
	/* ��1970�굽���ڵ�����(����������) */
	uint32_t days = days_to_year(calendar.year)-days_to_year(1970);
	days += days_to_month(calendar.year, calendar.month-1);
	days += calendar.day-1;

	timestamp->ss = days*SINDAY + calendar.hour*SINHOUR + calendar.minute*SINMINUTE + calendar.second;
	timestamp->ms = calendar.msecond;
	
	return 0;
}
int vkRtclockTimestamp2Calendar(vkCALENDAR* calendar, vkTIMESTAMP timestamp)
{
	uint16_t y;
	uint8_t m,d,h,min;

	/* ���� */
	calendar->weekday = rtclock_week[(timestamp.ss/SINDAY)%7];

	/* ��� */
	for(y=1970;timestamp.ss>=rtclock_siy[is_leap_year(y)];y++)
	{
		timestamp.ss -= rtclock_siy[is_leap_year(y)];
	}
	calendar->year = y;

	/* �·� */
	for(m=0;timestamp.ss>=rtclock_sim[is_leap_year(y)][m];m++)
	{
		timestamp.ss -= rtclock_sim[is_leap_year(y)][m];
	}	
	calendar->month = m+1;

	/* �� */
	for(d=0;timestamp.ss>=SINDAY;d++)
	{
		timestamp.ss -= SINDAY;
	}
	calendar->day = d+1;

	/* Сʱ */
	for(h=0;timestamp.ss>=SINHOUR;h++)
	{
		timestamp.ss -= SINHOUR;
	}
	calendar->hour = h;

	/* ���� */
	for(min=0;timestamp.ss>=SINMINUTE;min++)
	{
		timestamp.ss -= SINMINUTE;
	}
	calendar->minute = min;

	/* ���� */
	calendar->second = timestamp.ss;

	/* ���� */
	calendar->msecond = timestamp.ms;

	return 0;
}

int vkRtclockGetCalendar(vkCALENDAR *calendar)
{
	vkRtclockGetDate(&calendar->year, &calendar->month, &calendar->day, &calendar->weekday);
	vkRtclockGetTime(&calendar->hour, &calendar->minute, &calendar->second);
	vkRtclockGetMsec(&calendar->msecond);
	return 0;
}
int vkRtclockGetTimestamp(vkTIMESTAMP* timestamp)
{
	vkCALENDAR calendar;
	vkRtclockGetCalendar(&calendar);
	vkRtclockCalendar2Timestamp(timestamp, calendar);

	return 0;
}

