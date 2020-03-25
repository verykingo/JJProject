
/******************************************************************************
 * �ļ�  ��vktimetick.h
 * ����    ��STM8L���Ķ�ʱ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKTIMETICK_H
#define __VKTIMETICK_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */
typedef enum TIME
{
	TIME1 = 1,	//��ʱ��1
	TIME2 = 2,	//��ʱ��2
	TIME3 = 3,	//��ʱ��3
	TIME4 = 4,	//��ʱ��4
	TIME5 = 5	//��ʱ��5
} vkTIME;


/* ��ʱ��ÿһ��Tick����10����(���ʱʱ��2^32*10ms=42949000s)ʱ�� */
#define SOFTTIMER_US_PER_TICKS	((uint32_t)(10000ul))

int vkTimeTick_Init(vkTIME TIM);
int vkTimeTick_Deinit(vkTIME TIM);
int vkTimeTickStart(vkTIME TIM);
int vkTimeTickStop(vkTIME TIM);

#endif

