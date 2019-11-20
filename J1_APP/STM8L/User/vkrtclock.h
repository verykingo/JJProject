
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
int vkRtclock_Init();

int vkRtclockSetTime(uint8_t hour, uint8_t minute, uint8_t second);
int vkRtclockGetTime(uint8_t *hour, uint8_t *minute, uint8_t *second);

int vkRtclockSetDate(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday);
int vkRtclockGetDate(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *weekday);

int vkRtclockGetMsec(uint16_t *msec);

int vkRtclockDlyHMSM(uint8_t hour, uint8_t min, uint8_t sec, uint16_t msec);

uint32_t vkRtclockMktime(void);

int vkRtclockRefresh();

#endif
