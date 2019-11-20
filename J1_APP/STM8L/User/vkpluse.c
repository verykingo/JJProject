/******************************************************************************
 * 文件  ：vkpluse.c
 * 描述    ：电极脉冲
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include "stdlib.h"
#include <math.h>
#include <string.h>

/* 包含自定义头文件 */
#include "vkpluse.h"
#include "vktimetick.h"

/* 自定义新类型 */

/* 自定义宏 */
#define PLUSE_USING_POQ_SOFTTIMER 0u
#define PLUSE_USING_POQ_HARDTIMER 1u

/* 全局变量定义 */
static vkPLUSE gPluse[2] = {0};
static uint8_t gMutex = 0;

void vkPluse_Timer_Callback(void * pdata);

/*******************************************************************************
 * 名称: vkPluse_Init
 * 功能: 脉冲初始化
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 默认设置脉宽200us，频率33Hz，上升时间0ms，下降时间0ms 
 ******************************************************************************/
int vkPluse_Init(void)
{
	/* 脉冲0 */
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

	/* 定时器 */
	gPluse[0].p_poqtimer.timer_name = &gPluse[0].p_poqtimer;
	gPluse[0].p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse[0].p_poqtimer.cb_data  	= &gPluse[0];
	gPluse[0].p_poqtimer.cb_ticks 	= 0;

	gPluse[0].p_freqy  = 33;	//默认33Hz
	gPluse[0].p_width = 200;	//默认200us
	gPluse[0].p_tickfreqy = 0;
	gPluse[0].p_tickwidth = 0;
	gPluse[0].p_bodcount = 0;
	memset(gPluse[0].p_bodvalue,0,PLUSE_COUNT_WIDTH_MAX-1);
	gPluse[0].p_bodlevel = PLUSE_LEVEL_LOW;

	/* 开关 */
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

	/* 电流ADC */
	gPluse[0].p_adcchnl  = ADC_Channel_16;
	gPluse[0].p_adc_gpio = PLUSE0_ADC_GPIO;
	gPluse[0].p_adc_pin  = PLUSE0_ADC_PIN;
	GPIO_Init(gPluse[0].p_adc_gpio,gPluse[0].p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse[0].pwm = PWM1;
	vkPWM_Init(gPluse[0].pwm);	

	/* 脉冲1 */
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

	/* 定时器 */
	gPluse[1].p_poqtimer.timer_name = &gPluse[1].p_poqtimer;
	gPluse[1].p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse[1].p_poqtimer.cb_data  	= &gPluse[1];
	gPluse[1].p_poqtimer.cb_ticks 	= 0;

	gPluse[1].p_freqy = 33;	//默认33Hz
	gPluse[1].p_width = 200;	//默认200us
	gPluse[1].p_tickfreqy = 0;
	gPluse[1].p_tickwidth = 0;	
	gPluse[1].p_bodcount = 0;
	memset(gPluse[1].p_bodvalue,0,PLUSE_COUNT_WIDTH_MAX-1);
	gPluse[1].p_bodlevel = PLUSE_LEVEL_LOW;

	/* 开关 */
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

	/* 电流ADC */
	gPluse[1].p_adcchnl  = ADC_Channel_23;
	gPluse[1].p_adc_gpio = PLUSE1_ADC_GPIO;
	gPluse[1].p_adc_pin  = PLUSE1_ADC_PIN;
	GPIO_Init(gPluse[1].p_adc_gpio,gPluse[1].p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse[1].pwm = PWM2;
	vkPWM_Init(gPluse[1].pwm);

	return 0;
}

/*******************************************************************************
 * 名称: vkPluse_Deinit
 * 功能: 脉冲销毁
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 
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
 * 名称: vkPluseSetMode
 * 功能: 设置脉冲模式：连续模式/单个模式
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 设置成功后，立即生效
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
 * 名称: vkPluseSetExpt
 * 功能: 设置脉冲电流强度
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 设置成功后，立即生效
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
 * 名称: vkPluseSetTime
 * 功能: 设置脉冲持续时间和间隔时间
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 设置成功后，立即生效，时间单位是ms
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
 * 名称: vkPluseSetRise
 * 功能: 设置一次脉冲刺激上升时间
 * 形参: 脉冲pluse，上升时间rise(0/100/200/300/400/500)
 * 返回: 成功0，失败-1
 * 说明: 下一次脉冲刺激，上升时间生效，上升/下降时间不超过脉冲持续时间的1/3
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
		/* 脉冲持续时间小于300us,此时上升时间和下降时间必需是0 */
		gPluse[index].p_rise = 0;
		
		/* 如果rise非0，则返回设置失败 */
		if(rise!= 0)
		{
			return -1;
		}
	}
	else if(gPluse[index].p_duration >= 300 && gPluse[index].p_duration < 600)
	{
		/* 脉冲持续时间是300~600us,此时上升时间和下降时间必需不超过100us */
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
		/* 脉冲持续时间是600~900us,此时上升时间和下降时间必需不超过200us */
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
		/* 脉冲持续时间是900~1200us,此时上升时间和下降时间必需不超过300us */
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
		/* 脉冲持续时间是1200~1500us,此时上升时间和下降时间必需不超过400us */
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
		/* 脉冲持续时间超过1500us,此时上升时间和下降时间必需不超过500us */
		gPluse[index].p_rise = rise;	
	}

	return 0;
}

/*******************************************************************************
 * 名称: vkPluseSetFall
 * 功能: 设置一次脉冲刺激下降时间
 * 形参: 脉冲pluse，上升时间fall
 * 返回: 成功0，失败-1
 * 说明: 下一次脉冲刺激，上升时间生效
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
		/* 脉冲持续时间小于300us,此时上升时间和下降时间必需是0 */
		gPluse[index].p_fall = 0;
		
		/* 如果rise非0，则返回设置失败 */
		if(fall!= 0)
		{
			return -1;
		}
	}
	else if(gPluse[index].p_duration >= 300 && gPluse[index].p_duration < 600)
	{
		/* 脉冲持续时间是300~600us,此时上升时间和下降时间必需不超过100us */
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
		/* 脉冲持续时间是600~900us,此时上升时间和下降时间必需不超过200us */
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
		/* 脉冲持续时间是900~1200us,此时上升时间和下降时间必需不超过300us */
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
		/* 脉冲持续时间是1200~1500us,此时上升时间和下降时间必需不超过400us */
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
		/* 脉冲持续时间超过1500us,此时上升时间和下降时间必需不超过500us */
		gPluse[index].p_fall = fall;	
	}

	return 0;
}


/*******************************************************************************
 * 名称: vkPluseSetWidth
 * 功能: 设置脉冲宽度
 * 形参: 脉冲pluse，宽度width时间
 * 返回: 成功0，失败-1
 * 说明: 下一次脉冲刺激，脉宽设置和频率设置生效 
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
 * 名称: vkPluseSetFrequency
 * 功能: 设置脉冲频率
 * 形参: 脉冲pluse，频率frequency
 * 返回: 成功0，失败-1
 * 说明: 下一次脉冲刺激，脉宽设置和频率设置生效 
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
 * 名称: vkPluseGetExpt
 * 功能: 获取脉冲强度
 * 形参: 脉冲pluse
 * 返回: 脉冲强度值
 * 说明: 
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
 * 名称: vkPluseGetPoqL
 * 功能: 获取脉冲上层状态
 * 形参: 脉冲pluse
 * 返回: 脉冲上层状态
 * 说明: 
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
 * 名称: vkPluseStart
 * 功能: 启动脉冲输出
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无
 ******************************************************************************/
inline int vkPluseStart(uint8_t pluse)
{
	if(pluse != PLUSE0 && pluse != PLUSE1)
	{
		return -1;
	}
	
	uint8_t index = pluse;

	/* 必需检查持续时间和间隔时间是否是0 */
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
	/* 启动软定时器 */
	vkTimerCancel(&gPluse[index].p_poqtimer);	
	gPluse[index].p_poqtimer.cb_ticks = vkMS_TO_TICKS(gPluse[index].p_duration);
	vkTimerInsert(&gPluse[index].p_poqtimer);

	#elif PLUSE_USING_POQ_HARDTIMER == 1u	
	/* 上层计数清零 */
	vkPluse1MSSet(pluse, 0);
	gPluse[index].p_poqcount = 0;
	
	#endif

	/* 上层设置高电平 */
	gPluse[index].p_poqlevel = PLUSE_LEVEL_HIGH;

	return 0;
}

/*******************************************************************************
 * 名称: vkPluseStop
 * 功能: 停止脉冲输出
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: 无
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
	/* 上层计数清零 */
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
		
	GPIO_ResetBits(gPluse[index].p_s1_gpio,gPluse[index].p_s1_pin); //断开
	GPIO_ResetBits(gPluse[index].p_s2_gpio,gPluse[index].p_s2_pin); //断开
	GPIO_ResetBits(gPluse[index].p_s3_gpio,gPluse[index].p_s3_pin); //断开

	return 0;
}

/*******************************************************************************
 * 名称: vkPluse_Timer_Callback
 * 功能: 脉冲定时回调函数
 * 形参: 
 * 返回: 无
 * 说明: 无
 ******************************************************************************/
void vkPluse_Timer_Callback(void * pdata)
{
	vkPLUSE *pluse = (vkPLUSE *)pdata;
	if(pluse == NULL)
	{
		return;
	}
	/* 周期模式 */
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
	/* 单个模式 */
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
 * 名称: vkPluseRation
 * 功能: 设置占空比
 * 形参: 
 * 返回: 无
 * 说明: 无
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
 * 名称: vkPluseAdjust
 * 功能: 自稳设定脉冲强度
 * 形参: 
 * 返回: 无
 * 说明: 无
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
		
	/* 占空比不为0，说明电路有电压，电流为0，电路断开，说明电极脱落，此时不再调占空比升压*/
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
		/* 误差范围大于容忍范围 */
		abs_val = abs(current-expect);
		
		if(abs_val>PLUSE_PWM_TOLERANCE)
		{
			step = (current == 0)?1:(uint8_t)(abs_val*0.5*0.7 + 0.5 + 1);
					
			if(current > expect)	//当前电流大于设定电流，占空比减小
			{	
				ret_ration = (ration-step <=0)?0:(ration-step);
			}
			else if(current < expect)	//当前电流小于设定电流，占空比增大，最大值90
			{	
				ret_ration = (ration + step >=90)?90:(ration + step);
			}
		}
		
		//pwm设定 //默认为100hz  10ms
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, ret_ration);
	}
	
	return ret_ration;
}

/*******************************************************************************
 * 名称: vkPluseGetIdle
 * 功能: 两个脉冲都处于低电平状态
 * 形参: 
 * 返回: 无
 * 说明: 无
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
 * 名称: vkPluseTrimExpt
 * 功能: 根据上市时间和下降时间计算脉冲强度
 * 形参: 
 * 返回: 无
 * 说明: 无
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
	//电极A间隔时间
	if(gPluse[0].p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse[0].p_poqflags == 0)
		{
			gPluse[0].p_poqflags = 1;
			
			vkPWMStop(gPluse[0].pwm);

			//第一路开关设置低电平，电极停止放电
			GPIO_ResetBits(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin);	//断开
			//第二路开关设置低电平，保持3.3V
			GPIO_ResetBits(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin);	//断开
	
			GPIO_ResetBits(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin);	//断开

			/* 设置低电平 */
			gPluse[0].p_bodlevel = PLUSE_LEVEL_LOW;

			/* 计算脉宽时间Tick计数，计算脉冲周期Tick计数 */
			/* 一次脉冲刺激后，下一次脉冲刺激脉宽和频率设置生效 */
			gPluse[0].p_tickwidth = gPluse[0].p_width/PLUSE_TICK_TIME;
			gPluse[0].p_tickfreqy = ((1000ul*1000ul)/gPluse[0].p_freqy)/PLUSE_TICK_TIME;			
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[0].p_poqcount != 0xFFFF)
		{
			if(gPluse[0].p_poqcount >= gPluse[0].p_interval)
			{
				/* 间隔时间到，上层计数清零 */
				vkPluse1MSSet(PLUSE0, 0);
				gPluse[0].p_poqcount = 0;
				
				/* 周期模式 */
				if(gPluse[0].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[0].p_poqlevel = PLUSE_LEVEL_HIGH;			
				}
				/* 单个模式 */
				else if(gPluse[0].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* 每毫秒增1 */
				gPluse[0].p_poqcount += vkPluse1MSAdd(PLUSE0, 1);
			}
		}
		#endif		
	}

	//电极B间隔时间
	if(gPluse[1].p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse[1].p_poqflags == 0)
		{
			gPluse[1].p_poqflags = 1;

			vkPWMStop(gPluse[1].pwm);

			//第一路开关设置低电平，电极停止放电
			GPIO_ResetBits(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin);	//断开
			//第二路开关设置低电平，保持3.3V
			GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//断开
	
			GPIO_ResetBits(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin);	//断开

			gPluse[1].p_bodlevel = PLUSE_LEVEL_LOW;

			/* 计算脉宽时间Tick计数，计算脉冲周期Tick计数 */
			/* 一次脉冲刺激后，下一次脉冲刺激脉宽和频率设置生效 */
			gPluse[1].p_tickwidth = gPluse[1].p_width/PLUSE_TICK_TIME;
			gPluse[1].p_tickfreqy = ((1000ul*1000ul)/gPluse[1].p_freqy)/PLUSE_TICK_TIME;	
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[1].p_poqcount != 0xFFFF)
		{
			if(gPluse[1].p_poqcount >= gPluse[1].p_interval)
			{
				/* 间隔时间到，上层计数清零 */
				vkPluse1MSSet(PLUSE1, 0);
				gPluse[1].p_poqcount = 0;
				
				/* 周期模式 */
				if(gPluse[1].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[1].p_poqlevel = PLUSE_LEVEL_HIGH;			
				}
				/* 单个模式 */
				else if(gPluse[1].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* 每毫秒增1 */
				gPluse[1].p_poqcount += vkPluse1MSAdd(PLUSE1, 1);
			}
			#endif	
		}
	}

	//电极A持续时间
	if(gPluse[0].p_poqlevel == PLUSE_LEVEL_HIGH)
	{	
		if(gPluse[0].p_bodlevel == PLUSE_LEVEL_LOW)
		{	
			gPluse[0].p_bodcount++;
					
			if(gPluse[0].p_bodcount==1)	//第一个计数，低电平，开始充电	
			{
				//第二路开关设置高电平，接地，迅速拉低电极电压OV
				GPIO_SetBits(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin);

				GPIO_ResetBits(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin);	//断开
				
				//第一路开关设置低电平，电极停止放电
				GPIO_ResetBits(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin);	//断开	
			}			
			else if(gPluse[0].p_bodcount == gPluse[0].p_tickwidth) //延时 PLUSE_MUTEX_DELAY 释放互斥量
			{
				//释放互斥量
				gMutex = 0;	

				//第二路开关设置低电平，
				GPIO_ResetBits(gPluse[0].p_s2_gpio,gPluse[0].p_s2_pin);			//断开				
			}			
			else if(gPluse[0].p_bodcount == (gPluse[0].p_tickwidth+gPluse[0].p_tickwidth+1)) 	//计算电流时间到
			{		
				//计算电流，设置占空比
				//计算电流,此处代码优化
				
				uint16_t adc_V = vkADCRead(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;
				for(uint8_t i=0; i<(gPluse[0].p_tickwidth-1); i++)
				{
					adc_T += gPluse[0].p_bodvalue[i];
				}
				adc_T = adc_T/(gPluse[0].p_tickwidth-1);
				gPluse[0].p_current = (uint16_t)((((1000*PLUSE_VREFINT)/adc_V)*adc_T)/PLUSE0_RESISTANCE);
				//设置脉冲强度并启动PWM，开始充电，积聚电量				
				//vkPluseRation(PLUSE0, 40);	//固定占空比
				
				//uint16_t expect = vkPluseTrimExpt(gPluse[0].p_expect, gPluse[0].p_duration, gPluse[0].p_rise, gPluse[0].p_fall, gPluse[0].p_poqcount);
				gPluse[0].p_ration = vkPluseAdjust(PLUSE0, gPluse[0].p_expect, gPluse[0].p_ration, gPluse[0].p_current);	
			}			
			else if(gPluse[0].p_bodcount >= (gPluse[0].p_tickfreqy-gPluse[0].p_tickwidth)) //计数时间到
			{
				if(gMutex == 0)
				{
					//重置计数，改变电平状态
					gPluse[0].p_bodcount = 0;
					gPluse[0].p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse[0].p_bodlevel == PLUSE_LEVEL_HIGH)	//200us
		{
			//占用互斥量
			gMutex = 1;							
			gPluse[0].p_bodcount++;
			
			//第一个计数，高电平，开始放电
			if(gPluse[0].p_bodcount == 1)
			{
				//关闭PWM，已经积聚了电量，开始放电							
				vkPWMStop(gPluse[0].pwm);

				GPIO_SetBits(gPluse[0].p_s3_gpio,gPluse[0].p_s3_pin);	//接上		
				
				//第二路开关设置低电平，不接地
				//第二路开关设置低电平，保持3.3V
				GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//断开
				
				//第一路开关设置高电平，电极放电0.1ms
				GPIO_SetBits(gPluse[0].p_s1_gpio,gPluse[0].p_s1_pin); //接上
			}			
			else if(gPluse[0].p_bodcount == gPluse[0].p_tickwidth)	//计数时间到width_count0*50us = 200us
			{
				/* 最后一次采集 */
				gPluse[0].p_bodvalue[gPluse[0].p_bodcount-2] = vkADCRead(gPluse[0].p_adcchnl);
				
				//重置计数，改变电平状态
				gPluse[0].p_bodcount = 0;				
				gPluse[0].p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse[0].p_poqflags = 0;
			}
			//第一次不采ADC
			else
			{
				//采集ADC
				gPluse[0].p_bodvalue[gPluse[0].p_bodcount-2] = vkADCRead(gPluse[0].p_adcchnl);
			}
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[0].p_poqcount != 0xFFFF)
		{
			if(gPluse[0].p_poqcount >= gPluse[0].p_duration)
			{
				/* 持续时间到，上层计数清零 */
				vkPluse1MSSet(PLUSE0, 0);
				gPluse[0].p_poqcount = 0;
				
				/* 周期模式 */
				if(gPluse[0].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;			
				}
				/* 单个模式 */
				else if(gPluse[0].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[0].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* 每毫秒增1 */
				gPluse[0].p_poqcount += vkPluse1MSAdd(PLUSE0, 1);
			}
		}
		#endif
	}

	//电极B持续时间
	if(gPluse[1].p_poqlevel == PLUSE_LEVEL_HIGH)	
	{
		if(gPluse[1].p_bodlevel == PLUSE_LEVEL_LOW)	//30ms
		{	
			gPluse[1].p_bodcount++;			
			
			if(gPluse[1].p_bodcount == 1)	//第1个计数，低电平，开始充电			
			{
				//第二路开关设置高电平，接地，迅速拉低电极电压OV
				GPIO_SetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);		//接上

				GPIO_ResetBits(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin);	//断开
				
				//第一路开关设置低电平，电极停止放电
				GPIO_ResetBits(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin);	//断开	
			}			
			else if(gPluse[1].p_bodcount == gPluse[1].p_tickwidth)	//延时 PLUSE_MUTEX_DELAY 释放互斥量
			{
				//释放互斥量
				gMutex = 0;	

				//第二路开关设置低电平，
				GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//断开				
			}			
			else if(gPluse[1].p_bodcount == (gPluse[1].p_tickwidth+gPluse[1].p_tickwidth+1)) //计算电流时间到
			{
				//计算电流，设置占空比
				//计算电流,此处代码优化
				
				uint16_t adc_V = vkADCRead(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;		
				for(uint8_t i=0; i<(gPluse[1].p_tickwidth-1); i++)
				{
					adc_T += gPluse[1].p_bodvalue[i];
				}
				adc_T = adc_T/(gPluse[1].p_tickwidth-1);				
				gPluse[1].p_current = (uint16_t)((((1000*PLUSE_VREFINT)/adc_V)*adc_T)/PLUSE0_RESISTANCE);
				//设置脉冲强度并启动PWM，开始充电，积聚电量				
				//vkPluseRation(PLUSE1, 1);		//固定占空比

				//uint16_t expect = vkPluseTrimExpt(gPluse[1].p_expect, gPluse[1].p_duration, gPluse[1].p_rise, gPluse[1].p_fall, gPluse[1].p_poqcount);
				gPluse[1].p_ration = vkPluseAdjust(PLUSE1, gPluse[1].p_expect, gPluse[1].p_ration, gPluse[1].p_current);	
			}			
			else if(gPluse[1].p_bodcount >= (gPluse[1].p_tickfreqy-gPluse[1].p_tickwidth)) //计数时间到
			{
				if(gMutex == 0)
				{
					//重置计数，改变电平状态
					gPluse[1].p_bodcount = 0;
					gPluse[1].p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse[1].p_bodlevel == PLUSE_LEVEL_HIGH) //200us
		{
			//占用互斥量
			gMutex = 1;
			gPluse[1].p_bodcount++;
			
			//第一个计数，高电平，开始放电
			if(gPluse[1].p_bodcount == 1)
			{
				//关闭PWM，已经积聚了电量，开始放电							
				vkPWMStop(gPluse[1].pwm);

				GPIO_SetBits(gPluse[1].p_s3_gpio,gPluse[1].p_s3_pin); 	//接上		
				
				//第二路开关设置低电平，不接地
				//第二路开关设置低电平，保持3.3V
				GPIO_ResetBits(gPluse[1].p_s2_gpio,gPluse[1].p_s2_pin);	//断开
				
				//第一路开关设置高电平，电极放电0.1ms
				GPIO_SetBits(gPluse[1].p_s1_gpio,gPluse[1].p_s1_pin); 	//接上
			}			
			else if(gPluse[1].p_bodcount == gPluse[1].p_tickwidth)		//计数时间到width_count1*50us = 200us
			{
				/* 最后一次采集 */
				gPluse[1].p_bodvalue[gPluse[1].p_bodcount-2] = vkADCRead(gPluse[1].p_adcchnl);
								
				//重置计数，改变电平状态
				gPluse[1].p_bodcount = 0; 			
				gPluse[1].p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse[1].p_poqflags = 0;
			}
			else
			{
				//采集ADC
				gPluse[1].p_bodvalue[gPluse[1].p_bodcount-2] = vkADCRead(gPluse[1].p_adcchnl);
			}
		}

		#if PLUSE_USING_POQ_HARDTIMER == 1u
		if(gPluse[1].p_poqcount != 0xFFFF)
		{
			if(gPluse[1].p_poqcount >= gPluse[1].p_duration)
			{
				/* 持续时间到，上层计数清零 */
				vkPluse1MSSet(PLUSE1, 0);
				gPluse[1].p_poqcount = 0;
				
				/* 周期模式 */
				if(gPluse[1].p_mode == PLUSE_MODE_CONTINUE)
				{				
					gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW; 		
				}
				/* 单个模式 */
				else if(gPluse[1].p_mode == PLUSE_MODE_ONLYONCE)
				{
					gPluse[1].p_poqlevel = PLUSE_LEVEL_LOW;
				}
			}
			else
			{		
				/* 每毫秒增1 */
				gPluse[1].p_poqcount += vkPluse1MSAdd(PLUSE1, 1);
			}
		}
		#endif
	}
}

