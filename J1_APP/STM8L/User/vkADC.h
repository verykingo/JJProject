
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
uint16_t vkADC_Read(ADC_Channel_TypeDef channel);

#endif
