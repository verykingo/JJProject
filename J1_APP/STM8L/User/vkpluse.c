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
#define PLUSE_USING_POQ_SOFTTIMER 0u
#define PLUSE_USING_POQ_HARDTIMER 1u

/* ȫ�ֱ������� */
static vkPLUSE gPluse[2] = {0};
static uint8_t gMutex = 0;

void vkPluse_Timer_Callback(void * pdata);

/*******************************************************************************
 * ����: vkPluse_Init
 * ����: �����ʼ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: Ĭ����������200us��Ƶ��33Hz������ʱ��0ms���½�ʱ��0ms 
 ******************************************************************************/
int vkPluse_Init(void)
{
	/* ����0 */
	gPluse[0].p_mode = 0;
	gPluse[0].p_expect = 0;
	gPluse[0].p_current = 0;
	gPluse[0].p_ration = 0;

	gPluse[0].p_rise = 0;
	gPluse[0].p_fall = 0;
	
	gPluse[0].p_duration = 0;
	gPluse[0].p_interval = 0;	
	gPluse[0].p_poqcount = 0xFFFF;
	gPluse[0].p_1mscount = 0;
	gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse[0].p_poqflags = 0;

	/* ��ʱ�� */
	gPluse[0].p_poqtimer.timer_name = &gPluse[0].p_poqtimer;
	gPluse[0].p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse[0].p_poqtimer.cb_data  	= &gPluse[0];
	gPluse[0].p_poqtimer.cb_ticks 	= 0;

	gPluse[0].p_freqy  = 33;	//Ĭ��33Hz
	gPluse[0].p_width = 200;	//Ĭ��200us
	gPluse[0].p_tickfreqy = 0;
	gPluse[0].p_tickwidth = 0;
	gPluse[0].p_bodcount = 0;
	memset(gPluse[0].p_bodvalue,0,PLUSE_COUNT_WIDTH_MAX-1);
	gPluse[0].p_bodlevel = PLUSE_LEVEL_LOW;

	/* ���� */
	gPluse[0].p_s0_gpio = PLUSE0_S0_GPIO;
	gPluse[0].p_s0_pin  = PLUSE0_S0_PIN;	
	gPluse[0].p_s1_gpio = PLUSE0_S1_GPIO;
	gPluse[0].p_s1_pin  = PLUSE0_S1_PIN;
	gPluse[0].p_s2_gpio = PLUSE0_S2_GPIO;
	gPluse[0].p_s2_pin  = PLUSE0_S2_PIN;
	gPluse[0].p_s3_gpio = PLUSE0_S3_GPIO;
	gPluse[0].p_s3_pin  = PLUSE0_S3_PIN;
	GPIO_Init(gPluse[0].p_s0_gpio,gPluse[0].p_s0_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin,GPIO_Mode_Out_PP_Low_Slow);

	/* ����ADC */
	gPluse[0].p_adcchnl  = ADC_Channel_16;
	gPluse[0].p_adc_gpio = PLUSE0_ADC_GPIO;
	gPluse[0].p_adc_pin  = PLUSE0_ADC_PIN;
	GPIO_Init(gPluse[0].p_adc_gpio,gPluse[0].p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse[0].pwm = PWM1;
	vkPWM_Init(gPluse[0].pwm);	

	/* ����1 */
	gPluse[1].p_mode = 0;
	gPluse[1].p_expect = 0;
	gPluse[1].p_current = 0;
	gPluse[1].p_ration = 0;

	gPluse[1].p_rise = 0;
	gPluse[1].p_fall = 0;	
	
	gPluse[1].p_duration = 0;
	gPluse[1].p_interval = 0;
	gPluse[1].p_poqcount = 0xFFFF;
	gPluse[1].p_1mscount = 0;	
	gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse[1].p_poqflags = 0;

	/* ��ʱ�� */
	gPluse[1].p_poqtimer.timer_name = &gPluse[1].p_poqtimer;
	gPluse[1].p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse[1].p_poqtimer.cb_data  	= &gPluse[1];
	gPluse[1].p_poqtimer.cb_ticks 	= 0;

	gPluse[1].p_freqy = 33;	//Ĭ��33Hz
	gPluse[1].p_width = 200;	//Ĭ��200us
	gPluse[1].p_tickfreqy = 0;
	gPluse[1].p_tickwidth = 0;	
	gPluse[1].p_bodcount = 0;
	memset(gPluse[1].p_bodvalue,0,PLUSE_COUNT_WIDTH_MAX-1);
	gPluse[1].p_bodlevel = PLUSE_LEVEL_LOW;

	/* ���� */
	gPluse[1].p_s0_gpio = PLUSE1_S0_GPIO;
	gPluse[1].p_s0_pin  = PLUSE1_S0_PIN;	
	gPluse[1].p_s1_gpio = PLUSE1_S1_GPIO;
	gPluse[1].p_s1_pin  = PLUSE1_S1_PIN;
	gPluse[1].p_s2_gpio = PLUSE1_S2_GPIO;
	gPluse[1].p_s2_pin  = PLUSE1_S2_PIN;
	gPluse[1].p_s3_gpio = PLUSE1_S3_GPIO;
	gPluse[1].p_s3_pin  = PLUSE1_S3_PIN;
	GPIO_Init(gPluse[1].p_s0_gpio,gPluse[1].p_s0_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin,GPIO_Mode_Out_PP_Low_Slow);

	/* ����ADC */
	gPluse[1].p_adcchnl  = ADC_Channel_23;
	gPluse[1].p_adc_gpio = PLUSE1_ADC_GPIO;
	gPluse[1].p_adc_pin  = PLUSE1_ADC_PIN;
	GPIO_Init(gPluse[1].p_adc_gpio,gPluse[1].p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse[1].pwm = PWM2;
	vkPWM_Init(gPluse[1].pwm);

	return 0;
}

/*******************************************************************************
 * ����: vkPluse_Deinit
 * ����: ��������
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 
 ******************************************************************************/
int vkPluse_Deinit(void)
{
	vkTimerCancel(&gPluse[0].p_poqtimer);
	vkPWM_Deinit(gPluse[0].pwm);
	
	vkTimerCancel(&gPluse[1].p_poqtimer);
	vkPWM_Deinit(gPluse[1].pwm);
	
	return 0;
}

/*******************************************************************************
 * ����: vkPluseSetMode
 * ����: ��������ģʽ������ģʽ/����ģʽ
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ���óɹ���������Ч
 ******************************************************************************/
int vkPluseSetMode(uint8_t pluse, uint8_t mode)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	gPluse[index].p_mode = mode;
	
	return 0;
}

/*******************************************************************************
 * ����: vkPluseSetExpt
 * ����: �����������ǿ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ���óɹ���������Ч
 ******************************************************************************/
int vkPluseSetExpt(uint8_t pluse, uint8_t expert)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	gPluse[index].p_expect = expert;

	return 0;
}

/*******************************************************************************
 * ����: vkPluseSetTime
 * ����: �����������ʱ��ͼ��ʱ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ���óɹ���������Ч��ʱ�䵥λ��ms
 ******************************************************************************/
int vkPluseSetTime(uint8_t pluse, uint16_t duration, uint16_t interval)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	gPluse[index].p_duration = duration;
	gPluse[index].p_interval = interval;

	return 0;
}

/*******************************************************************************
 * ����: vkPluseSetRise
 * ����: ����һ������̼�����ʱ��
 * �β�: ����pluse������ʱ��rise(0/100/200/300/400/500)
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��һ������̼�������ʱ����Ч������/�½�ʱ�䲻�����������ʱ���1/3
 ******************************************************************************/
int vkPluseSetRise(uint8_t pluse, uint16_t rise)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	
	if(gPluse[index].p_duration < 300)
	{
		/* �������ʱ��С��300us,��ʱ����ʱ����½�ʱ�������0 */
		gPluse[index].p_rise = 0;
		
		/* ���rise��0���򷵻�����ʧ�� */
		if(rise!= 0)
		{
			return -1;
		}
	}
	else if(gPluse[index].p_duration >= 300 && gPluse[index].p_duration < 600)
	{
		/* �������ʱ����300~600us,��ʱ����ʱ����½�ʱ����費����100us */
		if(rise <= 100)
		{
			gPluse[index].p_rise = rise;
		}
		else
		{
			return -1;
		}
	}
	else if(gPluse[index].p_duration >= 600 && gPluse[index].p_duration < 900)
	{
		/* �������ʱ����600~900us,��ʱ����ʱ����½�ʱ����費����200us */
		if(rise <= 200)
		{
			gPluse[index].p_rise = rise;
		}
		else
		{
			return -1;
		}	
	}
	else if(gPluse[index].p_duration >= 900 && gPluse[index].p_duration < 1200)
	{
		/* �������ʱ����900~1200us,��ʱ����ʱ����½�ʱ����費����300us */
		if(rise <= 300)
		{
			gPluse[index].p_rise = rise;
		}
		else
		{
			return -1;
		}	
	}
	else if(gPluse[index].p_duration >= 1200 && gPluse[index].p_duration < 1500)
	{
		/* �������ʱ����1200~1500us,��ʱ����ʱ����½�ʱ����費����400us */
		if(rise <= 400)
		{
			gPluse[index].p_rise = rise;
		}
		else
		{
			return -1;
		}	
	}
	else if(gPluse[index].p_duration >= 1500)
	{
		/* �������ʱ�䳬��1500us,��ʱ����ʱ����½�ʱ����費����500us */
		gPluse[index].p_rise = rise;	
	}

	return 0;
}

/*******************************************************************************
 * ����: vkPluseSetFall
 * ����: ����һ������̼��½�ʱ��
 * �β�: ����pluse������ʱ��fall
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��һ������̼�������ʱ����Ч
 ******************************************************************************/
int vkPluseSetFall(uint8_t pluse, uint16_t fall)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	
	if(gPluse[index].p_duration < 300)
	{
		/* �������ʱ��С��300us,��ʱ����ʱ����½�ʱ�������0 */
		gPluse[index].p_fall = 0;
		
		/* ���rise��0���򷵻�����ʧ�� */
		if(fall!= 0)
		{
			return -1;
		}
	}
	else if(gPluse[index].p_duration >= 300 && gPluse[index].p_duration < 600)
	{
		/* �������ʱ����300~600us,��ʱ����ʱ����½�ʱ����費����100us */
		if(fall <= 100)
		{
			gPluse[index].p_fall = fall;
		}
		else
		{
			return -1;
		}
	}
	else if(gPluse[index].p_duration >= 600 && gPluse[index].p_duration < 900)
	{
		/* �������ʱ����600~900us,��ʱ����ʱ����½�ʱ����費����200us */
		if(fall <= 200)
		{
			gPluse[index].p_fall = fall;
		}
		else
		{
			return -1;
		}	
	}
	else if(gPluse[index].p_duration >= 900 && gPluse[index].p_duration < 1200)
	{
		/* �������ʱ����900~1200us,��ʱ����ʱ����½�ʱ����費����300us */
		if(fall <= 300)
		{
			gPluse[index].p_fall = fall;
		}
		else
		{
			return -1;
		}	
	}
	else if(gPluse[index].p_duration >= 1200 && gPluse[index].p_duration < 1500)
	{
		/* �������ʱ����1200~1500us,��ʱ����ʱ����½�ʱ����費����400us */
		if(fall <= 400)
		{
			gPluse[index].p_fall = fall;
		}
		else
		{
			return -1;
		}	
	}
	else if(gPluse[index].p_duration >= 1500)
	{
		/* �������ʱ�䳬��1500us,��ʱ����ʱ����½�ʱ����費����500us */
		gPluse[index].p_fall = fall;	
	}

	return 0;
}


/*******************************************************************************
 * ����: vkPluseSetWidth
 * ����: ����������
 * �β�: ����pluse�����widthʱ��
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��һ������̼����������ú�Ƶ��������Ч 
 ******************************************************************************/
int vkPluseSetWidth(uint8_t pluse, uint16_t width)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	gPluse[index].p_width = width;

	return 0;
}

/*******************************************************************************
 * ����: vkPluseSetFrequency
 * ����: ��������Ƶ��
 * �β�: ����pluse��Ƶ��frequency
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��һ������̼����������ú�Ƶ��������Ч 
 ******************************************************************************/
int vkPluseSetFrequency(uint8_t pluse, uint16_t frequency)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	gPluse[index].p_freqy = frequency;

	return 0;
}

/*******************************************************************************
 * ����: vkPluseGetExpt
 * ����: ��ȡ����ǿ��
 * �β�: ����pluse
 * ����: ����ǿ��ֵ
 * ˵��: 
 ******************************************************************************/
uint8_t vkPluseGetExpt(uint8_t pluse)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return 0;
	}

	uint8_t index = pluse;

	return gPluse[index].p_expect;	
}

/*******************************************************************************
 * ����: vkPluseGetPoqL
 * ����: ��ȡ�����ϲ�״̬
 * �β�: ����pluse
 * ����: �����ϲ�״̬
 * ˵��: 
 ******************************************************************************/
uint8_t vkPluseGetPoqL(uint8_t pluse)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return 0;
	}

	uint8_t index = pluse;

	return gPluse[index].p_poqlevel;		
}

/*******************************************************************************
 * ����: vkPluseStart
 * ����: �����������
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��
 ******************************************************************************/
inline int vkPluseStart(uint8_t pluse)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}
	
	uint8_t index = pluse;

	/* ���������ʱ��ͼ��ʱ���Ƿ���0 */
	if(gPluse[index].p_mode == PLUSE_MODE_CONTINUE)
	{
		if(gPluse[index].p_duration==0 || gPluse[index].p_interval==0)
			return -1;
	}
	else if(gPluse[index].p_mode == PLUSE_MODE_ONLYONCE)
	{
		if(gPluse[index].p_duration==0)
			return -1;
	}

	#if	PLUSE_USING_POQ_SOFTTIMER == 1u	
	/* ������ʱ�� */
	vkTimerCancel(&gPluse[index].p_poqtimer);	
	gPluse[index].p_poqtimer.cb_ticks = vkMS_TO_TICKS(gPluse[index].p_duration);
	vkTimerInsert(&gPluse[index].p_poqtimer);

	#elif PLUSE_USING_POQ_HARDTIMER == 1u	
	/* �ϲ�������� */
	vkPluse1MSSet(pluse, 0);
	gPluse[index].p_poqcount = 0;
	
	#endif

	/* �ϲ����øߵ�ƽ */
	gPluse[index].p_poqlevel = PLUSE_LEVEL_HIGH;

	return 0;
}

/*******************************************************************************
 * ����: vkPluseStop
 * ����: ֹͣ�������
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: ��
 ******************************************************************************/
inline int vkPluseStop(uint8_t pluse)
{	
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;

	#if	PLUSE_USING_POQ_SOFTTIMER == 1u
	vkTimerCancel(&gPluse[index].p_poqtimer);
	
	#elif PLUSE_USING_POQ_HARDTIMER == 1u
	/* �ϲ�������� */
	gPluse[index].p_poqcount = 0xFFFF;
	vkPluse1MSSet(pluse, 0);
	
	#endif

	gPluse[index].p_current = 0;	
	gPluse[index].p_ration  = 0;

	gPluse[index].p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse[index].p_poqflags = 0;
	
	gPluse[index].p_bodlevel = PLUSE_LEVEL_LOW;
	gPluse[index].p_bodcount = 0;
	
	vkPWMStop(gPluse[index].pwm);
		
	GPIO_ResetBits(gPluse[index].p_s1_gpio,gPluse[index].p_s1_pin); //�Ͽ�
	GPIO_ResetBits(gPluse[index].p_s2_gpio,gPluse[index].p_s2_pin); //�Ͽ�
	GPIO_ResetBits(gPluse[index].p_s3_gpio,gPluse[index].p_s3_pin); //�Ͽ�

	return 0;
}

/*******************************************************************************
 * ����: vkPluse_Timer_Callback
 * ����: ���嶨ʱ�ص�����
 * �β�: 
 * ����: ��
 * ˵��: ��
 ******************************************************************************/
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
			pluse->p_poqtimer.cb_ticks 	= vkMS_TO_TICKS(pluse->p_duration);
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_HIGH;
		}
		else if(pluse->p_poqlevel == PLUSE_LEVEL_HIGH)
		{
			pluse->p_poqtimer.cb_ticks 	= vkMS_TO_TICKS(pluse->p_interval);
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_LOW;			
		}
	}
	/* ����ģʽ */
	else if(pluse->p_mode == PLUSE_MODE_ONLYONCE)
	{
		if(pluse->p_poqlevel == PLUSE_LEVEL_LOW)
		{
			pluse->p_poqtimer.cb_ticks 	= vkMS_TO_TICKS(pluse->p_duration);
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_HIGH;
		}
		else if(pluse->p_poqlevel == PLUSE_LEVEL_HIGH)
		{			
			pluse->p_poqlevel = PLUSE_LEVEL_LOW;			
		}		
	}
}

/*******************************************************************************
 * ����: vkPluseRation
 * ����: ����ռ�ձ�
 * �β�: 
 * ����: ��
 * ˵��: ��
 ******************************************************************************/
inline uint8_t vkPluseRation(uint8_t pluse, uint8_t ration)
{	
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;

	gPluse[index].p_ration = ration;	
	vkPWMStart(gPluse[index].pwm, PLUSE_PWM_PERIOD, ration);
	
	return 0;
}

/*******************************************************************************
 * ����: vkPluseAdjust
 * ����: �����趨����ǿ��
 * �β�: 
 * ����: ��
 * ˵��: ��
 ******************************************************************************/
inline uint8_t vkPluseAdjust(uint8_t pluse, uint8_t expect, uint8_t ration, uint8_t current)
{	
	uint8_t ret_ration = ration;
	uint8_t abs_val = 0;
	uint8_t step = 0;
	vkPWM pwm;

	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;

	pwm = gPluse[index].pwm;	
		
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
		abs_val = abs(current-expect);
		
		if(abs_val>PLUSE_PWM_TOLERANCE)
		{
			step = (current == 0)?1:(uint8_t)(abs_val*0.5*0.7 + 0.5 + 1);
					
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

/*******************************************************************************
 * ����: vkPluseGetIdle
 * ����: �������嶼���ڵ͵�ƽ״̬
 * �β�: 
 * ����: ��
 * ˵��: ��
 ******************************************************************************/
int vkPluseGetIdle(void)
{
	if(gPluse[0].p_bodlevel != PLUSE_LEVEL_HIGH && gPluse[1].p_bodlevel != PLUSE_LEVEL_HIGH)
	{
		return 1;
	}

	return 0;
}

int vkPluse1MSAdd(uint8_t pluse, uint8_t value)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return 0;
	}
	
	uint8_t index = pluse;

	if(gPluse[index].p_1mscount >= 	PLUSE_TICKS_PER_MS)
	{
		gPluse[index].p_1mscount = 0;

		return 1;
	}
	else
	{
		gPluse[index].p_1mscount += value;
	}

    return 0;
}

int vkPluse1MSSet(uint8_t pluse, uint8_t value)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}

	uint8_t index = pluse;
	gPluse[index].p_1mscount = value;
	
	return 0;
}

/*******************************************************************************
 * ����: vkPluseTrimExpt
 * ����: ��������ʱ����½�ʱ���������ǿ��
 * �β�: 
 * ����: ��
 * ˵��: ��
 ******************************************************************************/
inline uint8_t vkPluseTrimExpt(uint8_t expect ,uint16_t duration, uint16_t rise, uint16_t fall, uint16_t poqcount)
{
	if(expect == 0 || duration == 0 || rise > duration || fall > duration || poqcount > duration || (rise+fall) > duration)
	{
		return 0;
	}

	if(poqcount <= rise)
	{
		if(rise == 0)
		{
			return expect;
		}
		else
		{
			return (uint8_t)(1.0*poqcount/rise)*expect;
		}
	}
	else if(poqcount > rise && poqcount < duration-fall)
	{
		return expect;
	}
	else if(poqcount >= duration-fall && poqcount <= duration)
	{
		if(fall == 0)
		{
			return expect;
		}
		else
		{
			return (uint8_t)(1.0*(duration-poqcount)/fall)*expect;
		}
	}
	
	return 0;
}

void vkPluseTick(void)
{
	//�缫A���ʱ��
	if(gPluse[0].p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse[0].p_poqflags == 0)
		{
			gPluse[0].p_poqflags = 1;
			
			vkPWMStop(gPluse[0].pwm);

			//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
			GPIO_ResetBits(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin);	//�Ͽ�
			//�ڶ�·�������õ͵�ƽ������3.3V
			GPIO_ResetBits(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin);	//�Ͽ�
	
			GPIO_ResetBits(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin);	//�Ͽ�

			/* ���õ͵�ƽ */
			gPluse[0].p_bodlevel = PLUSE_LEVEL_LOW;

			/* ��������ʱ��Tick������������������Tick���� */
			/* һ������̼�����һ������̼������Ƶ��������Ч */
			gPluse[0].p_tickwidth = gPluse[0].p_width/PLUSE_TICK_TIME;
			gPluse[0].p_tickfreqy = ((1000ul*1000ul)/gPluse[0].p_freqy)/PLUSE_TICK_TIME;			
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[0].p_poqcount != 0xFFFF)
		{
			if(gPluse[0].p_poqcount >= gPluse[0].p_interval)
			{
				/* ���ʱ�䵽���ϲ�������� */
				vkPluse1MSSet(PLUSE0, 0);
				gPluse[0].p_poqcount = 0;
				
				/* ����ģʽ */
				if(gPluse[0].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[0].p_poqlevel = PLUSE_LEVEL_HIGH;			
				}
				/* ����ģʽ */
				else if(gPluse[0].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* ÿ������1 */
				gPluse[0].p_poqcount += vkPluse1MSAdd(PLUSE0, 1);
			}
		}
		#endif		
	}

	//�缫B���ʱ��
	if(gPluse[1].p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse[1].p_poqflags == 0)
		{
			gPluse[1].p_poqflags = 1;

			vkPWMStop(gPluse[1].pwm);

			//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
			GPIO_ResetBits(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin);	//�Ͽ�
			//�ڶ�·�������õ͵�ƽ������3.3V
			GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//�Ͽ�
	
			GPIO_ResetBits(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin);	//�Ͽ�

			gPluse[1].p_bodlevel = PLUSE_LEVEL_LOW;

			/* ��������ʱ��Tick������������������Tick���� */
			/* һ������̼�����һ������̼������Ƶ��������Ч */
			gPluse[1].p_tickwidth = gPluse[1].p_width/PLUSE_TICK_TIME;
			gPluse[1].p_tickfreqy = ((1000ul*1000ul)/gPluse[1].p_freqy)/PLUSE_TICK_TIME;	
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[1].p_poqcount != 0xFFFF)
		{
			if(gPluse[1].p_poqcount >= gPluse[1].p_interval)
			{
				/* ���ʱ�䵽���ϲ�������� */
				vkPluse1MSSet(PLUSE1, 0);
				gPluse[1].p_poqcount = 0;
				
				/* ����ģʽ */
				if(gPluse[1].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[1].p_poqlevel = PLUSE_LEVEL_HIGH;			
				}
				/* ����ģʽ */
				else if(gPluse[1].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* ÿ������1 */
				gPluse[1].p_poqcount += vkPluse1MSAdd(PLUSE1, 1);
			}
			#endif	
		}
	}

	//�缫A����ʱ��
	if(gPluse[0].p_poqlevel == PLUSE_LEVEL_HIGH)
	{	
		if(gPluse[0].p_bodlevel == PLUSE_LEVEL_LOW)
		{	
			gPluse[0].p_bodcount++;
					
			if(gPluse[0].p_bodcount==1)	//��һ���������͵�ƽ����ʼ���	
			{
				//�ڶ�·�������øߵ�ƽ���ӵأ�Ѹ�����͵缫��ѹOV
				GPIO_SetBits(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin);

				GPIO_ResetBits(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin);	//�Ͽ�
				
				//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
				GPIO_ResetBits(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin);	//�Ͽ�	
			}			
			else if(gPluse[0].p_bodcount == gPluse[0].p_tickwidth) //��ʱ PLUSE_MUTEX_DELAY �ͷŻ�����
			{
				//�ͷŻ�����
				gMutex = 0;	

				//�ڶ�·�������õ͵�ƽ��
				GPIO_ResetBits(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin);			//�Ͽ�				
			}			
			else if(gPluse[0].p_bodcount == (gPluse[0].p_tickwidth+gPluse[0].p_tickwidth+1)) 	//�������ʱ�䵽
			{		
				//�������������ռ�ձ�
				//�������,�˴������Ż�
				
				uint16_t adc_V = vkADCRead(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;
				for(uint8_t i=0; i<(gPluse[0].p_tickwidth-1); i++)
				{
					adc_T += gPluse[0].p_bodvalue[i];
				}
				adc_T = adc_T/(gPluse[0].p_tickwidth-1);
				gPluse[0].p_current = (uint16_t)((((1000*PLUSE_VREFINT)/adc_V)*adc_T)/PLUSE0_RESISTANCE);
				//��������ǿ�Ȳ�����PWM����ʼ��磬���۵���				
				//vkPluseRation(PLUSE0, 40);	//�̶�ռ�ձ�
				
				//uint16_t expect = vkPluseTrimExpt(gPluse[0].p_expect, gPluse[0].p_duration, gPluse[0].p_rise, gPluse[0].p_fall, gPluse[0].p_poqcount);
				gPluse[0].p_ration = vkPluseAdjust(PLUSE0, gPluse[0].p_expect, gPluse[0].p_ration, gPluse[0].p_current);	
			}			
			else if(gPluse[0].p_bodcount >= (gPluse[0].p_tickfreqy-gPluse[0].p_tickwidth)) //����ʱ�䵽
			{
				if(gMutex == 0)
				{
					//���ü������ı��ƽ״̬
					gPluse[0].p_bodcount = 0;
					gPluse[0].p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse[0].p_bodlevel == PLUSE_LEVEL_HIGH)	//200us
		{
			//ռ�û�����
			gMutex = 1;							
			gPluse[0].p_bodcount++;
			
			//��һ���������ߵ�ƽ����ʼ�ŵ�
			if(gPluse[0].p_bodcount == 1)
			{
				//�ر�PWM���Ѿ������˵�������ʼ�ŵ�							
				vkPWMStop(gPluse[0].pwm);

				GPIO_SetBits(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin);	//����		
				
				//�ڶ�·�������õ͵�ƽ�����ӵ�
				//�ڶ�·�������õ͵�ƽ������3.3V
				GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//�Ͽ�
				
				//��һ·�������øߵ�ƽ���缫�ŵ�0.1ms
				GPIO_SetBits(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin); //����
			}			
			else if(gPluse[0].p_bodcount == gPluse[0].p_tickwidth)	//����ʱ�䵽width_count0*50us = 200us
			{
				/* ���һ�βɼ� */
				gPluse[0].p_bodvalue[gPluse[0].p_bodcount-2] = vkADCRead(gPluse[0].p_adcchnl);
				
				//���ü������ı��ƽ״̬
				gPluse[0].p_bodcount = 0;				
				gPluse[0].p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse[0].p_poqflags = 0;
			}
			//��һ�β���ADC
			else
			{
				//�ɼ�ADC
				gPluse[0].p_bodvalue[gPluse[0].p_bodcount-2] = vkADCRead(gPluse[0].p_adcchnl);
			}
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[0].p_poqcount != 0xFFFF)
		{
			if(gPluse[0].p_poqcount >= gPluse[0].p_duration)
			{
				/* ����ʱ�䵽���ϲ�������� */
				vkPluse1MSSet(PLUSE0, 0);
				gPluse[0].p_poqcount = 0;
				
				/* ����ģʽ */
				if(gPluse[0].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;			
				}
				/* ����ģʽ */
				else if(gPluse[0].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* ÿ������1 */
				gPluse[0].p_poqcount += vkPluse1MSAdd(PLUSE0, 1);
			}
		}
		#endif
	}

	//�缫B����ʱ��
	if(gPluse[1].p_poqlevel == PLUSE_LEVEL_HIGH)	
	{
		if(gPluse[1].p_bodlevel == PLUSE_LEVEL_LOW)	//30ms
		{	
			gPluse[1].p_bodcount++;			
			
			if(gPluse[1].p_bodcount == 1)	//��1���������͵�ƽ����ʼ���			
			{
				//�ڶ�·�������øߵ�ƽ���ӵأ�Ѹ�����͵缫��ѹOV
				GPIO_SetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);		//����

				GPIO_ResetBits(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin);	//�Ͽ�
				
				//��һ·�������õ͵�ƽ���缫ֹͣ�ŵ�
				GPIO_ResetBits(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin);	//�Ͽ�	
			}			
			else if(gPluse[1].p_bodcount == gPluse[1].p_tickwidth)	//��ʱ PLUSE_MUTEX_DELAY �ͷŻ�����
			{
				//�ͷŻ�����
				gMutex = 0;	

				//�ڶ�·�������õ͵�ƽ��
				GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//�Ͽ�				
			}			
			else if(gPluse[1].p_bodcount == (gPluse[1].p_tickwidth+gPluse[1].p_tickwidth+1)) //�������ʱ�䵽
			{
				//�������������ռ�ձ�
				//�������,�˴������Ż�
				
				uint16_t adc_V = vkADCRead(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;		
				for(uint8_t i=0; i<(gPluse[1].p_tickwidth-1); i++)
				{
					adc_T += gPluse[1].p_bodvalue[i];
				}
				adc_T = adc_T/(gPluse[1].p_tickwidth-1);				
				gPluse[1].p_current = (uint16_t)((((1000*PLUSE_VREFINT)/adc_V)*adc_T)/PLUSE0_RESISTANCE);
				//��������ǿ�Ȳ�����PWM����ʼ��磬���۵���				
				//vkPluseRation(PLUSE1, 1);		//�̶�ռ�ձ�

				//uint16_t expect = vkPluseTrimExpt(gPluse[1].p_expect, gPluse[1].p_duration, gPluse[1].p_rise, gPluse[1].p_fall, gPluse[1].p_poqcount);
				gPluse[1].p_ration = vkPluseAdjust(PLUSE1, gPluse[1].p_expect, gPluse[1].p_ration, gPluse[1].p_current);	
			}			
			else if(gPluse[1].p_bodcount >= (gPluse[1].p_tickfreqy-gPluse[1].p_tickwidth)) //����ʱ�䵽
			{
				if(gMutex == 0)
				{
					//���ü������ı��ƽ״̬
					gPluse[1].p_bodcount = 0;
					gPluse[1].p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse[1].p_bodlevel == PLUSE_LEVEL_HIGH) //200us
		{
			//ռ�û�����
			gMutex = 1;
			gPluse[1].p_bodcount++;
			
			//��һ���������ߵ�ƽ����ʼ�ŵ�
			if(gPluse[1].p_bodcount == 1)
			{
				//�ر�PWM���Ѿ������˵�������ʼ�ŵ�							
				vkPWMStop(gPluse[1].pwm);

				GPIO_SetBits(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin); 	//����		
				
				//�ڶ�·�������õ͵�ƽ�����ӵ�
				//�ڶ�·�������õ͵�ƽ������3.3V
				GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//�Ͽ�
				
				//��һ·�������øߵ�ƽ���缫�ŵ�0.1ms
				GPIO_SetBits(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin); 	//����
			}			
			else if(gPluse[1].p_bodcount == gPluse[1].p_tickwidth)		//����ʱ�䵽width_count1*50us = 200us
			{
				/* ���һ�βɼ� */
				gPluse[1].p_bodvalue[gPluse[1].p_bodcount-2] = vkADCRead(gPluse[1].p_adcchnl);
								
				//���ü������ı��ƽ״̬
				gPluse[1].p_bodcount = 0; 			
				gPluse[1].p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse[1].p_poqflags = 0;
			}
			else
			{
				//�ɼ�ADC
				gPluse[1].p_bodvalue[gPluse[1].p_bodcount-2] = vkADCRead(gPluse[1].p_adcchnl);
			}
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[1].p_poqcount != 0xFFFF)
		{
			if(gPluse[1].p_poqcount >= gPluse[1].p_duration)
			{
				/* ����ʱ�䵽���ϲ�������� */
				vkPluse1MSSet(PLUSE1, 0);
				gPluse[1].p_poqcount = 0;
				
				/* ����ģʽ */
				if(gPluse[1].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW; 		
				}
				/* ����ģʽ */
				else if(gPluse[1].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* ÿ������1 */
				gPluse[1].p_poqcount += vkPluse1MSAdd(PLUSE1, 1);
			}
		}
		#endif
	}
}

