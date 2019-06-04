/*****************************************************************************
 * �ļ���  ��timetick.c
 * ����    ��RTOS����ʱ������ʱ������   
 * �޸�ʱ�� ��2019-04-01

*******************************************************************************/
#ifndef __TIMETICK_H
#define __TIMETICK_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */

/* ÿһ��Tick����10000΢��ʱ�� */
#define US_PER_TICKS	((uint32_t)(10u*1000u))

/* ΢��ת�� */
#define TICKS_TO_US(x) 	((uint32_t)(x*US_PER_TICKS))
#define US_TO_TICKS(x) 	((uint32_t)(x/US_PER_TICKS))

/* ����ת�� */
#define TICKS_TO_MS(x) 	((uint32_t)(x*US_PER_TICKS/1000u))
#define MS_TO_TICKS(x) 	((uint32_t)(x*1000u/US_PER_TICKS))

/* ��ת�� */
#define TICKS_TO_SS(x) 	((uint32_t)(x*US_PER_TICKS/1000u*1000u))
#define SS_TO_TICKS(x) 	((uint32_t)(x*1000u*1000u/US_PER_TICKS))

void TimeTick_Init(uint8_t TIM);

#endif

