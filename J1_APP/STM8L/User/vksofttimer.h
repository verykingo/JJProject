/******************************************************************************
 * �ļ�  ��vksofttimer.h
 * ����    ����ʱ���������β���ͬһ����ʱ����һ����ɾ��������������ͬ�Ķ����ʱ��
           ���Ҫȷ����ʱ��������ֻ��һ���ض����ֵĶ�ʱ����������ڲ���֮ǰ����ɾ��
           �ö�ʱ����
 * ƽ̨    ��ALL
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKSOFTTIMER_H_
#define __VKSOFTTIMER_H_

/***********************************************************************************/
/* ƽ̨����������Ͷ��� */
/***********************************************************************************/
#include "stm8l15x.h"
#include "vktimetick.h"

/* ƽ̨����ٽ����򱣻� */
/* COSMIC: Use inline assembler */
#if defined(__CSMC__)
#define CRITICAL_STORE      uint8_t ccr
#define CRITICAL_START()    _asm ("push CC\npop a\nld (X),A\nsim", &ccr)
#define CRITICAL_END()      _asm ("ld A,(X)\npush A\npop CC", &ccr)
#define FORCE_USED

/* IAR: Use intrinsics */
#elif defined(__IAR_SYSTEMS_ICC__)
#define CRITICAL_STORE      __istate_t _istate
#define CRITICAL_START()    _istate = __get_interrupt_state(); __disable_interrupt()
#define CRITICAL_END()      __set_interrupt_state(_istate)
#define FORCE_USED  __root

/* Raisonance: Use intrinsics */
#elif defined(__RCSTM8__)
#define CRITICAL_STORE      unsigned char ccr
#define CRITICAL_START()    ccr = _getCC_(); _sim_()
#define CRITICAL_END()      _setCC_(ccr)
#define FORCE_USED
#endif

/***********************************************************************************/
/* ƽ̨��ؽ��� */
/***********************************************************************************/
#define USING_TIMER_TICKS		0u	//ʹ����ʱ��tick

#define vkTIMER_OK        		 0	//�ɹ���ȷ
#define vkTIMER_ERR_PARAM 		-1	//��������
#define vkTIMER_ERR_NOT_FOUND	-2	//û�з���

enum CB_FUNC_TYPE
{
	CB_FUNC_TYPE_TIMER = 0,		//��ʱ��ִ��
	CB_FUNC_TYPE_WHILE = 1,		//��ѭ��ִ��
};

enum CB_RUNING_MODE
{
	CB_RUNING_MODE_ONCE = 0,	//������һ��
	CB_RUNING_MODE_KEEP = 1,	//��������
};

/* Architecture-specific types */
#define POINTER   void *

/* Callback function prototype */
typedef void ( * vkTIMER_CB_FUNC ) ( POINTER cb_data ) ;

/* Data structures */
/* Timer descriptor ��ʱ������ */
typedef struct soft_timer
{	
	POINTER			timer_name;			/* Timer Name ��ʱ������ */
	uint8_t			cb_type;			/* Callback type �ص����ͣ��жϻص�/while�ص� */
	uint8_t			cb_mode;			/* ���ε���/�������� */
    vkTIMER_CB_FUNC cb_func;    		/* Callback function �ص����� */
    POINTER	        cb_data;    		/* Pointer to callback parameter/data �������� */
    uint32_t	    cb_ticks;   		/* Ticks until callback ��ʱʱ�� */

	/* Internal data */
	struct soft_timer *prev_timer;		/* Next timer in doubly-linked list ��һ��ʱ��ָ�� */
    struct soft_timer *next_timer;		/* Next timer in doubly-linked list ��һ��ʱ��ָ�� */

} vkTIMER;

#define TIMER_STRUCT_NUMS 	15ul						/* ��ʱ�������Ŀ	*/
#define TIMER_STRUCT_SIZE	sizeof(vkTIMER)				/* ��ʱ����С	*/
#define TIMER_STATCK_SIZE 	(TIMER_STRUCT_NUMS*TIMER_STRUCT_SIZE)		/* ��ʱ����ջ��С */
#define TIMER_US_PER_TICK	SOFTTIMER_US_PER_TICKS						/* ��ʱ��ÿ��tick��10000us */

/* ΢��ת�� */
#define vkTICKS_TO_US(x) 	((uint32_t)(x*TIMER_US_PER_TICK))
#define vkUS_TO_TICKS(x) 	((uint32_t)(x/TIMER_US_PER_TICK))

/* ����ת�� */
#define vkTICKS_TO_MS(x) 	((uint32_t)((x*TIMER_US_PER_TICK)/1000ul))
#define vkMS_TO_TICKS(x) 	((uint32_t)((x*1000ul)/TIMER_US_PER_TICK))

/* ��ת�� */
#define vkTICKS_TO_SS(x) 	((uint32_t)((x*TIMER_US_PER_TICK)/1000ul*1000ul))
#define vkSS_TO_TICKS(x) 	((uint32_t)((x*1000ul*1000ul)/TIMER_US_PER_TICK))

/* Function prototypes */
extern int8_t 	vkTimerInsert(vkTIMER *timer_ptr);
extern int8_t 	vkTimerCancel(vkTIMER *timer_ptr);
extern uint32_t	vkTimerRemain(vkTIMER *timer_ptr);
extern int8_t 	vkTimerClear (void);

uint32_t vkTimerTaskCreate(void (*task)(void *p_arg),void *p_arg,uint8_t type,uint32_t slice );
void vkTimerTaskDestory(uint32_t handle);


#if USING_TIMER_TICKS==1u
extern int8_t vkTimerSetTicks (uint32_t new_time);
extern int8_t vkTimerGetTicks (uint32_t *new_time );
extern void vkTimerDelayMS(uint32_t msec);
extern void vkTimerDelaySS(uint32_t s);
#endif

extern void vkTimerTick (void);
extern void vkTimerDone (void);

#endif
