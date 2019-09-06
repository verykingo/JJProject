/*****************************************************************************
 * �ļ���  ��sysclock.c
 * ����    ��ϵͳʱ������   
 * �޸�ʱ�� ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "sysclock.h"

/*******************************************************************************
 * ����: Sysclock_Init
 * ����: ����ϵͳʱ��Ƶ��
 * �β�: ��
 * ����: ��
 * ˵��: ʱ��ѡ��
         ���ѡ���ⲿ24MHz��ʱ�ӵģ��ⲿ����24M�Ļ�
         ��#define HSE_24M  HSE_VALUEע�͵�����,
         �������Ҫ�л����ⲿʱ�ӣ���#define __HSE_VALUE  HSE_VALUE
         ע�͵�����
 ******************************************************************************/
void SystemClock_Init(void)
{
	/* Select HSI as system clock source */
	CLK_SYSCLKSourceSwitchCmd(ENABLE);
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

	/*High speed external clock prescaler: 1*/
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

	/* Check the system clock source is HSI */
	while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
	{}	
}


/***************************************************************END OF FILE****/