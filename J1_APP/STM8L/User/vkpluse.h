
/******************************************************************************
 * �ļ�  ��vkpluse.h
 * ����    ��STM8L Pluse
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKPLUSE_H
#define __VKPLUSE_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "vkpwm.h"

#define PLUSE_TICK_TIME 40	//40us

/* �Զ����������� */
typedef struct pluse
{
	uint8_t  p_mode;		//ģʽһ��/ѭ��
	uint8_t  p_enable;		//�Ƿ�ʹ��
	uint8_t  p_expert;		//Ԥ�����mA
	uint8_t  p_current;		//��ǰ����mA
	uint8_t  p_duration;	//����ʱ��
	uint8_t  p_interval;	//���ʱ��
	uint8_t  p_poqlevel;	//�ϲ�״̬
	uint8_t  p_bodlevel;	//�²�״̬
	uint16_t p_poqcount;	//�ϲ����
	uint16_t p_bodcount;	//�²����
	
	vkPWM pwm;
} vkPLUSE;

void vkPluseTick(void);

#endif
