/*
 * 软定时器
 */

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
typedef void ( * TIMER_CB_FUNC ) ( POINTER cb_data ) ;

/* Data structures */
/* Timer descriptor 定时器定义 */
typedef struct soft_timer
{
	POINTER			timer_name;			/* Timer Name 定时器名字 */
    TIMER_CB_FUNC   cb_func;    		/* Callback function 回调函数 */
    POINTER	        cb_data;    		/* Pointer to callback parameter/data 函数参数 */
    uint32_t	    cb_ticks;   		/* Ticks until callback 定时时间 */

	/* Internal data */
    struct soft_timer *next_timer;		/* Next timer in doubly-linked list 下一定时器指针 */

} vkTIMER;

#define TIMER_STRUCT_SIZE	20			/* 软定时器大小			*/
#define TIMER_STATCK_SIZE 	400			/* 软定时器堆栈大小 */

/* Function prototypes */
extern int8_t 	vkTimerInsert(vkTIMER *timer_ptr);
extern int8_t 	vkTimerCancel(vkTIMER *timer_ptr);
extern int8_t 	vkTimerClear (void);
extern uint32_t vkTimerGet (void);
extern void 	vkTimerSet (uint32_t new_time);

extern void vkTimerTick (void);

#endif
