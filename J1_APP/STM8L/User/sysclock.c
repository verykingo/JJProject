/******************************************************************************
 * �ļ�  ��sysclock.c
 * ����    ��ϵͳʱ������
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "sysclock.h"

/*******************************************************************************
 * ����: Sysclock_Init
 * ����: ����ϵͳʱ��Ƶ�ʣ�ʱ��ѡ��16MHz
 * �β�: ��
 * ����: ��
 * ˵��: ϵͳ��λ��Ĭ��ѡ��HSI=16/2=8MHz����������ʱ�ӹر�
 ******************************************************************************/
int SystemClock_Init(void)
{
	/* Deinitializes the CLK peripheral */
	CLK_DeInit();

	/* ʹ��ʱ���л����� */
	CLK_SYSCLKSourceSwitchCmd(ENABLE);

	/* ����ϵͳʱ��HSI */
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

	/* ����ϵͳʱ�ӷ�Ƶ */
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

	/* Check the system clock source is HSI */
	while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
	{nop();}

	/* �ر�ʱ���л����� */
	CLK_SYSCLKSourceSwitchCmd(DISABLE);

	return 0;
}

/*******************************************************************************
 * ����: Sysclock_Deinit
 * ����: ȡ��ϵͳʱ��ʼ��
 * �β�: ��
 * ����: ��
 * ˵��: ʱ��ѡ��16MHz
 ******************************************************************************/
int Systemclock_Deinit(void)
{
	/* Deinitializes the CLK peripheral */
	CLK_DeInit();

	return 0;
}

/***************************************************************END OF FILE****/