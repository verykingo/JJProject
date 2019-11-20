
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

#define PLUSE_TICK_TIME 		50		//һ��Tick��50us
#define PLUSE_LEVEL_HIGH		1
#define PLUSE_LEVEL_LOW			0

#define PLUSE_TICKS_PER_MS		(1000/PLUSE_TICK_TIME)	//һ����TICK��

#define PLUSE_COUNT_WIDTH_MAX   (300/PLUSE_TICK_TIME)	//����ߵ�ƽ������������300us/50us=6��

#define PLUSE_MODE_ONLYONCE		0x01	//����ģʽ
#define PLUSE_MODE_CONTINUE		0x02	//ѭ��ģʽ	

#define PLUSE_VREFINT			1.200f	//оƬ�ڲ��ο���ѹֵ	
#define PLUSE0_RESISTANCE		10.0f	//����0ǿ�Ȳο�����	
#define PLUSE1_RESISTANCE		10.0f	//����1ǿ�Ȳο�����


/* �缫A���غ�ADC GPIO���� */
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

/* �缫B���غ�ADC GPIO */
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

#define PLUSE_EXPECT_MAX 80

/* �Զ����������� */
enum PLUSE
{
	PLUSE0 = 0,
	PLUSE1 = 1
};

typedef struct pluse
{
	uint8_t  p_mode;		//ģʽ����/ѭ��
	uint8_t  p_expect;		//Ԥ�����0~80mA
	uint8_t  p_current;		//��ǰ����0~80mA
	uint8_t  p_ration;		//��ǰռ�ձ�0~100%

	uint16_t p_rise;		//һ������̼�����ʱ��(0~500ms��������ʱ��)
	uint16_t p_fall;		//һ������̼��½�ʱ��(0~500ms��������ʱ��)
	
	uint16_t p_duration;	//����ʱ��ms
	uint16_t p_interval;	//���ʱ��ms		
	uint16_t p_poqcount;	//�ϲ������1������1ms	
	uint8_t  p_1mscount;	//1�������
	uint8_t  p_poqlevel;	//�ϲ�״̬
	uint8_t  p_poqflags;	//�ϲ���	
	vkTIMER  p_poqtimer;	//�ϲ㶨ʱ��
	
	uint16_t p_freqy;		//����Ƶ��Hz(17/20/25/33/40)
	uint16_t p_width;		//100us~300us/50
	uint16_t p_tickfreqy;	//����ʱ��Tick��Ŀ
	uint16_t p_tickwidth;	//����ʱ��Tick��Ŀ		
	uint16_t p_bodcount;	//�²����	
	uint16_t p_bodvalue[PLUSE_COUNT_WIDTH_MAX-1];	//ADC����ǿ��ֵ
	uint8_t  p_bodlevel;	//�²�״̬

	GPIO_TypeDef* 		p_s0_gpio;	//���翪��S0
	GPIO_Pin_TypeDef	p_s0_pin;			
	GPIO_TypeDef* 		p_s1_gpio;	//�������S1
	GPIO_Pin_TypeDef	p_s1_pin;
	GPIO_TypeDef* 		p_s2_gpio;	//�ӵؿ���S2
	GPIO_Pin_TypeDef	p_s2_pin;
	GPIO_TypeDef* 		p_s3_gpio;	//��⿪��S3
	GPIO_Pin_TypeDef	p_s3_pin;

	ADC_Channel_TypeDef p_adcchnl;	//ADC�������ͨ��
	GPIO_TypeDef* 		p_adc_gpio;	//ADC�������GPIO
	GPIO_Pin_TypeDef	p_adc_pin;	//ADC�����������
	
	vkPWM pwm;
} vkPLUSE;

int vkPluse_Init(void);
int vkPluse_Deinit(void);

int vkPluseSetMode(uint8_t pluse, uint8_t mode);
int vkPluseSetExpt(uint8_t pluse, uint8_t expert);
int vkPluseSetTime(uint8_t pluse, uint16_t duration, uint16_t interval);
int vkPluseSetWidth(uint8_t pluse, uint16_t width);
int vkPluseSetFrequency(uint8_t pluse, uint16_t frequency);
int vkPluseSetRise(uint8_t pluse, uint16_t rise);
int vkPluseSetFall(uint8_t pluse, uint16_t fall);

int vkPluse1MSAdd(uint8_t pluse, uint8_t value);
int vkPluse1MSSet(uint8_t pluse, uint8_t value);

uint8_t vkPluseGetExpt(uint8_t pluse);
uint8_t vkPluseGetPoqL(uint8_t pluse);

int vkPluseStart(uint8_t pluse);
int vkPluseStop(uint8_t pluse);
int vkPluseGetIdle(void);

void vkPluseTick(void);
uint8_t vkPluseAdjust(uint8_t pluse, uint8_t expect, uint8_t ration, uint8_t current);
uint8_t vkPluseRation(uint8_t pluse, uint8_t ration);
uint8_t vkPluseTrimExpt(uint8_t expect ,uint16_t duration, uint16_t rise, uint16_t fall, uint16_t poqcount);

#endif
