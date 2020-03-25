
/******************************************************************************
 * �ļ�  ��vkleds.h
 * ����    ��ledָʾ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-09-04

*******************************************************************************/

#ifndef __VKLEDS_H
#define __VKLEDS_H

/* ����ϵͳͷ�ļ� */
#include "vksofttimer.h"

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */

typedef void (*vkLEDS_MODE_CB)(void *);

typedef struct
{
	GPIO_TypeDef* GPIOx; 
	uint8_t GPIO_Pin;
} vkLEDGPIO;

enum LED
{
	LED0 = 0,
	LED1,
	LEDMAX
};

#define LEDG LED0
#define LEDR LED1

enum LEDSTATE
{
	LED_STATE_RED_OFF 		= 0x00,
	LED_STATE_RED_ON 		= 0x01,
	LED_STATE_GREEN_OFF 	= 0x02,
	LED_STATE_GREEN_ON 		= 0x03,
	LED_STATE_ALL_OFF 		= 0x04,
	LED_STATE_ALL_ON 		= 0x05,
};
	
/* LED״̬�ṹ�� */
typedef struct
{
	uint16_t 	led_stat;	//led״̬
	uint16_t 	led_time;	//ledʱ��
} vkLEDSTAT;

enum LEDMODE
{
	LED_MODE_CONNECTED = 0,	//���������ӣ��̵Ƴ���
	LED_MODE_UNCONNECT,		//����δ���ӣ��̵���˸��1s
	LED_MODE_CHARGING,		//�������磺�����˸��1s
	LED_MODE_LOWBATT,		//��ص͵����������˸��0.5s
	LED_MODE_POWERON,		//����:	���̵Ƴ���	
	LED_MODE_SHUTDOWN,		//�ػ������̵���
	LED_MODE_RUNERROR,		//���г������̵ƽ�����˸��0.5s
	LED_MODE_TEST,			//���ԣ����̵ƽ�����˸,1s
	LED_MODE_COUNT			//ģʽ����
};

typedef struct LEDLEVEL
{
	uint8_t 	mode;		//ģʽ
	uint8_t 	enable;		//ʹ��
} vkLEDLEVEL;

typedef struct
{
	uint8_t	  mode;			//ģʽ
	vkLEDSTAT stat[3];		//״̬
} vkLEDMSTAT;


/* LED״̬�ṹ�� */
typedef struct
{
	vkLEDGPIO	led_gpio[LEDMAX];
	
	vkLEDMSTAT 	led_mtable[LED_MODE_COUNT];
	vkLEDLEVEL	led_mlevel[LED_MODE_COUNT];
	
	uint8_t		led_mode;
	uint8_t 	led_index;

	vkTIMER		led_timer;
} vkLEDS;

int vkLeds_Init();
int vkLedsSet(uint8_t led, uint8_t state);

int vkLedsStart();
int vkLedsStop();

int vkLedsSetMode(uint8_t mode, int16_t duration);
int vkLedsGetMode(uint8_t mode);

int vkLedsChekMode();
int vkLedsNextMode();
int vkLedsNextBout();

int vkLedsGetStat(uint16_t *led_stat, uint16_t *led_time);

#endif
