
/******************************************************************************
 * �ļ�  ��vkADC.h
 * ����  ��ADC
 * ƽ̨  ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKADC_H
#define __VKADC_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */

int vkADC_Init(void);
int vkADC_Deinit(void);

int vkADCStart(void);
int vkADCStop(void);

uint16_t vkADCRead(ADC_Channel_TypeDef channel);

#endif