/******************************************************************************
 * 文件  ：vksofttimer.h
 * 描述    ：软定时器，允许多次插入同一个定时器，一次性删除队列中名字相同的多个定时器
           如果要确保定时器链表中只有一个特定名字的定时器，则必须在插入之前，先删除
           该定时器。
 * 平台    ：ALL
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKSOFTTIMER_H_
#define __VKSOFTTIMER_H_

/***********************************************************************************/
/* 平台相关数据类型定义 */
/***********************************************************************************/
#include "stm8l15x.h"

/* 平台相关临界区域保护 */
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
/* 平台相关结束 */
/***********************************************************************************/

#define vkTIMER_OK        		0
#define vkTIMER_ERR_PARAM 		-1
#define vkTIMER_ERR_NOT_FOUND	-2


/* Architecture-specific types */
#define POINTER   void *

/* Callback function prototype */
typedef void ( * vkTIMER_CB_FUNC ) ( POINTER cb_data ) ;

/* Data structures */
/* Timer descriptor 定时器定义 */
typedef struct soft_timer
{
	POINTER			timer_name;			/* Timer Name 定时器名字 */
    vkTIMER_CB_FUNC cb_func;    		/* Callback function 回调函数 */
    POINTER	        cb_data;    		/* Pointer to callback parameter/data 函数参数 */
    uint32_t	    cb_ticks;   		/* Ticks until callback 定时时间 */

	/* Internal data */
	struct soft_timer *prev_timer;		/* Next timer in doubly-linked list 下一定时器指针 */
    struct soft_timer *next_timer;		/* Next timer in doubly-linked list 下一定时器指针 */

} vkTIMER;

#define TIMER_STRUCT_NUMS 	16ul						/* 软定时器最大数目	*/
#define TIMER_STRUCT_SIZE	sizeof(vkTIMER)				/* 软定时器大小	*/
#define TIMER_STATCK_SIZE 	(TIMER_STRUCT_NUMS*TIMER_STRUCT_SIZE)		/* 软定时器堆栈大小 */
#define TIMER_US_PER_TICK	1000ul						/* 定时器每个tick是1000us */

/* Function prototypes */
extern int8_t 	vkTimerInsert(vkTIMER *timer_ptr);
extern int8_t 	vkTimerCancel(vkTIMER *timer_ptr);
extern int8_t 	vkTimerClear (void);
extern int8_t 	vkTimerSetTicks (uint32_t new_time);
extern uint32_t vkTimerGetTicks (void);
extern void vkTimerDelayMS(uint32_t msec);
extern void vkTimerDelaySS(uint32_t s);

extern void vkTimerTick (void);

#endif
