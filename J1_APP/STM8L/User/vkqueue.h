/*
 * 循环数组队列
 */

#ifndef __QUEUE_H_
#define __QUEUE_H_

/* 平台相关数据类型定义 */
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

/* 队列返回参数定义 */
#define vkQUEUE_OK                  0	//正确
#define vkQUEUE_TIMEOUT             -1	//超时
#define vkQUEUE_WOULDBLOCK          -2	//阻塞
#define vkQUEUE_ERR_PARAM           -3	//参数错误
#define vkQUEUE_NULL				-4	//空队列
#define vkQUEUE_FILLED				-5	//满队列
#define vkQUEUE_BEING				-6	//队列有元素

#ifdef __cplusplus
extern "C" {
#endif

typedef struct queue
{
    uint8_t *   buff_ptr;       	/* Pointer to queue data area 队列数据区域起始地址 */
    uint32_t    unit_size;      	/* Size of each message 队列数据单元大小 */
    uint32_t    max_num_msgs;   	/* Max number of storable messages 最大可存储数据单元数 */
    uint32_t    insert_index;   	/* Next byte index to insert into 数据单元插入位置 */
    uint32_t    remove_index;   	/* Next byte index to remove from 数据单元删除位置 */
    uint32_t    num_msgs_stored;	/* Number of messages stored 已存入数据单元数 */
} vkQUEUE;

extern int8_t vkQueueCreate (vkQUEUE *qptr, uint8_t *buff_ptr, uint32_t unit_size, uint32_t max_num_msgs);
extern int8_t vkQueueDestory (vkQUEUE *qptr);
extern int8_t vkQueueGet (vkQUEUE *qptr, uint32_t timeout, uint8_t *msgptr);
extern int8_t vkQueuePut (vkQUEUE *qptr, uint32_t timeout, uint8_t *msgptr);
extern int8_t vkQueueClear (vkQUEUE *qptr);
extern int8_t vkQueueEmpty (vkQUEUE *qptr);



#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H */
