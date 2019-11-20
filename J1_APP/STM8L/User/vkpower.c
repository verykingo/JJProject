/******************************************************************************
 * �ļ�  ��vkpower.c
 * ����    ��FootDrop��Դ����
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "vkpower.h"
#include "vksofttimer.h"
#include "vktimetick.h"
#include "vksysrun.h"
#include "vkleds.h"
#include "vkbattery.h"
#include "BltProtocol.h"
#include "vkpole.h"
#include "vktly.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */
static vkTIMER gPower_Timer;	//��������������ʱ��ʱ��

/*******************************************************************************
 * ����: vkPower_Init
 * ����: ������ģ�顢����ģ�顢�缫A�͵缫B���巢��ģ�鹩�翪��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkPower_Init(void)
{
	/* ģ��GPIO��ʼ�� GPIO_Mode_Out_PP_Low_Fast */
	GPIO_Init(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);	
	GPIO_Init(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN, GPIO_Mode_Out_PP_Low_Fast);

	/* ģ�鲻���� */
	GPIO_ResetBits(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN);
	GPIO_ResetBits(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN);	
	GPIO_ResetBits(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN);
	GPIO_ResetBits(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN);

	/* ����������ʼ�� */
	GPIO_Init(BTN_SWITCH_IO_PORT,BTN_SWITCH_PIN,GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);
	ITC_SetSoftwarePriority(EXTI2_IRQn, ITC_PriorityLevel_3);	

	return 0;
}

/*******************************************************************************
 * ����: vkPowerBLESet
 * ����: ��������ģ�鹩��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��PC5 
 ******************************************************************************/
int vkPowerBLESet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN):GPIO_SetBits(BLE_SWITCH_IO_PORT, BLE_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * ����: vkPowerTlySet
 * ����: ���������ǹ���
 * �β�: 
 * ����: �ɹ�0
 * ˵��: �� 
 ******************************************************************************/
int vkPowerTLYSet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN):GPIO_SetBits(TLY_SWITCH_IO_PORT, TLY_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * ����: vkPowerPoleASet
 * ����: ���õ缫A����
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��
 ******************************************************************************/
int vkPowerPoleASet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN):GPIO_SetBits(DJA_SWITCH_IO_PORT, DJA_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * ����: vkPowerPoleBSet
 * ����: ���õ缫B����
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��
 ******************************************************************************/
int vkPowerPoleBSet(uint8_t mode)
{
	(mode == 0)?GPIO_ResetBits(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN):GPIO_SetBits(DJB_SWITCH_IO_PORT, DJB_SWITCH_PIN);

	return 0;
}

/*******************************************************************************
 * ����: vkPowerBTNGet
 * ����: ��ȡ���ذ���״̬
 * �β�: 
 * ����: ����״̬
 * ˵��: �� 
 ******************************************************************************/
uint8_t vkPowerBTNGet()
{	
	return (GPIO_ReadInputDataBit(BTN_SWITCH_IO_PORT, BTN_SWITCH_PIN) == RESET)?0:1;
}


/*
 * ��Դ���س�����ʱ���ص�����
 */
void vkPower_Timer_Callback(void *pdata)
{
	/* ��ʱ���˳�haltģʽ */
	
	/* �������¿�������2��� */
	if(vkPowerBTNGet() == 0)
	{
		/* �ػ�״̬�£��򿪻� */
		if(vkDeviceWorkGet() == 0)
		{
			/* ���ÿ���״̬ */
			vkDeviceWorkSet(1);

			/* LEDָʾ���� */
			vkLedsSet(LED0, 1);
			vkLedsSet(LED1, 1);	
			vkLedsStart();

			/* ����ģ�鹩�� */
			vkPowerBLESet(1);
			DelayMS(10);
			
			/* ����ģ���ʼ�� */
			if(0 != Blt_Init())
			{
				/* ����ģ���ʼ��ʧ�ܣ���⴮������ */
				/* ָʾ����ʾ��ϵͳ�ػ� */	
				vkPowerInitError(0, 10);
				WWDG_SWReset();				
			}			
			/* ��������ģ��ͨ�� */
			Blt_CommunicationStart();
			
			/* �豸δ��������APP */
			vkDeviceBlueSet(DEVICE_BLUE_UNCONNECT);

			/* ���������ǹ��� */
			vkPowerTLYSet(1);
			DelayMS(10);
			
			/* �����ǳ�ʼ�� */
			if(0 != vkTly_Init())
			{
				/* ָʾ����ʾ */
				vkPowerInitError(1, 10);
			}
			
			/* ������ع��� */
			vkBatteryStart();
		}
		/* ����״̬�¡���ػ� */
		else
		{
			/* ���ùػ�״̬ */
			vkDeviceWorkSet(0);
			
			/* ����ģ�鹩�� */
			vkPowerBLESet(0);

			/* ֹͣ�缫A��B���� */
			vkPoleStop(POLE_A);
			vkPoleStop(POLE_B);

			/* LEDָʾ���� */
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);
			
			/* û�г�磬ϵͳ��λ���� */
			if(vkBatteryCharge() == 0)
			{
				WWDG_SWReset();
			}
			//ϵͳ���ڳ�磬��ϵͳ��λ�������ȵ�����ͷ�γ���ϵͳ��λ����
			else
			{
				//do nothing
			}			
		}
	}
	/* �������¿�������С��2�룬�����κβ��� */
	else
	{
		//do nothing
	}
}

/*
 * ��Դ���ذ����жϻص�������
 */
void vkPower_Interrupt_Callback()
{	
	/* ��ɾ����ǰ��ʱ�� */
	vkTimerCancel(&gPower_Timer);
	
	gPower_Timer.timer_name = &gPower_Timer;
	gPower_Timer.cb_func 	= vkPower_Timer_Callback;
	gPower_Timer.cb_data 	= NULL;
	gPower_Timer.cb_ticks 	= vkSS_TO_TICKS(2);
	
	vkTimerInsert(&gPower_Timer);

}

int vkPowerInitError(uint8_t error, uint16_t duration)
{
	/* ���̵ƽ���0.5s��˸ */
	if(error == 0)
	{
		for(int i=0; i<duration; i++)
		{
			vkLedsSet(LED0, i%2);
			vkLedsSet(LED1, (i+1)%2);
			DelayMS(500);
			
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);		
			DelayMS(500);
		}
	}
	else if(error == 1)
	{
		for(int i=0; i<duration/2; i++)
		{
			vkLedsSet(LED0, i%2);
			vkLedsSet(LED1, (i+1)%2);
			DelayMS(500);
			
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);		
			DelayMS(500);

			vkLedsSet(LED0, i%2);
			vkLedsSet(LED1, (i+1)%2);
			DelayMS(500);
			
			vkLedsSet(LED0, 0);
			vkLedsSet(LED1, 0);		
			DelayMS(500);
		}		
	}
	
	return 0;
}

