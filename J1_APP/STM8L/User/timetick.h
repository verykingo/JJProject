/*****************************************************************************
 * 文件名  ：timetick.c
 * 描述    ：RTOS节拍时钟设置   
 * 修改时间 ：2019-04-01

*******************************************************************************/
#ifndef __TIMETICK_H
#define __TIMETICK_H

/* 包含系统头文件 */

/* 包含自定义头文件 */

/* 自定义数据类型 */

/* 每一个Tick等于10000微秒时间 */
#define US_PER_TICKS	((uint32_t)(10u*1000u))

#define TICKS_TO_US(x) 	((uint32_t)(x*US_PER_TICKS))
#define US_TO_TICKS(x) 	((uint32_t)(x/US_PER_TICKS))

#define TICKS_TO_MS(x) 	((uint32_t)(x*US_PER_TICKS/1000u))
#define MS_TO_TICKS(x) 	((uint32_t)(x*1000u/US_PER_TICKS))

#define TICKS_TO_SS(x) 	((uint32_t)(x*US_PER_TICKS/1000u*1000u))
#define SS_TO_TICKS(x) 	((uint32_t)(x*1000u*1000u/US_PER_TICKS))

void TimeTick_Init(uint8_t TIM);

#endif

