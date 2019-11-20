
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
#include <time.h>

/* �����Զ���ͷ�ļ� */
#include "vkrtclock.h"

/* �Զ��������� */

/* �Զ���� */
#define ASYNCH_PREDIV			37		//�첽��Ƶ��������RTC����Ƶ���й�
#define SYNCH_PREDIV			999		//ͬ����Ƶ��������RTC����Ƶ���й�
#define SUBSECOND_FREQUENCY		(SYNCH_PREDIV+1)	//����Ƶ��	

/* ȫ�ֱ������� */
static uint8_t rtclock_century = 19;	//ϵͳĬ��19����			

/* ĳ���е��������� */
const static uint8_t rtclock_dim[12] = 
{31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* ƽ��/���굽ĳ�µ��������� */
const static uint16_t rtclock_dtm[2][12] = 
{
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},	//ƽ��2��28/365
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},	//����2��29/366
};

/* �ж��Ƿ������� */
static inline uint8_t is_leap_year(uint16_t year)
{
    return ((!(year % 4) && (year % 100)) || !(year % 400));
}

/* ����ĳ��ĳ�µ����� */
static inline uint8_t days_in_month(uint8_t month, uint16_t year)
{
    uint8_t ret = rtclock_dim[month];
    if (ret == 0)
	{
        ret = is_leap_year(year) ? 29 : 28;
    }
    return ret;
}

/* ��ֹyear��ǰһ���ܹ������� */
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

/* ��year���꣬��ֹmonth�µ����� */
static inline uint32_t days_to_month(uint16_t year, uint8_t month)
{	
	return rtclock_dtm[is_leap_year(year)][month];
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

	/* ͬ��ʱ�� */
	RTC_WaitForSynchro();
	
	return 0;
}

/*******************************************************************************
 * ����: vkRtclockSetTime
 * ����: ����ʱ��
 * �β�: Сʱhour������minute����second
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
 * �β�: ��year����month����day������weekday
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
int vkRtclockGetMsec(uint16_t *msec)
{
	*msec = ((SYNCH_PREDIV-RTC_GetSubSecond())*1000ul)/SUBSECOND_FREQUENCY;
	
	return 0;
}


/*******************************************************************************
 * ����: vkRtclockMktime
 * ����: �����1970-01-01:00:00:00:000������ʱ��ĺ�����
 * �β�: ��
 * ����: ��1970-01-01:00:00:00:000������ʱ��ĺ�����
 * ˵��: 
 ******************************************************************************/
uint32_t vkRtclockMktime(void)
{
	uint16_t year,msecond;
	uint8_t month,day,weekday,hour,minute,second;

	vkRtclockGetMsec(&msecond);	 
	vkRtclockGetTime(&hour, &minute, &second);
	vkRtclockGetDate(&year, &month, &day, &weekday);

	uint32_t days = days_to_year(year)-days_to_year(1970);
	days += days_to_month(year, month-1);
	days += day-1;

	return ((days*24ul*60ul*60ul+hour*60ul*60ul+minute*60ul+second)*1000ul+msecond);
}

/*******************************************************************************
 * ����: vkRtclockDlyHMSM
 * ����: ʵʱʱ��ʵ����ʱ
 * �β�: Сʱh������m����s������ms
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkRtclockDlyHMSM(uint8_t h, uint8_t m, uint8_t s, uint16_t ms)
{
	if(m>59 || s>59 || ms>999)
	{
		return 0;
	}

	uint32_t t = vkRtclockMktime();

	uint32_t d = t+(h*60ul*60ul+m*60ul+s)*1000ul+ms;

	while(vkRtclockMktime() <= d)
	{
		nop();
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


