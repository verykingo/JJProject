
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
