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
#include "vktimetick.h"

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
#define USING_TIMER_TICKS		0u	//使用软定时器tick

#define vkTIMER_OK        		 0	//成功正确
#define vkTIMER_ERR_PARAM 		-1	//参数错误
#define vkTIMER_ERR_NOT_FOUND	-2	//没有发现

enum CB_FUNC_TYPE
{
	CB_FUNC_TYPE_TIMER = 0,		//定时器执行
	CB_FUNC_TYPE_WHILE = 1,		//主循环执行
};

enum CB_RUNING_MODE
{
	CB_RUNING_MODE_ONCE = 0,	//仅运行一次
	CB_RUNING_MODE_KEEP = 1,	//持续运行
};

/* Architecture-specific types */
#define POINTER   void *

/* Callback function prototype */
typedef void ( * vkTIMER_CB_FUNC ) ( POINTER cb_data ) ;

/* Data structures */
/* Timer descriptor 定时器定义 */
typedef struct soft_timer
{	
	POINTER			timer_name;			/* Timer Name 定时器名字 */
	uint8_t			cb_type;			/* Callback type 回调类型，中断回调/while回调 */
	uint8_t			cb_mode;			/* 单次调用/持续调用 */
    vkTIMER_CB_FUNC cb_func;    		/* Callback function 回调函数 */
    POINTER	        cb_data;    		/* Pointer to callback parameter/data 函数参数 */
    uint32_t	    cb_ticks;   		/* Ticks until callback 定时时间 */

	/* Internal data */
	struct soft_timer *prev_timer;		/* Next timer in doubly-linked list 下一定时器指针 */
    struct soft_timer *next_timer;		/* Next timer in doubly-linked list 下一定时器指针 */

} vkTIMER;

#define TIMER_STRUCT_NUMS 	15ul						/* 软定时器最大数目	*/
#define TIMER_STRUCT_SIZE	sizeof(vkTIMER)				/* 软定时器大小	*/
#define TIMER_STATCK_SIZE 	(TIMER_STRUCT_NUMS*TIMER_STRUCT_SIZE)		/* 软定时器堆栈大小 */
#define TIMER_US_PER_TICK	SOFTTIMER_US_PER_TICKS						/* 定时器每个tick是10000us */

/* 微秒转换 */
#define vkTICKS_TO_US(x) 	((uint32_t)(x*TIMER_US_PER_TICK))
#define vkUS_TO_TICKS(x) 	((uint32_t)(x/TIMER_US_PER_TICK))

/* 毫秒转换 */
#define vkTICKS_TO_MS(x) 	((uint32_t)((x*TIMER_US_PER_TICK)/1000ul))
#define vkMS_TO_TICKS(x) 	((uint32_t)((x*1000ul)/TIMER_US_PER_TICK))

/* 秒转换 */
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
