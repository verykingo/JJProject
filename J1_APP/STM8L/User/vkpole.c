/******************************************************************************
 * �ļ�  ��vkpole.c
 * ����    ���豸�缫
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include "stdio.h"
#include "string.h"

/* �����Զ���ͷ�ļ� */
#include "vkpole.h"
#include "vksysrun.h"
#include "vkpower.h"
#include "vkpluse.h"
#include "vkpole.h"
#include "vktimetick.h"
#include "vktly.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */
static vkPOLE gPole =
{
	.device = {PLUSE0, PLUSE1},
	.enable = {0, 0},
	.linked = {0, 0},
	.trained = {0, 0},
	.delayed = {0, 0},
	.expired = 0
};

/*******************************************************************************
 * ����: vkPole_Init
 * ����: �缫��ʼ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkPole_Init(void)
{
	return vkPluse_Init();
}

/*******************************************************************************
 * ����: vkPole_Deinit
 * ����: �缫������
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkPole_Deinit(void)
{
	return vkPluse_Deinit();
}

/*******************************************************************************
 * ����: vkPoleSetMode
 * ����: ���õ缫ģʽ
 * �β�: mode����ģʽ��ѭ��ģʽ
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkPoleSetMode(uint8_t pole, uint8_t mode)
{
	int index = pole-1;
	return vkPluseSetMode(gPole.device[index], mode);
}

/*******************************************************************************
 * ����: vkPoleSetExpt
 * ����: ���õ缫����ǿ��mA
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkPoleSetExpt(uint8_t pole, uint8_t expect)
{
	int index = pole-1;
	return vkPluseSetExpt(gPole.device[index], expect);
}

/*******************************************************************************
 * ����: vkPoleSetTime
 * ����: ���õ缫�������ʱ��ͼ��ʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ���óɹ���������Ч��ʱ�䵥λ��ms
 ******************************************************************************/
int vkPoleSetTime(uint8_t pole, uint16_t duration, uint16_t interval)
{
	int index = pole-1;
	return vkPluseSetTime(gPole.device[index], duration, interval);
}

/*******************************************************************************
 * ����: vkPoleSetTrained
 * ����: ��̬ģʽ������ѵ��ʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ʱ�䵥λ��s��5min~30min��5min/step����300s~1800s��
 ******************************************************************************/
int vkPoleSetTrained(uint8_t pole, uint16_t train)
{
	int index = pole-1;

	gPole.trained[index] = train;
	
	return 0;
}

/*******************************************************************************
 * ����: vkPoleSetDelayed
 * ����: ��̬ģʽ�����ô�������ʱʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ʱ�䵥λ��ms��0ms~1000ms��100ms/step��
 ******************************************************************************/
int vkPoleSetDelayed(uint8_t pole, uint16_t delay)
{
	int index = pole-1;

	gPole.delayed[index] = delay;
	
	return 0;
}

/*******************************************************************************
 * ����: vkPoleSetExpired
 * ����: ��̬ģʽ������ֹͣʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ʱ�䵥λ��s��0.5h/1h/2h/3h/4h����180s~14400s��
 ******************************************************************************/
int vkPoleSetExpired(uint16_t expire)
{
	gPole.expired = expire;
	
	return 0;
}

/* �缫�²����嶼���ڵ͵�ƽ */
int vkPoleGetIdle(void)
{
	return vkPluseGetIdle();
}

void vkPole_Timer_Delayed_Callback(void *pdata)
{
    uint8_t pluse = *((uint8_t *)pdata);

	/* �����Ƿ�����������̼� */
	if(PLUSE_LEVEL_LOW == vkPluseGetPoqL(pluse))
	{
		vkPluseStart(pluse);
	}
}

/*******************************************************************************
 * ����: vkPoleTriger
 * ����: ����ģʽ�£�����һ������̼�
 * �β�: �缫pole
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
inline int vkPoleTriger(uint8_t pole)
{
	int index = pole-1;

	/* �Ƿ�ʹ�� */
	if(gPole.enable[index] == 0)
	{
		return -1;
	}	

	/* �Ƿ�����ʱ */
	if(gPole.delayed[index] == 0)
	{
		/* �����Ƿ�����������̼� */
		if(PLUSE_LEVEL_LOW ==vkPoleGetPoqlvl(pole))
		{
			/* �������������������������̼� */
			vkPluseStart(gPole.device[index]);
		}
	}
	else
	{
		/* ���������������ʱ��������̼� */
		/* ������ʱ�� */
		vkTimerCancel(&gPole.timer_delayed[index]);
		
		gPole.timer_delayed[index].timer_name = &gPole.timer_delayed[index];
		gPole.timer_delayed[index].cb_func    = vkPole_Timer_Delayed_Callback;
		gPole.timer_delayed[index].cb_data    = (void *)&gPole.device[index];
		gPole.timer_delayed[index].cb_ticks   = vkMS_TO_TICKS(gPole.delayed[index]);
		
		vkTimerInsert(&gPole.timer_delayed[index]);		
	}	
    
    return 0;
}

/* �缫�Ƿ��� */
uint8_t vkPoleGetEnable(uint8_t pole)
{
	int index = pole -1;

	return gPole.enable[index];
}

/* �缫�Ƿ���� */
uint8_t vkPoleGetLinked(uint8_t pole)
{
	int index = pole -1;

	return gPole.linked[index];
}

uint8_t vkPoleGetExpect(uint8_t pole)
{
	int index = pole-1;
	
	return vkPluseGetExpt(gPole.device[index]);
}

uint8_t vkPoleGetPoqlvl(uint8_t pole)
{
	int index = pole-1;

	return vkPluseGetPoqL(gPole.device[index]);
}

void vkPole_Timer_expired_Callback(void * pdata)
{
	vkPoleStop(POLE_A);
	vkPoleStop(POLE_B);
}

void vkPole_Timer_Trained_Callback(void * pdata)
{
	uint8_t pole = *((uint8_t *)pdata) + 1;
		
	vkPoleStop(pole);
}

int vkPoleStart(uint8_t pole)
{
	int index = pole-1;

	uint8_t device_mode = vkDeviceModeGet();
	
	/* �˶�ģʽ */
	if(device_mode == DEVICE_MODE_WALKING)
	{
		/*��������Tick */
		vkTimeTickStart(TIME5);
		
		/* �򿪵缫A��B���� */
		vkPowerPoleASet(1);
		vkPowerPoleBSet(1);

		/* ���������� */
		vkTlyStart();

		/* ��ʱ10ms����֤�����ǵ���ȷ���� */
		DelayMS(10);

		/* �缫ʹ�� */
		gPole.enable[0] = 1;
		gPole.enable[1] = 1;

		/* ��������ʱ�䶨ʱ�� */
		if(gPole.expired != 0)
		{
			/* ������ʱ�� */
			vkTimerCancel(&gPole.timer_expired);
			
			gPole.timer_expired.timer_name = &gPole.timer_expired;
			gPole.timer_expired.cb_func    = vkPole_Timer_expired_Callback;
			gPole.timer_expired.cb_data	   = NULL;
			gPole.timer_expired.cb_ticks   = vkSS_TO_TICKS(gPole.expired);

			vkTimerInsert(&gPole.timer_expired);
		}
		
		nop();
	}
	/* ��̬ģʽ */
	else if(device_mode == DEVICE_MODE_TRAINING)	
	{
		if(gPole.enable[0]==0 && gPole.enable[1]==0)
		{
			/*��������Tick */
			vkTimeTickStart(TIME5);			
		}
		
		/* �򿪵缫ģ�鹩�� */
		if(pole == POLE_A)
		{
			vkPowerPoleASet(1);
		}
		else if(pole == POLE_B)
		{
			vkPowerPoleBSet(1);
		}

		/* �������壬��̬ģʽ������ʱʱ�䣬��ѵ��ʱ�� */
		gPole.enable[index] = 1;		
		vkPluseStart(gPole.device[index]);

		/* ����ѵ��ʱ�䶨ʱ�� */
		if(gPole.trained[index] != 0)
		{
			/* ������ʱ�� */
			vkTimerCancel(&gPole.timer_trained[index]);
			
			gPole.timer_trained[index].timer_name = &gPole.timer_trained[index];
			gPole.timer_trained[index].cb_func    = vkPole_Timer_Trained_Callback;
			gPole.timer_trained[index].cb_data	  = (void *)&gPole.device[index];
			gPole.timer_trained[index].cb_ticks   = vkSS_TO_TICKS(gPole.trained[index]);

			vkTimerInsert(&gPole.timer_trained[index]);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int vkPoleStop(uint8_t pole)
{
	int index = pole-1;
		
	uint8_t device_mode = vkDeviceModeGet();

	/* �˶�ģʽ */
	if(device_mode == DEVICE_MODE_WALKING)
	{
		/* ֹͣ������� */
		gPole.enable[0] = 0;
		gPole.enable[1] = 0;
		vkPluseStop(gPole.device[0]);
		vkPluseStop(gPole.device[1]);

		/* �رյ缫ģ�鹩�� */
		vkPowerPoleASet(0);
		vkPowerPoleBSet(0);		

		/* ֹͣ����Tick */
		vkTimeTickStop(TIME5);
		
		/* ֹͣ������ */
		vkTlyStop();

		/* ֹͣ����ʱ/��ֹʱ�� */
		vkTimerCancel(&gPole.timer_delayed[0]);
		vkTimerCancel(&gPole.timer_delayed[1]);
		vkTimerCancel(&gPole.timer_expired);
	}
	/* ��̬ģʽ */
	else if(device_mode == DEVICE_MODE_TRAINING)
	{
		/* ���õ缫ֹͣ��ֹͣ���� */
		gPole.enable[index] = 0;
		vkPluseStop(gPole.device[index]);

		/* �رյ缫ģ�鹩�� */
		if(pole == POLE_A)
		{
			vkPowerPoleASet(0);
		}
		else if(pole == POLE_B)
		{
			vkPowerPoleBSet(0);
		}

		/* ֹͣѵ����ʱ�� */
		vkTimerCancel(&gPole.timer_trained[0]);
		vkTimerCancel(&gPole.timer_trained[1]);

		/* ��������缫��ֹͣ          */
		if(gPole.enable[0] == 0 && gPole.enable[1] == 0)
		{
			/* ֹͣ����Tick */
			vkTimeTickStop(TIME5);
			nop();
		}
	}
	else
	{
		return -1;
	}

	return 0;
}


