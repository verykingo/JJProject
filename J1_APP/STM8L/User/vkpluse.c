/******************************************************************************
 * �ļ�  ��vkpluse.c
 * ����    ���缫����
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include "stdlib.h"
#include <math.h>
#include <string.h>

/* �����Զ���ͷ�ļ� */
#include "vkpluse.h"
#include "vktimetick.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */
static vkPLUSE gPluse0 = {0};
static vkPLUSE gPluse1 = {0};
static uint8_t gMutex = 0;
static uint8_t gDelay_times = 0;	//gPluse1��gPluse0�ͺ�ʱ��
static vkTIMER gDelay_Timer;		//gPluse1��gPluse0�ͺ�ʱ��

void vkPluse_Timer_Callback(void * pdata);
void gDelay_Timer_Callback(void * pdata);

int vkPluse_Init(void)
{
	/* ����0 */
	gPluse0.p_mode = 0;
	gPluse0.p_expect = 0;
	gPluse0.p_ration = 0;
	gPluse0.p_current = 0;
	
	gPluse0.p_duration = 0;
	gPluse0.p_interval = 0xFF;
	gPluse0.p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse0.p_poqflags = 0;
	gPluse0.p_poqtimer.timer_name 	= &gPluse0.p_poqtimer;
	gPluse0.p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse0.p_poqtimer.cb_data  	= &gPluse0;
	gPluse0.p_poqtimer.cb_ticks 	= 0;

	gPluse0.p_bodlevel = PLUSE_LEVEL_LOW;
	gPluse0.p_bodcount = 0;
	memset(gPluse0.p_bodvalue,0,PLUSE_COUNT_HIGH-1);

	gPluse0.p_s0_gpio = PLUSE0_S0_GPIO;
	gPluse0.p_s0_pin  = PLUSE0_S0_PIN;	
	gPluse0.p_s1_gpio = PLUSE0_S1_GPIO;
	gPluse0.p_s1_pin  = PLUSE0_S1_PIN;
	gPluse0.p_s2_gpio = PLUSE0_S2_GPIO;
	gPluse0.p_s2_pin  = PLUSE0_S2_PIN;
	gPluse0.p_s3_gpio = PLUSE0_S3_GPIO;
	gPluse0.p_s3_pin  = PLUSE0_S3_PIN;
	GPIO_Init(gPluse0.p_s0_gpio,gPluse0.p_s0_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse0.p_s1_gpio,gPluse0.p_s1_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse0.p_s2_gpio,gPluse0.p_s2_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse0.p_s3_gpio,gPluse0.p_s3_pin,GPIO_Mode_Out_PP_Low_Slow);

	gPluse0.p_adcchnl  = ADC_Channel_16;
	gPluse0.p_adc_gpio = PLUSE0_ADC_GPIO;
	gPluse0.p_adc_pin  = PLUSE0_ADC_PIN;
	GPIO_Init(gPluse0.p_adc_gpio,gPluse0.p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse0.pwm = PWM1;
	vkPWM_Init(gPluse0.pwm);	

	/* ����1 */
	gPluse1.p_mode = 0;
	gPluse1.p_expect = 0;
	gPluse1.p_ration = 0;
	gPluse1.p_current = 0;
	
	gPluse1.p_duration = 0;
	gPluse1.p_interval = 0xFF;
	gPluse1.p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse1.p_poqflags = 0;
	gPluse1.p_poqtimer.timer_name 	= &gPluse1.p_poqtimer;
	gPluse1.p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse1.p_poqtimer.cb_data  	= &gPluse1;
	gPluse1.p_poqtimer.cb_ticks 	= 0;

	gPluse1.p_bodlevel = PLUSE_LEVEL_LOW;
	gPluse1.p_bodcount = 0;
	memset(gPluse1.p_bodvalue,0,PLUSE_COUNT_HIGH-1);

	gPluse1.p_s0_gpio = PLUSE0_S0_GPIO;
	gPluse1.p_s0_pin  = PLUSE0_S0_PIN;	
	gPluse1.p_s1_gpio = PLUSE0_S1_GPIO;
	gPluse1.p_s1_pin  = PLUSE0_S1_PIN;
	gPluse1.p_s2_gpio = PLUSE0_S2_GPIO;
	gPluse1.p_s2_pin  = PLUSE0_S2_PIN;
	gPluse1.p_s3_gpio = PLUSE0_S3_GPIO;
	gPluse1.p_s3_pin  = PLUSE0_S3_PIN;
	GPIO_Init(gPluse1.p_s0_gpio,gPluse1.p_s0_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse1.p_s1_gpio,gPluse1.p_s1_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse1.p_s2_gpio,gPluse1.p_s2_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse1.p_s3_gpio,gPluse1.p_s3_pin,GPIO_Mode_Out_PP_Low_Slow);

	gPluse1.p_adcchnl  = ADC_Channel_16;
	gPluse1.p_adc_gpio = PLUSE0_ADC_GPIO;
	gPluse1.p_adc_pin  = PLUSE0_ADC_PIN;
	GPIO_Init(gPluse1.p_adc_gpio,gPluse1.p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse1.pwm = PWM2;
	vkPWM_Init(gPluse1.pwm);

	gDelay_Timer.timer_name = &gDelay_Timer;
	gDelay_Timer.cb_func  = gDelay_Timer_Callback;
	gDelay_Timer.cb_data  = NULL;
	gDelay_Timer.cb_ticks = 0;

	return 0;
}

/*
 * ��������ģʽ
 */
int vkPluseSetMode(uint8_t pluse, uint8_t mode)
{
	if(pluse == PLUSE0)
	{
		gPluse0.p_mode = mode;
	}
	else if(pluse == PLUSE1)
	{
		gPluse1.p_mode = mode;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * ���õ���ǿ��
 */
int vkPluseSetExpt(uint8_t pluse, uint8_t expert)
{
	if(pluse == PLUSE0)
	{
		gPluse0.p_expect = expert;
	}
	else if(pluse == PLUSE1)
	{
		gPluse1.p_expect = expert;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * �����������ʱ��ͼ��ʱ��
 */
int vkPluseSetTime(uint8_t pluse, uint8_t duration, uint8_t interval)
{
	if(pluse == PLUSE0)
	{
		gPluse0.p_duration = duration;
		gPluse0.p_interval = interval;
	}
	else if(pluse == PLUSE1)
	{
		gPluse1.p_duration = duration;
		gPluse1.p_interval = interval;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * ���������ͺ�ʱ��
 */
int vkPluseSetDelay(uint8_t delay)
{
	gDelay_times = delay;

	return 0;
}

/*
 * �����������
 */
int vkPluseStart(uint8_t pluse)
{	
	if(pluse == PLUSE0)
	{
		gPluse0.p_poqtimer.cb_ticks = vkSS_TO_TICKS(gPluse0.p_duration)/2;
		vkTimerInsert(&gPluse0.p_poqtimer);
		gPluse0.p_poqlevel = PLUSE_LEVEL_HIGH;
	}
	else if(pluse == PLUSE1)
	{		
		gPluse1.p_poqtimer.cb_ticks = vkSS_TO_TICKS(gPluse1.p_duration)/2;
		vkTimerInsert(&gPluse1.p_poqtimer);
		gPluse1.p_poqlevel = PLUSE_LEVEL_HIGH;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * ֹͣ�������
 */
int vkPluseStop(uint8_t pluse)
{	
	if(pluse == PLUSE0)
	{
		vkTimerCancel(&gPluse0.p_poqtimer);
		gPluse0.p_ration  = 0;
		gPluse0.p_current = 0;

		gPluse0.p_poqlevel = PLUSE_LEVEL_LOW;
		gPluse0.p_poqflags = 0;

		gPluse0.p_bodlevel = PLUSE_LEVEL_LOW;
		gPluse0.p_bodcount = 0;

		vkPWMStop(gPluse0.pwm);
			
		GPIO_ResetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin);	//�Ͽ�
		GPIO_ResetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);	//�Ͽ�
		GPIO_ResetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//�Ͽ�		
	}
	else if(pluse == PLUSE1)
	{
		vkTimerCancel(&gPluse1.p_poqtimer);
		gPluse1.p_ration  = 0;
		gPluse1.p_current = 0;

		gPluse1.p_poqlevel = PLUSE_LEVEL_LOW;
		gPluse1.p_poqflags = 0;

		gPluse1.p_bodlevel = PLUSE_LEVEL_LOW;
		gPluse1.p_bodcount = 0;

		vkPWMStop(gPluse1.pwm);
			
		GPIO_ResetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin);	//�Ͽ�
		GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//�Ͽ�
		GPIO_ResetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin);	//�Ͽ�
	}
	else
	{
		return -1;
	}

	return 0;
}


void vkPluse_Timer_Callback(void * pdata)
{
	vkPLUSE *pluse = (vkPLUSE *)pdata;
	if(pluse == NULL)
	{
		return;
	}
	/* ����ģʽ */
	if(pluse->p_mode == PLUSE_MODE_CONTINUE)
	{
		if(pluse->p_poqlevel == PLUSE_LEVEL_LOW)
		{
			pluse->p_poqtimer.cb_ticks 	= vkSS_TO_TICKS(pluse->p_duration)/2;
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_HIGH;
		}
		else if(pluse->p_poqlevel == PLUSE_LEVEL_HIGH)
		{
			pluse->p_poqtimer.cb_ticks 	= vkSS_TO_TICKS(pluse->p_interval)/2;
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_LOW;			
		}
	}
	/* ����ģʽ */
	else if(pluse->p_mode == PLUSE_MODE_ONLYONCE)
	{
		if(pluse->p_poqlevel == PLUSE_LEVEL_LOW)
		{
			pluse->p_poqtimer.cb_ticks 	= vkSS_TO_TICKS(pluse->p_duration)/2;
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_HIGH;
		}
		else if(pluse->p_poqlevel == PLUSE_LEVEL_HIGH)
		{			
			pluse->p_poqlevel = PLUSE_LEVEL_LOW;			
		}		
	}
}

void gDelay_Timer_Callback(void * pdata)
{
	return;
}

/*
 * ����PWMռ�ձ�
 */
inline uint8_t vkPluseRation(vkPWM pwm, uint8_t ration)
{
	vkPWMStart(pwm, PLUSE_PWM_PERIOD, ration);

	return 0;
}

/*
 * �Ե�������ǿ��
 */
inline uint8_t vkPluseAdjust(vkPWM pwm, uint8_t expect, uint8_t ration, uint8_t current)
{	
	uint8_t ret_ration = 0;
		
	/* ռ�ձȲ�Ϊ0��˵����·�е�ѹ������Ϊ0����·�Ͽ���˵���缫���䣬��ʱ���ٵ�ռ�ձ���ѹ*/
	if(ration!=0 && current ==0)
	{
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, 0);
	}
	else if(ration==0 && current <=2) 
	{
		ration = 2;
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, ration);
	}
	else
	{		
		/* ��Χ�������̷�Χ */
		if(fabs(current-expect)>PLUSE_PWM_TOLERANCE)
		{
			uint8_t step =(current==0)?1:(int)(fabs(current-expect)*(1/2)*0.7 + 1);
			
			if(current > expect)	//��ǰ���������趨������ռ�ձȼ�С
			{	
				ret_ration = (ration-step <=0)?0:(ration-step);
			}
			else if(current < expect)	//��ǰ����С���趨������ռ�ձ��������ֵ90
			{	
				ret_ration = (ration + step >=90)?90:(ration + step);
			}
		}
		
		//pwm�趨 //Ĭ��Ϊ100hz  10ms
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, ret_ration);
	}
	
	return ret_ration;
}


void vkPluseTick(void)
{
	//�缫A���ʱ��
	if(gPluse0.p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse0.p_poqflags == 0)
		{
			gPluse0.p_poqflags = 1;
			
			vkPWMStop(gPluse0.pwm);

			//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
			GPIO_ResetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin);	//�Ͽ�
			//�ڶ�·�������õ͵�ƽ������3.3V
			GPIO_ResetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);	//�Ͽ�
	
			GPIO_ResetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//�Ͽ�
		}
	}

	//�缫B���ʱ��
	if(gPluse1.p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse1.p_poqflags == 0)
		{
			gPluse1.p_poqflags = 1;

			vkPWMStop(gPluse1.pwm);

			//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
			GPIO_ResetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin);	//�Ͽ�
			//�ڶ�·�������õ͵�ƽ������3.3V
			GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//�Ͽ�
	
			GPIO_ResetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin);	//�Ͽ�
		}
	}

	//�缫A����ʱ��
	if(gPluse0.p_poqlevel == PLUSE_LEVEL_HIGH)
	{	
		if(gPluse0.p_bodlevel == PLUSE_LEVEL_LOW)	//30ms
		{	
			gPluse0.p_bodcount++;
					
			if(gPluse0.p_bodcount==1)	//��һ���������͵�ƽ����ʼ���	
			{
				//�ڶ�·�������øߵ�ƽ���ӵأ�Ѹ�����͵缫��ѹOV
				GPIO_SetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);

				GPIO_ResetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//�Ͽ�
				
				//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
				GPIO_ResetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin);	//�Ͽ�	
			}			
			else if(gPluse0.p_bodcount == PLUSE_MUTEX_DELAY) //��ʱ PLUSE_MUTEX_DELAY �ͷŻ�����
			{
				//�ͷŻ�����
				gMutex = 0;	

				//�ڶ�·�������õ͵�ƽ��
				GPIO_ResetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);			//�Ͽ�				
			}			
			else if(gPluse0.p_bodcount == (PLUSE_MUTEX_DELAY+PLUSE_COUNT_HIGH+1)) //�������ʱ�䵽
			{
				/* PE2��1 */
				//GPIOE->ODR |= GPIO_Pin_2;
				
				//�������������ռ�ձ�
				//�������,�˴������Ż�
				
				uint16_t adc_V = vkADC_Read(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;
				for(uint8_t i=0; i<(PLUSE_COUNT_HIGH-1); i++)
				{
					adc_T += gPluse0.p_bodvalue[i];
				}
				adc_T = adc_T >>2;
				gPluse0.p_current = (uint16_t)((((1000*1.200f)/adc_V)*adc_T)/10.0f);
				//��������ǿ�Ȳ�����PWM����ʼ��磬���۵���				
				vkPluseRation(gPluse0.pwm, 50);
				//gPluse0.p_ration = vkPluseAdjust(gPluse0.pwm, gPluse0.p_expect, gPluse0.p_ration, gPluse0.p_current);	

				/* PE2��0 */
				//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);				
			}			
			else if(gPluse0.p_bodcount >= PLUSE_COUNT_LOW) //����ʱ�䵽
			{
				if(gMutex == 0)
				{
					//���ü������ı��ƽ״̬
					gPluse0.p_bodcount = 0;
					gPluse0.p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse0.p_bodlevel == PLUSE_LEVEL_HIGH)	//200us
		{
			//ռ�û�����
			gMutex = 1;							
			gPluse0.p_bodcount++;
			
			/* PE2��1 */
			//GPIOE->ODR |= GPIO_Pin_2;
			//��һ���������ߵ�ƽ����ʼ�ŵ�
			if(gPluse0.p_bodcount == 1)
			{
				//�ر�PWM���Ѿ������˵�������ʼ�ŵ�							
				vkPWMStop(gPluse0.pwm);

				GPIO_SetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//����		
				
				//�ڶ�·�������õ͵�ƽ�����ӵ�
				//�ڶ�·�������õ͵�ƽ������3.3V
				GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//�Ͽ�
				
				//��һ·�������øߵ�ƽ���缫�ŵ�0.1ms
				GPIO_SetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin); //����
			}			
			else if(gPluse0.p_bodcount == PLUSE_COUNT_HIGH)	//����ʱ�䵽5*40us = 200us
			{
				/* ���һ�βɼ� */
				gPluse0.p_bodvalue[gPluse0.p_bodcount-2] = vkADC_Read(gPluse0.p_adcchnl);
				
				//���ü������ı��ƽ״̬
				gPluse0.p_bodcount = 0;				
				gPluse0.p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse0.p_poqflags = 0;
			}
			//��һ�β���ADC
			else
			{
				//�ɼ�ADC
				gPluse0.p_bodvalue[gPluse0.p_bodcount-2] = vkADC_Read(gPluse0.p_adcchnl);
			}

			/* PE2��0 */
			//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);
		}
	}

	//�缫B����ʱ��
	if(gPluse1.p_poqlevel == PLUSE_LEVEL_HIGH)	
	{	
		if(gPluse1.p_bodlevel == PLUSE_LEVEL_LOW)	//30ms
		{	
			gPluse1.p_bodcount++;			
			
			if(gPluse1.p_bodcount == 1)	//��1���������͵�ƽ����ʼ���			
			{
				//�ڶ�·�������øߵ�ƽ���ӵأ�Ѹ�����͵缫��ѹOV
				GPIO_SetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);		//����

				GPIO_ResetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin);	//�Ͽ�
				
				//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
				GPIO_ResetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin);	//�Ͽ�	
			}			
			else if(gPluse1.p_bodcount == PLUSE_MUTEX_DELAY)	//��ʱ PLUSE_MUTEX_DELAY �ͷŻ�����
			{
				//�ͷŻ�����
				gMutex = 0;	

				//�ڶ�·�������õ͵�ƽ��
				GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//�Ͽ�				
			}			
			else if(gPluse1.p_bodcount == (PLUSE_MUTEX_DELAY+PLUSE_COUNT_HIGH+1)) //�������ʱ�䵽
			{
				/* PE2��1 */
				//GPIOE->ODR |= GPIO_Pin_2;
				
				//�������������ռ�ձ�
				//�������,�˴������Ż�
				
				uint16_t adc_V = vkADC_Read(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;		
				for(uint8_t i=0; i<(PLUSE_COUNT_HIGH-1); i++)
				{
					adc_T += gPluse1.p_bodvalue[i];
				}
				adc_T = adc_T >>2;
				gPluse1.p_current = (uint16_t)((((1000*1.200f)/adc_V)*adc_T)/10.0f);
				//��������ǿ�Ȳ�����PWM����ʼ��磬���۵���				
				vkPluseRation(gPluse1.pwm, 50);
				//gPluse1.p_ration = vkPluseAdjust(gPluse1.pwm, gPluse1.p_expect, gPluse1.p_ration, gPluse0.p_current);	

				/* PE2��0 */
				//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2); 			
			}			
			else if(gPluse1.p_bodcount >= PLUSE_COUNT_LOW) //����ʱ�䵽
			{
				if(gMutex == 0)
				{
					//���ü������ı��ƽ״̬
					gPluse1.p_bodcount = 0;
					gPluse1.p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse1.p_bodlevel == PLUSE_LEVEL_HIGH) //200us
		{
			//ռ�û�����
			gMutex = 1;
			gPluse1.p_bodcount++;
			
			/* PE2��1 */
			//GPIOE->ODR |= GPIO_Pin_2;
			//��һ���������ߵ�ƽ����ʼ�ŵ�
			if(gPluse1.p_bodcount == 1)
			{
				//�ر�PWM���Ѿ������˵�������ʼ�ŵ�							
				vkPWMStop(gPluse1.pwm);

				GPIO_SetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin); 	//����		
				
				//�ڶ�·�������õ͵�ƽ�����ӵ�
				//�ڶ�·�������õ͵�ƽ������3.3V
				GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//�Ͽ�
				
				//��һ·�������øߵ�ƽ���缫�ŵ�0.1ms
				GPIO_SetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin); 	//����
			}			
			else if(gPluse1.p_bodcount == PLUSE_COUNT_HIGH)		//����ʱ�䵽5*40us = 200us
			{
				/* ���һ�βɼ� */
				gPluse1.p_bodvalue[gPluse1.p_bodcount-2] = vkADC_Read(gPluse1.p_adcchnl);
								
				//���ü������ı��ƽ״̬
				gPluse1.p_bodcount = 0; 			
				gPluse1.p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse1.p_poqflags = 0;
			}
			else
			{
				//�ɼ�ADC
				gPluse1.p_bodvalue[gPluse1.p_bodcount-2] = vkADC_Read(gPluse1.p_adcchnl);
			}

			/* PE2��0 */
			//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);
		}
	}
}



