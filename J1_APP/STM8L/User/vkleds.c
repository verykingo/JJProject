/******************************************************************************
 * �ļ�  ��vkleds.c
 * ����    ��ledָʾ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-09-04

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "vkleds.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */

/*******************************************************************************
 * ����: vkLeds_Init
 * ����: Led�Ƴ�ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkLeds_Init()
{
	int size = sizeof(led_gpio)/sizeof(struct LED_GPIO);

	for(int i=0; i<size; i++)
	{
		GPIO_Init(led_gpio[i].GPIOx, led_gpio[i].GPIO_Pin, GPIO_Mode_Out_PP_High_Slow);
	}
	
	return 0;
}

/*******************************************************************************
 * ����: vkLedsSet
 * ����: ����Led״̬
 * �β�: state=0����state=1����
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkLedsSet(vkLED led, uint8_t state)
{
	if(led >=LEDMAX)
	{
		return -1;
	}
	
	int i = (int)led;
	
	(state == 0)?(GPIO_SetBits(led_gpio[i].GPIOx, led_gpio[i].GPIO_Pin)):(GPIO_ResetBits(led_gpio[i].GPIOx, led_gpio[i].GPIO_Pin));
	
	return 0;
}
