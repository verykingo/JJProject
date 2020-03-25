
/******************************************************************************
 * 文件  ：vkrtclock.c
 * 描述    ：RTC实时时钟，使用内部LSI时钟38KHz驱动，提供获取毫秒函数vkRtclockMktime()
           和延时函数vkRtclockDlyHMSM()，      系统初始化时间1970-01-01:00:00:00 星期四。
 * 平台    ：STM8L
 * 时间  ：2019-04-01
*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

/* 包含自定义头文件 */
#include "vkrtclock.h"

/* 自定义新类型 */

/* 自定义宏 */
#define ASYNCH_PREDIV			37		//异步分频，参数跟RTC驱动频率有关
#define SYNCH_PREDIV			999		//同步分频，参数跟RTC驱动频率有关
#define SUBSECOND_FREQUENCY		(SYNCH_PREDIV+1)	//亚秒频率	

#define SINDAY		24ul*60ul*60ul	//一天秒数
#define SINHOUR		60ul*60ul		//小时秒数
#define SINMINUTE	60ul			//分钟秒数


/* 全局变量定义 */
static uint8_t rtclock_century = 19;	//系统默认19世纪	

/* 某月中的天数 */
const static uint8_t rtclock_dim[2][12] = 
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},		//平年2月28
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},		//闰年2月29
};

/* 某月中的秒数 */
const static uint32_t rtclock_sim[2][12] = 
{
	{31ul*SINDAY, 28ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY},		//平年2月28
	{31ul*SINDAY, 29ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY, 30ul*SINDAY, 31ul*SINDAY},		//闰年2月29
};

/* 平年365天闰年366天 */
const static uint16_t rtclock_diy[2] = 
{365, 366};

/* 平年365天闰年366天的秒数 */
const static uint32_t rtclock_siy[2] = 
{365ul*SINDAY, 366ul*SINDAY};


/* 平年/闰年到某月的天数数组 */
const static uint16_t rtclock_dtm[2][12] = 
{
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},	//平年2月28/365
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},	//闰年2月29/366
};

/* 星期 */
const static uint8_t rtclock_week[7] = 
{4, 5, 6, 7, 1, 2, 3};


/* 判断是否是闰年，闰年返回1，平年返回0 */
static inline uint8_t is_leap_year(uint16_t year)
{
    return ((!(year % 4) && (year % 100)) || !(year % 400));
}

/* 计算某年某月的天数，month=1~12 */
static inline uint8_t days_in_month(uint8_t month, uint16_t year)
{
    return rtclock_dim[is_leap_year(year)][month-1];
}

/* 截止某年总共的天数，不包括当年 */
static inline uint32_t days_to_year(uint16_t year)
{	
	/* year的前一年 */
	year = year - 1;

	/* 闰年数 */
    uint16_t leap_year_num = year/4 - year/100 + year/400;

	/* 总计天数 */
    uint32_t days = year*365ul + leap_year_num;

	return days;
}

/* 在year这年，截止month月的天数，month=1~12 */
static inline uint32_t days_to_month(uint16_t year, uint8_t month)
{	
	return rtclock_dtm[is_leap_year(year)][month-1];
}

/*******************************************************************************
 * 名称: vkRtclock_Init
 * 功能: 实时时钟初始化，内部时钟LSI=38KHz，同步时钟频率1000Hz
 * 形参: 无
 * 返回: 成功0，失败-1
 * 说明: 
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
		.RTC_AsynchPrediv	= ASYNCH_PREDIV,	//异步分频38=37+1
		.RTC_SynchPrediv 	= SYNCH_PREDIV		//同步分频1000=999+1
	};
		
	if(SUCCESS != RTC_Init(&init))
	{	
		printf("%s rtc init!\r\n", __FUNCTION__);
		return -1;
	}

	RTC_WriteProtectionCmd(DISABLE);
	RTC_EnterInitMode();

	/* 设置时间00:00:00 */
	RTC_TimeTypeDef time =
	{
		.RTC_Hours 		= 0,
		.RTC_Minutes 	= 0,
		.RTC_Seconds 	= 0,
		.RTC_H12 		= RTC_H12_AM,
	};
	RTC_SetTime(RTC_Format_BIN, &time);

	/* 设置日期1970-01-01     星期四*/
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

	/* 同步时钟 */
	RTC_WaitForSynchro();

	#if 0	//有问题，会从halt模式唤醒
	/****************闹铃相关设置*************************/
	/* Disable RTC alarm */
	RTC_AlarmCmd(DISABLE);

	/* 初始闹铃时间00h:00mn:00sec，关闭日期星期功能 */
	RTC_AlarmTypeDef alarm =
	{
		.RTC_AlarmTime = 
		{
			.RTC_H12 = RTC_H12_AM,
			.RTC_Hours = 0,
			.RTC_Minutes = 0,
			.RTC_Seconds = 0,
		},
		/* 无效配置 */
		.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date,
  		.RTC_AlarmDateWeekDay 		= 1,
		/* 秒分时有效，日期星期无效 */
		.RTC_AlarmMask 	= RTC_AlarmMask_DateWeekDay,
	};

	/* 设置参数 */
	RTC_SetAlarm(RTC_Format_BIN, &alarm);

	/* 初始化亚秒(毫秒)0ssec */
	RTC_AlarmSubSecondConfig(0, RTC_AlarmSubSecondMask_None);

	/* 使能闹铃中断 */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	#endif
	
	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockSetTime
 * 功能: 设置时间
 * 形参: 小时hour=0~23，分钟minute=0~59，秒second=0~59
 * 返回: 成功0，失败-1
 * 说明: 
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

	/* 设置时间 */
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
	
	/* 同步时钟 */
	RTC_WaitForSynchro();

	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockGetTime
 * 功能: 获取时间
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
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
 * 名称: vkRtclockSetDate
 * 功能: 设置时间
 * 形参: 年year，月month=1~12，日day=1~31，星期weekday=1~7
 * 返回: 成功0，失败-1
 * 说明: 
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

	/* 设置日期 */
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
	
	/* 同步时钟 */
	RTC_WaitForSynchro();

	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockGetDate
 * 功能: 获取日期
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkRtclockGetDate(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *weekday)
{
	/* 设置日期 */
	RTC_DateTypeDef date;
	
  	RTC_GetDate(RTC_Format_BIN, &date);

	*year = date.RTC_Year + rtclock_century*100ul;
	*month = date.RTC_Month;
	*day = date.RTC_Date;
	*weekday = date.RTC_WeekDay;
	
	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockGetMsec
 * 功能: 获取豪秒
 * 形参: 毫秒指针
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkRtclockGetMsec(uint16_t *msecond)
{
	*msecond = ((SYNCH_PREDIV-RTC_GetSubSecond())*1000ul)/SUBSECOND_FREQUENCY;
	
	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockDlyHMSM
 * 功能: 实时时钟实现延时
 * 形参: 小时h，分钟m，秒s，毫秒ms
 * 返回: 成功0，失败-1
 * 说明: 
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
 * 名称: vkRtclockRefresh
 * 功能: 刷新实时时钟
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkRtclockRefresh()
{
	/* halt模式唤醒主机刷新时钟 */
	RTC_WaitForSynchro();
	
	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockSetAlarm
 * 功能: 设置闹铃时钟
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
 ******************************************************************************/
int vkRtclockSetAlarm(uint8_t hour, uint8_t minute, uint8_t second, uint16_t msecond)
{
	RTC_AlarmTypeDef alarm;
	/* 时间 */
	alarm.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	alarm.RTC_AlarmTime.RTC_Hours = hour;
	alarm.RTC_AlarmTime.RTC_Minutes = minute;
	alarm.RTC_AlarmTime.RTC_Seconds = second;

	/* 日期/星期 */
  	alarm.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  	alarm.RTC_AlarmDateWeekDay = 1;

	/* 掩码　*/
  	alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	/* 设置参数 */
	RTC_SetAlarm(RTC_Format_BIN, &alarm);

	/* 初始化亚秒(毫秒) */
	RTC_AlarmSubSecondConfig(msecond, RTC_AlarmSubSecondMask_None);

	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockStartAlarm
 * 功能: 启动闹铃
 * 形参: 
 * 返回: 成功0
 * 说明: 
 ******************************************************************************/
int vkRtclockStartAlarm(void)
{
	/* 使能闹铃 */
	RTC_AlarmCmd(ENABLE);	
    
    return 0;
}

/*******************************************************************************
 * 名称: vkRtclockStopAlarm
 * 功能: 停止闹铃
 * 形参: 
 * 返回: 成功0
 * 说明: 
 ******************************************************************************/
int vkRtclockStopAlarm(void)
{
	/* 使能闹铃 */
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
	/* 从1970年到现在的天数(不包括当天) */
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

	/* 星期 */
	calendar->weekday = rtclock_week[(timestamp.ss/SINDAY)%7];

	/* 年份 */
	for(y=1970;timestamp.ss>=rtclock_siy[is_leap_year(y)];y++)
	{
		timestamp.ss -= rtclock_siy[is_leap_year(y)];
	}
	calendar->year = y;

	/* 月份 */
	for(m=0;timestamp.ss>=rtclock_sim[is_leap_year(y)][m];m++)
	{
		timestamp.ss -= rtclock_sim[is_leap_year(y)][m];
	}	
	calendar->month = m+1;

	/* 日 */
	for(d=0;timestamp.ss>=SINDAY;d++)
	{
		timestamp.ss -= SINDAY;
	}
	calendar->day = d+1;

	/* 小时 */
	for(h=0;timestamp.ss>=SINHOUR;h++)
	{
		timestamp.ss -= SINHOUR;
	}
	calendar->hour = h;

	/* 分钟 */
	for(min=0;timestamp.ss>=SINMINUTE;min++)
	{
		timestamp.ss -= SINMINUTE;
	}
	calendar->minute = min;

	/* 秒钟 */
	calendar->second = timestamp.ss;

	/* 毫秒 */
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

