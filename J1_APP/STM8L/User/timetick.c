/******************************************************************************
 * �ļ���  ��timetick.c
 * ����    ��STM8L���Ķ�ʱ��
 * ƽ̨    ��STM8L
 * �޸�ʱ�� ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "timetick.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */

/*******************************************************************************
 * ����: TimeTick_Init
 * ����: TIM1��ʼ������
 * �β�: TIM��ʱ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void TimeTick_Init(uint8_t TIM)
{
	if(TIM == 1)
	{
		/* Enable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE); 

	    /* Reset TIM1 */
	    TIM1_DeInit();

	    /* Configure a 10ms tick, 16MHz/(15+1)=1MHz, 1s/MHz=1us */
		TIM1_TimeBaseInit(15, TIM1_CounterMode_Up, 9999, 0);
		TIM1_SetCounter(0);					/* ����������ֵ��Ϊ0 */
		TIM1_ARRPreloadConfig(DISABLE);		/* Ԥװ�ز�ʹ�� */
		ITC_SetSoftwarePriority(TIM1_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);
		
	    /* Generate an interrupt on timer count overflow �������*/
	    TIM1_ITConfig(TIM1_IT_Update, ENABLE);

	    /* Enable TIM1 */
	    TIM1_Cmd(ENABLE);
	}
}


