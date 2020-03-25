
/******************************************************************************
 * �ļ�  ��vktimetick.c
 * ����    ��STM8L���Ķ�ʱ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include <stdio.h>

/* �����Զ���ͷ�ļ� */
#include "vktimetick.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */

/*******************************************************************************
 * ����: vkTimeTick_Init
 * ����: TIM��ʼ������
 * �β�: TIM��ʱ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkTimeTick_Init(vkTIME TIM)
{
	/* ��ʱ��-��ʱ��1: 10ms/tick */
	if(TIM == TIME1)	
	{
		printf("%s Set TIM1 [10ms]\r\n", __FUNCTION__);
		
		/* Enable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE); 

	    /* Reset TIM1 */
	    TIM1_DeInit();

	    /* Configure a 10ms tick, 16MHz/(15+1)=1MHz, 1s/MHz=1us */
		TIM1_TimeBaseInit(15, TIM1_CounterMode_Up, 9999, 0);
		TIM1_SetCounter(0);					/* ����������ֵ��Ϊ0 */
		TIM1_ARRPreloadConfig(ENABLE);		/* Ԥװ��ʹ�� */
		ITC_SetSoftwarePriority(TIM1_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);
		
	    /* Generate an interrupt on timer count overflow �������*/
	    TIM1_ITConfig(TIM1_IT_Update, ENABLE);

	    /* Enable TIM1 */
	    //TIM1_Cmd(ENABLE);
	}
	/* ��ʱ��2: 10ms/tick    */
	else if(TIM == TIME2)
	{
		printf("%s Set TIM2 [10ms]\r\n", __FUNCTION__);
		
		/* Enable TIM2 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE); 

		/* Reset TIM2 */
	    TIM2_DeInit();

		/* Configure a 10ms tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, 9999);
		TIM2_SetCounter(0); 				/* ����������ֵ��Ϊ0 */
		TIM2_ARRPreloadConfig(ENABLE); 		/* Ԥװ��ʹ�� */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn, ITC_PriorityLevel_1);

		/* Generate an interrupt on timer count overflow �������*/
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);
		
		/* Enable TIM2 */
		//TIM2_Cmd(ENABLE);		
	}
	/* ��ʱ��3: 10ms/tick    */
	else if(TIM == TIME3)
	{
		printf("%s Set TIM3 [10ms]\r\n", __FUNCTION__);
		
		/* Enable TIM3 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE); 

		/* Reset TIM3 */
	    TIM3_DeInit();

		/* Configure a 10ms tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM3_TimeBaseInit(TIM3_Prescaler_16, TIM3_CounterMode_Up, 9999);
		TIM3_SetCounter(0); 				/* ����������ֵ��Ϊ0 */
		TIM3_ARRPreloadConfig(ENABLE); 		/* Ԥװ��ʹ�� */
		ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn, ITC_PriorityLevel_1);

		/* Generate an interrupt on timer count overflow �������*/
		TIM3_ITConfig(TIM3_IT_Update, ENABLE);
		
		/* Enable TIM3 */
		//TIM3_Cmd(ENABLE);		
	}
	/* ��ʱ��4: 1ms/tick    */
	else if(TIM == TIME4)
	{
	#if 0
		/* Enable TIM4 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); 

		/* Reset TIM4 */
	    TIM4_DeInit();

		/* Configure a 100ms tick, 16MHz/(32768)=2048us, 2048us*49~=100ms */
		/* Configure a 1ms tick, 16MHz/(4)=4us, 4us*250=1ms */
		TIM4_TimeBaseInit(TIM4_Prescaler_4, 249);
		TIM4_SetCounter(0); 				/* ����������ֵ��Ϊ0 */
		TIM4_ARRPreloadConfig(ENABLE); 		/* Ԥװ��ʹ�� */
		ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_1);

		/* Generate an interrupt on timer count overflow �������*/
		TIM4_ITConfig(TIM4_IT_Update, ENABLE);
		
		/* Enable TIM4 */
		//TIM4_Cmd(ENABLE);	
	#endif
	}
	/* ��ʱ��5: 50us/tick    */
	else if(TIM == TIME5)
	{
		printf("%s Set TIM5 [50us]\r\n", __FUNCTION__);
		
		/* Enable TIM5 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE); 

		/* Reset TIM5 */
	    TIM5_DeInit();

		/* Configure a 50us tick, 16MHz/(16)=1MHz, 1s/MHz=1us */
		TIM5_TimeBaseInit(TIM5_Prescaler_16, TIM5_CounterMode_Up, 49);
		TIM5_SetCounter(0); 				/* ����������ֵ��Ϊ0 */
		TIM5_ARRPreloadConfig(ENABLE); 		/* Ԥװ��ʹ�� */
		ITC_SetSoftwarePriority(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_3);

		/* Generate an interrupt on timer count overflow �������*/
		TIM5_ITConfig(TIM5_IT_Update, ENABLE);
		
		/* Enable TIM5 */
		//TIM5_Cmd(ENABLE);		
	}
	else
	{		
		printf("%s Failed!\r\n", __FUNCTION__);
				
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * ����: vkTimeTick_Deinit
 * ����: TIM���ٲ���
 * �β�: TIM��ʱ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkTimeTick_Deinit(vkTIME TIM)
{
	if(TIM == TIME1)
	{
		/* Disable TIM1 */
		TIM1_Cmd(DISABLE);

		/* Reset TIM1 */
	    TIM1_DeInit();	
		
		/* Disable TIM1 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, DISABLE); 	
	}
	else if(TIM == TIME2)
	{
		/* Disable TIM2 */
		TIM2_Cmd(DISABLE);

		/* Reset TIM2 */
	    TIM2_DeInit();	
		
		/* Disable TIM2 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE); 
	}
	else if(TIM == TIME3)
	{
		/* Disable TIM3 */
		TIM3_Cmd(DISABLE);	

		/* Reset TIM3 */
	    TIM3_DeInit();	
		
		/* Disable TIM3 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE); 
	}
	else if(TIM == TIME4)
	{
		/* Disable TIM4 */
		TIM4_Cmd(ENABLE);

		/* Reset TIM4 */
	    TIM4_DeInit();	
		
		/* Disable TIM4 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE); 
	}
	else if(TIM == TIME5)
	{
		/* Disable TIM5 */
		TIM5_Cmd(DISABLE);	

		/* Reset TIM5 */
	    TIM5_DeInit();	
		
		/* Disable TIM5 CLK*/
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, DISABLE); 
	}
	else
	{		
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * ����: vkTimeTickStart
 * ����: TIM����
 * �β�: TIM��ʱ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkTimeTickStart(vkTIME TIM)
{
	if(TIM == TIME1)
	{
	    /* Enable TIM1 */
	    TIM1_Cmd(ENABLE);		
	}
	else if(TIM == TIME2)
	{
	    /* Enable TIM2 */
	    TIM2_Cmd(ENABLE);		
	}
	else if(TIM == TIME3)
	{
	    /* Enable TIM3 */
	    TIM3_Cmd(ENABLE);		
	}
	else if(TIM == TIME4)
	{
	    /* Enable TIM4 */
	    TIM4_Cmd(ENABLE);		
	}
	else if(TIM == TIME5)
	{
	    /* Enable TIM5 */
	    TIM5_Cmd(ENABLE);		
	}
	else
	{
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * ����: vkTimeTickStop
 * ����: TIMֹͣ
 * �β�: TIM��ʱ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkTimeTickStop(vkTIME TIM)
{
	if(TIM == TIME1)
	{
	    /* Disable TIM1 */
	    TIM1_Cmd(DISABLE);		
	}
	else if(TIM == TIME2)
	{
	    /* Disable TIM2 */
	    TIM2_Cmd(DISABLE);		
	}
	else if(TIM == TIME3)
	{
	    /* Disable TIM3 */
	    TIM3_Cmd(DISABLE);		
	}
	else if(TIM == TIME4)
	{
	    /* Disable TIM4 */
	    TIM4_Cmd(ENABLE);		
	}
	else if(TIM == TIME5)
	{
	    /* Disable TIM5 */
	    TIM5_Cmd(DISABLE);		
	}
	else
	{
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

