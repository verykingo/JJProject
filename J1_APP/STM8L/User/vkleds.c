/******************************************************************************
 * �ļ�  ��vkleds.c
 * ����    ��ledָʾ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-09-04

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include <stdio.h>
#include <stdlib.h>

/* �����Զ���ͷ�ļ� */
#include "vkleds.h"
#include "vktimetick.h"

/* �Զ��������� */

/* �Զ���� */
#define LED_REPEAT_END(x) 	{x + LED_MODE_COUNT-1, 0}
#define LED_NEXT_MODE(x)	{x, 0}
#define LED_ALIGN3			{0, 0}, {0, 0}, {0, 0}
#define LED_ALIGN2			{0, 0}, {0, 0}
#define LED_ALIGN1			{0, 0}

/* ȫ�ֱ������� */
static vkLEDS gLeds = {

	.led_gpio = {
		{GPIOE, GPIO_Pin_0},
 		{GPIOE, GPIO_Pin_1}
	},
	
	.led_mtable = {
		/*Connected*/	
		{.mode = LED_MODE_CONNECTED,.stat = {{LED_STATE_GREEN_ON, 1000},	LED_NEXT_MODE(LED_MODE_CONNECTED),	LED_ALIGN1}},
		/*Unconnect*/	
		{.mode = LED_MODE_UNCONNECT,.stat = {{LED_STATE_GREEN_ON, 1000},	{LED_STATE_GREEN_OFF, 1000},		LED_NEXT_MODE(LED_MODE_UNCONNECT)}},
		/*Charging*/
		{.mode = LED_MODE_CHARGING,	.stat = {{LED_STATE_RED_ON, 1000},		{LED_STATE_RED_OFF, 1000},			LED_NEXT_MODE(LED_MODE_CHARGING)}},
		/*Lowbatt*/ 	
		{.mode = LED_MODE_LOWBATT,	.stat = {{LED_STATE_RED_ON, 500},		{LED_STATE_RED_OFF, 500},			LED_NEXT_MODE(LED_MODE_LOWBATT)}},
		/*Poweron */
		{.mode = LED_MODE_POWERON,	.stat = {{LED_STATE_ALL_ON, 1000},		LED_NEXT_MODE(LED_MODE_POWERON), 	LED_ALIGN1}},
		/*Shutdown*/
		{.mode = LED_MODE_SHUTDOWN,	.stat = {{LED_STATE_ALL_OFF, 1000}, 	LED_NEXT_MODE(LED_MODE_SHUTDOWN),	LED_ALIGN1}},
		/*Runerror*/
		{.mode = LED_MODE_RUNERROR,	.stat = {{LED_STATE_ALL_ON, 500},		{LED_STATE_ALL_OFF, 500},			LED_NEXT_MODE(LED_MODE_SHUTDOWN)}},
		/*Test*/
		{.mode = LED_MODE_TEST,		.stat = {{LED_STATE_ALL_ON, 1000},		{LED_STATE_ALL_OFF, 1000},			LED_NEXT_MODE(LED_MODE_TEST)}}		
	},
	
	.led_mlevel = {
		{LED_MODE_SHUTDOWN,	0},			
		{LED_MODE_RUNERROR,	0},			
		{LED_MODE_LOWBATT,	0},
		{LED_MODE_CHARGING,	0},	
		{LED_MODE_UNCONNECT,0},
		{LED_MODE_CONNECTED,0},
		{LED_MODE_POWERON,	0}
	},
	.led_mode = 0
};

/*******************************************************************************
 * ����: vkLeds_Init
 * ����: Led�Ƴ�ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
int vkLeds_Init()
{
	/* GPIO���ų�ʼ�� */
	for(int i=0; i<LEDMAX; i++)
	{
		GPIO_Init(gLeds.led_gpio[i].GPIOx, gLeds.led_gpio[i].GPIO_Pin, GPIO_Mode_Out_PP_High_Slow);
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
int vkLedsSet(uint8_t led, uint8_t state)
{
	if(led >=LEDMAX)
	{
		return -1;
	}
	
	int i = (int)led;
	
	(state == 0)?(GPIO_SetBits(gLeds.led_gpio[i].GPIOx, gLeds.led_gpio[i].GPIO_Pin)):(GPIO_ResetBits(gLeds.led_gpio[i].GPIOx, gLeds.led_gpio[i].GPIO_Pin));
	
	return 0;
}

int vkLedsSetState(uint8_t state)
{
	switch (state)
	{
		case LED_STATE_RED_OFF:
		{
			vkLedsSet(LEDR, 0);					
			break;
		}
		case LED_STATE_RED_ON:
		{
			vkLedsSet(LEDR, 1);
			vkLedsSet(LEDG, 0);
			break;
		}
		case LED_STATE_GREEN_OFF:
		{
			vkLedsSet(LEDG, 0);					
			break;
		}
		case LED_STATE_GREEN_ON:
		{
			vkLedsSet(LEDG, 1);
			vkLedsSet(LEDR, 0);
			break;
		}
		case LED_STATE_ALL_OFF: 
		{
			vkLedsSet(LEDG, 0);
			vkLedsSet(LEDR, 0);
			break;
		}
		case LED_STATE_ALL_ON:
		{
			vkLedsSet(LEDG, 1);
			vkLedsSet(LEDR, 1);
			break;
		}
		default:
			break;
	}

	return 0;
}

/*
 * 			����LEDָʾ��ģʽ
 *          time=-1 һֱ����
 *			time=0 ֹͣ
 *			time>0 ����time��ʱ��
 */
int vkLedsSetMode(uint8_t mode, int16_t duration)
{
	int ret = 0;
	int i = 0;
	
	if(duration < -1)
	{
		return -1;
	}
	

	/* ���ȼ��� */
	for(i=0; i<LED_MODE_COUNT; i++)
	{
		if(mode == gLeds.led_mlevel[i].mode)
		{
			if(gLeds.led_mlevel[i].enable == 1 && (duration==-1 || duration>0))
				return -1;
			if(gLeds.led_mlevel[i].enable == 0 && duration==0)
				return -1;
			break;
		}
	}

	/* ��ͣ��ʱ�� */
	vkTimerCancel(&gLeds.led_timer);

	/* �������ȼ��� */

	if(duration == -1 || duration > 0)		//һֱ����
	{
		gLeds.led_mlevel[i].enable = 1;
	}
	else if(duration == 0)	//ֹͣ
	{
		gLeds.led_mlevel[i].enable = 0;
	}

	/* ����ģʽ�� */
	if(duration == -1 || duration == 0)
	{		
		for(int i=0; i<LED_MODE_COUNT; i++)
		{
			if(mode == gLeds.led_mtable[i].mode)
			{
				switch(mode)
				{
					case LED_MODE_CONNECTED:
					case LED_MODE_POWERON:
					case LED_MODE_SHUTDOWN:
					{
						gLeds.led_mtable[i].stat[1].led_stat = mode;
						gLeds.led_mtable[i].stat[1].led_time = 0;
						break;
					}
					case LED_MODE_UNCONNECT:
					case LED_MODE_CHARGING:
					case LED_MODE_LOWBATT:
					case LED_MODE_RUNERROR:
					case LED_MODE_TEST:
					{
						gLeds.led_mtable[i].stat[2].led_stat = mode;
						gLeds.led_mtable[i].stat[2].led_time = 0;
						break;
					}
				}
				
				break;
			}
		}
	}
	else if(duration > 0)
	{
		for(int i=0; i<LED_MODE_COUNT; i++)
		{
			if(mode == gLeds.led_mtable[i].mode)
			{
				switch(mode)
				{
					case LED_MODE_CONNECTED:
					case LED_MODE_POWERON:
					case LED_MODE_SHUTDOWN:
					{
						gLeds.led_mtable[i].stat[1].led_stat = duration + LED_MODE_COUNT;
						gLeds.led_mtable[i].stat[1].led_time = 0;
						break;
					}
					case LED_MODE_LOWBATT:
					case LED_MODE_RUNERROR:
					{
						gLeds.led_mtable[i].stat[2].led_stat = duration + LED_MODE_COUNT;
						gLeds.led_mtable[i].stat[2].led_time = 0;
						break;
					}					
					case LED_MODE_UNCONNECT:
					case LED_MODE_CHARGING:
					case LED_MODE_TEST:
					{
						gLeds.led_mtable[i].stat[2].led_stat = duration/2 + LED_MODE_COUNT;
						gLeds.led_mtable[i].stat[2].led_time = 0;
						break;
					}			
				}

				break;
			}
		}
	}

	/* ����ȡ���ȼ����ģʽ */
	vkLedsChekMode();
	vkLedsNextBout();

	/* ��ȡLED״̬ */
	uint16_t led_stat, led_time;
	vkLedsGetStat(&led_stat, &led_time);

	/* ����LED��״̬ */
	vkLedsSetState(led_stat);

	/* ������ʱ�� */
	gLeds.led_timer.cb_ticks = vkMS_TO_TICKS(led_time);
	vkTimerInsert(&gLeds.led_timer);
	
	return ret;
}

int vkLedsGetMode(uint8_t mode)
{

	return 0;	
}

/*���ڹ�����״̬��ʱ���ص����� */
void vkLeds_Timer_Callback(void *pdata)
{
	uint16_t led_stat, led_time;
		
	/* ����ȡ���ȼ����ģʽ */
	vkLedsChekMode();
	
	/* ��ȡLED״̬ */
	vkLedsGetStat(&led_stat, &led_time);

	/* һ��LED״̬ѭ������������ѭ��/��ʱʱ�䵽/��ʱ��δ�� */
	if(led_time == 0)
	{		
		if(led_stat < LED_MODE_COUNT)
		{
			/* ����ѭ������һ��ѭ�� */
			vkLedsNextBout();

			/* ��ȡLED״̬ */
			vkLedsGetStat(&led_stat, &led_time);

			/* ����LED��״̬ */
			vkLedsSetState(led_stat);			
		}		
		/* ģʽ��������һ��ģʽ */
		else if(led_stat == LED_MODE_COUNT)
		{	
			/* �л�ģʽ */
			vkLedsNextMode();

			vkLedsNextBout();
			
			/* ��ȡLED״̬ */
			vkLedsGetStat(&led_stat, &led_time);

			/* ����LED��״̬ */
			vkLedsSetState(led_stat);
		}
		else if(led_stat > LED_MODE_COUNT)
		{
			/* ��ʱ�ڣ���һ��ѭ�� */
			vkLedsNextBout();
			
			/* ��ȡLED״̬ */
			vkLedsGetStat(&led_stat, &led_time);

			/* ����LED��״̬ */
			vkLedsSetState(led_stat);			
		}
	}
	else if(led_time > 0)
	{
		/* ����LED��״̬ */
		vkLedsSetState(led_stat);
	}
	
	gLeds.led_timer.cb_ticks = vkMS_TO_TICKS(led_time);
	vkTimerInsert(&gLeds.led_timer);
}

int vkLedsStart()
{
	uint16_t led_stat, led_time;
		
	/* ֹͣ��ʱ�� */
	vkTimerCancel(&gLeds.led_timer);

	/* ����ȡ���ȼ����ģʽ */
	vkLedsChekMode();
	vkLedsNextBout();
	
	/* ��ȡLED״̬ */
	vkLedsGetStat(&led_stat, &led_time);

	/* ����LED��״̬ */
	vkLedsSetState(led_stat);

	/* ���ö�ʱ���ص���1000msִ��һ�λص����� */
	gLeds.led_timer.timer_name = (void *)&gLeds.led_timer;
	gLeds.led_timer.cb_func 	= vkLeds_Timer_Callback;
	gLeds.led_timer.cb_data 	= NULL;
	gLeds.led_timer.cb_ticks	= vkMS_TO_TICKS(led_time);

	/* ������ʱ�� */	
	if(vkTIMER_OK != vkTimerInsert(&gLeds.led_timer))
	{
		return -1;
	}

	return 0;	
}

/* ����ȡ������ȼ�ģʽ */
int vkLedsChekMode()
{	
	uint8_t mode_pre = gLeds.led_mode;
	
	for(int i=0; i<LED_MODE_COUNT; i++)
	{
		if(gLeds.led_mlevel[i].enable == 1)
		{
			gLeds.led_mode = gLeds.led_mlevel[i].mode;	
		
			goto EXIT;
		}
	}

	/* �������κ�ģʽ��ǿ��ת���ɿ���ģʽ */
	gLeds.led_mode = LED_MODE_POWERON;

EXIT:
	if(mode_pre != gLeds.led_mode)
	{
		/* �л���ģʽ */
		vkLedsNextBout();
	}

	return 0;
}

int vkLedsNextMode()
{
	/* �ر�ģʽ */
	vkLedsSetMode(gLeds.led_mode, 0);

	/* ����ȡ������ȼ�ģʽ */
	vkLedsChekMode();

	/* ״̬������0 */	
	vkLedsNextBout();
	
    return 0;
}

/* ͬ��ģʽ��һ���ֻ� */
int vkLedsNextBout()
{
	/* ״̬������0 */	
	gLeds.led_index = 0;
    
    return 0;
}

int vkLedsGetStat(uint16_t *led_stat, uint16_t *led_time)
{
	for(int i=0; i<LED_MODE_COUNT; i++)
	{
		if(gLeds.led_mode == gLeds.led_mtable[i].mode)
		{
			*led_stat = gLeds.led_mtable[i].stat[gLeds.led_index].led_stat;
			*led_time = gLeds.led_mtable[i].stat[gLeds.led_index].led_time;
			if(*led_time == 0 && *led_stat > LED_MODE_COUNT)
			{
				gLeds.led_mtable[i].stat[gLeds.led_index].led_stat--;
			}

			gLeds.led_index++;
			
			return 0;
		}
	}

	*led_stat = 0;
	*led_time = 0;
		
	return -1;
}

