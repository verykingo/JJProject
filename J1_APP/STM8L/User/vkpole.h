
/******************************************************************************
 * �ļ�  ��vkpole.h
 * ����    ���豸ϵͳ״̬
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKPOLE_H
#define __VKPOLE_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include "vksofttimer.h"
#include "vkpluse.h"

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */

#define POLE_MODE_ONLYONCE PLUSE_MODE_ONLYONCE		//����ģʽ
#define POLE_MODE_CONTINUE PLUSE_MODE_CONTINUE		//ѭ��ģʽ	

enum POLE_DEVICE
{
	POLE_A	= 1,	//�缫A,��Ӧpluse0
	POLE_B 	= 2,	//�缫B,��Ӧpluse1
};

typedef struct POLE 
{
	uint8_t device[2];			//�缫A��B��Ӧ������
	uint8_t enable[2];			//�缫ABʹ��
	uint8_t linked[2];			//�缫�ǽ���
	
	uint16_t trained[2];		//ѵ��ʱ�䣬����̬ģʽ
	vkTIMER timer_trained[2];	//�缫ѵ��ʱ�䶨ʱ��������̬ģʽ
	
	uint16_t delayed[2];		//�缫������ʱʱ�䣬    ���˶�ģʽ
	vkTIMER timer_delayed[2];	//�缫������ʱ��ʱ�������˶�ģʽ
	uint16_t expired;			//�缫���йر�ʱ�䣬���˶�ģʽ
	vkTIMER timer_expired;		//�缫���йرն�ʱ�������˶�ģʽ	

} vkPOLE;

/* �缫�ӿں��� */
int vkPole_Init(void);
int vkPole_Deinit(void);

int vkPoleSetMode(uint8_t pole, uint8_t mode);
int vkPoleSetExpt(uint8_t pole, uint8_t expect);
int vkPoleSetTime(uint8_t pole, uint16_t duration, uint16_t interval);
int vkPoleSetDelayed(uint8_t pole, uint16_t delay);
int vkPoleSetExpired(uint16_t expire);
int vkPoleSetTrained(uint8_t pole, uint16_t train);


int vkPoleStart(uint8_t pole);
int vkPoleStop(uint8_t pole);
int vkPoleGetIdle(void);

int vkPoleTriger(uint8_t pole);

uint8_t vkPoleGetEnable(uint8_t pole);
uint8_t vkPoleGetLinked(uint8_t pole);
uint8_t vkPoleGetExpect(uint8_t pole);
uint8_t vkPoleGetPoqlvl(uint8_t pole);

#endif
