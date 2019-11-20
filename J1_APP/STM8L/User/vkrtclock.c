
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
#include <time.h>

/* 包含自定义头文件 */
#include "vkrtclock.h"

/* 自定义新类型 */

/* 自定义宏 */
#define ASYNCH_PREDIV			37		//异步分频，参数跟RTC驱动频率有关
#define SYNCH_PREDIV			999		//同步分频，参数跟RTC驱动频率有关
#define SUBSECOND_FREQUENCY		(SYNCH_PREDIV+1)	//亚秒频率	

/* 全局变量定义 */
static uint8_t rtclock_century = 19;	//系统默认19世纪			

/* 某月中的天数数组 */
const static uint8_t rtclock_dim[12] = 
{31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* 平年/闰年到某月的天数数组 */
const static uint16_t rtclock_dtm[2][12] = 
{
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},	//平年2月28/365
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},	//闰年2月29/366
};

/* 判断是否是闰年 */
static inline uint8_t is_leap_year(uint16_t year)
{
    return ((!(year % 4) && (year % 100)) || !(year % 400));
}

/* 计算某年某月的天数 */
static inline uint8_t days_in_month(uint8_t month, uint16_t year)
{
    uint8_t ret = rtclock_dim[month];
    if (ret == 0)
	{
        ret = is_leap_year(year) ? 29 : 28;
    }
    return ret;
}

/* 截止year的前一年总共的天数 */
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

/* 在year这年，截止month月的天数 */
static inline uint32_t days_to_month(uint16_t year, uint8_t month)
{	
	return rtclock_dtm[is_leap_year(year)][month];
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

	/* 同步时钟 */
	RTC_WaitForSynchro();
	
	return 0;
}

/*******************************************************************************
 * 名称: vkRtclockSetTime
 * 功能: 设置时间
 * 形参: 小时hour，分钟minute，秒second
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
 * 形参: 年year，月month，日day，星期weekday
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
int vkRtclockGetMsec(uint16_t *msec)
{
	*msec = ((SYNCH_PREDIV-RTC_GetSubSecond())*1000ul)/SUBSECOND_FREQUENCY;
	
	return 0;
}


/*******************************************************************************
 * 名称: vkRtclockMktime
 * 功能: 计算从1970-01-01:00:00:00:000到现在时间的毫秒数
 * 形参: 无
 * 返回: 从1970-01-01:00:00:00:000到现在时间的毫秒数
 * 说明: 
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
 * 名称: vkRtclockDlyHMSM
 * 功能: 实时时钟实现延时
 * 形参: 小时h，分钟m，秒s，毫秒ms
 * 返回: 成功0，失败-1
 * 说明: 
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


