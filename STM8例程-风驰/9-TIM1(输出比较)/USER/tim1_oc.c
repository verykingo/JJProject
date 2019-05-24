/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��tim1_oc.c
 * ����    ��TIM1����Ƚ�ʵ��   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 *�޸�ʱ�� ��2012-10-20
 
  ���STM8������Ӳ������
  LED1-PD0
  TIM1���벶��/����Ƚ�ͨ��1--PC1
  ʵ��ʱ����Ҫ��PD0��PC1�öŰ�������

*******************************************************************************/
/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "tim1_oc.h"
#include "stm8s_tim1.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */


/*******************************************************************************
 * ����: Tim1_OC_Init
 * ����: TIM1ͨ��1����Ƚϳ�ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Tim1_OC_Init()
{
	TIM1_DeInit();
	
	/*
	TIM1_Period = 50000
	TIM1_Prescaler = 0
	TIM1_CounterMode = TIM1_COUNTERMODE_UP
	TIM1_RepetitionCounter = 0
	*/
	TIM1_TimeBaseInit(23, TIM1_COUNTERMODE_UP, 49999, 0);	
	
	/*
	TIM1_OCMode = TIM1_OCMODE_TOGGLE
	TIM1_OutputState = TIM1_OUTPUTSTATE_ENABLE
	TIM1_OutputNState = TIM1_OUTPUTNSTATE_DISABLE
	TIM1_Pulse = 5000	
	TIM1_OCPolarity = TIM1_OCPOLARITY_LOW
	TIM1_OCNPolarity = TIM1_OCNPOLARITY_HIGH
	TIM1_OCIdleState = TIM1_OCIDLESTATE_SET
	TIM1_OCNIdleState = TIM1_OCIDLESTATE_RESET
	*/
	TIM1_OC3Init(TIM1_OCMODE_TOGGLE, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE,
			   4999, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET,
			   TIM1_OCNIDLESTATE_RESET);
 	
	/* TIM1 counter ʹ�� */
	TIM1_Cmd(ENABLE);
	TIM1_CtrlPWMOutputs(ENABLE);	/* ʹ��TIM1��� */
}