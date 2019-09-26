
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
#include "vksofttimer.h"
#include "vkpwm.h"
#include "vkADC.h"

#define PLUSE_PWM_FREQUENCY		5600ul
#define PLUSE_PWM_PERIOD   		(uint32_t)((1000ul*1000ul)/PLUSE_PWM_FREQUENCY)
#define PLUSE_PWM_TOLERANCE 	2

#define PLUSE_TICK_TIME 		40	//40us
#define PLUSE_LEVEL_HIGH		1
#define PLUSE_LEVEL_LOW			0

#define PLUSE_MUTEX_DELAY   	5
#define PLUSE_COUNT_HIGH    	5
#define PLUSE_COUNT_LOW     	745

#define PLUSE_MODE_CONTINUE		0x01	//ѭ��ģʽ	
#define PLUSE_MODE_ONLYONCE		0x02	//����ģʽ

#define PLUSE0_S0_GPIO			GPIOD
#define PLUSE0_S0_PIN			GPIO_Pin_7	
#define PLUSE0_S1_GPIO			GPIOD
#define PLUSE0_S1_PIN			GPIO_Pin_3	
#define PLUSE0_S2_GPIO			GPIOD	
#define PLUSE0_S2_PIN			GPIO_Pin_5
#define PLUSE0_S3_GPIO			GPIOE
#define PLUSE0_S3_PIN			GPIO_Pin_2
#define PLUSE0_ADC_GPIO			GPIOB
#define PLUSE0_ADC_PIN			GPIO_Pin_2

#define PLUSE1_S0_GPIO			GPIOF
#define PLUSE1_S0_PIN			GPIO_Pin_0	
#define PLUSE1_S1_GPIO			GPIOD
#define PLUSE1_S1_PIN			GPIO_Pin_1
#define PLUSE1_S2_GPIO			GPIOD
#define PLUSE1_S2_PIN			GPIO_Pin_6
#define PLUSE1_S3_GPIO			GPIOE
#define PLUSE1_S3_PIN			GPIO_Pin_3
#define PLUSE1_ADC_GPIO			GPIOE
#define PLUSE1_ADC_PIN			GPIO_Pin_5


/* �Զ����������� */
enum PLUSE
{
	PLUSE0 = 0,
	PLUSE1 = 1
};

typedef struct pluse
{
	uint8_t  p_mode;		//ģʽһ��/ѭ��
	uint8_t  p_expect;		//Ԥ�����mA
	uint8_t  p_ration;		//��ǰռ�ձ�
	uint8_t  p_current;		//��ǰ����mA
	
	uint8_t  p_duration;	//����ʱ��
	uint8_t  p_interval;	//���ʱ��	
	uint8_t  p_poqlevel;	//�ϲ�״̬
	uint8_t  p_poqflags;	//�ϲ���
	vkTIMER  p_poqtimer;	//�ϲ㶨ʱ��
	
	uint8_t  p_bodlevel;	//�²�״̬
	uint16_t p_bodcount;	//�²����	
	uint16_t p_bodvalue[PLUSE_COUNT_HIGH-1];	//ADC����ǿ��ֵ

	GPIO_TypeDef* p_s0_gpio;	//���翪��S0
	uint8_t		  p_s0_pin;			
	GPIO_TypeDef* p_s1_gpio;	//�������S1
	uint8_t		  p_s1_pin;
	GPIO_TypeDef* p_s2_gpio;	//�ӵؿ���S2
	uint8_t		  p_s2_pin;
	GPIO_TypeDef* p_s3_gpio;	//��⿪��S3
	uint8_t		  p_s3_pin;

	ADC_Channel_TypeDef p_adcchnl;	//ADC�������ͨ��
	GPIO_TypeDef* 		p_adc_gpio;	//ADC�������GPIO
	uint8_t		  		p_adc_pin;	//ADC�����������
	
	vkPWM pwm;
} vkPLUSE;

int vkPluse_Init(void);

int vkPluseSetMode(uint8_t pluse, uint8_t mode);
int vkPluseSetExpt(uint8_t pluse, uint8_t expert);
int vkPluseSetTime(uint8_t pluse, uint8_t duration, uint8_t interval);
int vkPluseSetDelay(uint8_t delay);
int vkPluseStart(uint8_t pluse);
int vkPluseStop(uint8_t pluse);

void vkPluseTick(void);
uint8_t vkPluseAdjust(vkPWM pwm, uint8_t expect, uint8_t ration, uint8_t current);
inline uint8_t vkPluseRation(vkPWM pwm, uint8_t ration);

#endif
