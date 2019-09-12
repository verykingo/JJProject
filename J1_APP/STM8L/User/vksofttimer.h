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

#define vkTIMER_OK        		0
#define vkTIMER_ERR_PARAM 		-1
#define vkTIMER_ERR_NOT_FOUND	-2


/* Architecture-specific types */
#define POINTER   void *

/* Callback function prototype */
typedef void ( * vkTIMER_CB_FUNC ) ( POINTER cb_data ) ;

/* Data structures */
/* Timer descriptor ��ʱ������ */
typedef struct soft_timer
{
	POINTER			timer_name;			/* Timer Name ��ʱ������ */
    vkTIMER_CB_FUNC cb_func;    		/* Callback function �ص����� */
    POINTER	        cb_data;    		/* Pointer to callback parameter/data �������� */
    uint32_t	    cb_ticks;   		/* Ticks until callback ��ʱʱ�� */

	/* Internal data */
    struct soft_timer *next_timer;		/* Next timer in doubly-linked list ��һ��ʱ��ָ�� */

} vkTIMER;

#define TIMER_STRUCT_SIZE	20			/* ��ʱ����С			*/
#define TIMER_STATCK_SIZE 	400			/* ��ʱ����ջ��С */
#define TIMER_US_PER_TICK	10000		/* ��ʱ��ÿ��tick��10000us */

/* Function prototypes */
extern int8_t 	vkTimerInsert(vkTIMER *timer_ptr);
extern int8_t 	vkTimerCancel(vkTIMER *timer_ptr);
extern int8_t 	vkTimerClear (void);
extern uint32_t vkTimerGetTicks (void);
extern int8_t 	vkTimerSetTicks (uint32_t new_time);
extern void vkTimerDelayMS(uint32_t msec);
extern void vkTimerDelayS(uint32_t s);

extern void vkTimerTick (void);

#endif
