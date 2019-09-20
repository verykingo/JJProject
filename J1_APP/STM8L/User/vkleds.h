
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
	uint8_t GPIO_Pin;
} LED_GPIO;

typedef enum led
{
	LED0 = 0,
	LED1,
	LEDMAX
} vkLED;

/* �Զ������ */
const static struct LED_GPIO led_gpio[] = 	{{GPIOE, GPIO_Pin_0},
											 {GPIOE, GPIO_Pin_1}	
											};

int vkLeds_Init();
int vkLedsSet(vkLED led, uint8_t state);

#endif
