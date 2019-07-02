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

/* ÿһ��Tick����10����(���ʱʱ��2^32*10ms=42949000s)ʱ�� */
#define US_PER_TICKS	((uint32_t)(10000ul))

/* ΢��ת�� */
#define TICKS_TO_US(x) 	((uint32_t)(x*US_PER_TICKS))
#define US_TO_TICKS(x) 	((uint32_t)(x/US_PER_TICKS))

/* ����ת�� */
#define TICKS_TO_MS(x) 	((uint32_t)(x*US_PER_TICKS/1000ul))
#define MS_TO_TICKS(x) 	((uint32_t)(x*1000ul/US_PER_TICKS))

/* ��ת�� */
#define TICKS_TO_SS(x) 	((uint32_t)(x*US_PER_TICKS/1000ul*1000ul))
#define SS_TO_TICKS(x) 	((uint32_t)(x*1000ul*1000ul/US_PER_TICKS))

void TimeTick_Init(uint8_t TIM);

#endif

