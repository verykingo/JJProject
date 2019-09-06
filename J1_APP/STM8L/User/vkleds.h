
/******************************************************************************
 * �ļ�  ��vkleds.h
 * ����    ��ledָʾ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-09-04

*******************************************************************************/

#ifndef __VKLEDS_H
#define __VKLEDS_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */
typedef struct LED_GPIO
{
	GPIO_TypeDef* GPIOx; 
	uint16_t GPIO_Pin;
} LED_GPIO;

enum vkLED
{
	LED0 = 0,
	LED1,
	LEDMAX
};

/* �Զ������ */
const static struct LED_GPIO led_gpio[] = 	{{GPIOE, GPIO_Pin_0},
											 {GPIOE, GPIO_Pin_1}	
											};

int vkLeds_Init();
int vkLedsSet(enum vkLED led, int state);

#endif
