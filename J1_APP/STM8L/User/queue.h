/*
 * ѭ���������
 */

#ifndef __QUEUE_H_
#define __QUEUE_H_

/* ƽ̨����������Ͷ��� */
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

/* ���з��ز������� */
#define QUEUE_OK                  0u	//��ȷ
#define QUEUE_TIMEOUT             1u	//��ʱ
#define QUEUE_WOULDBLOCK          2u	//����
#define QUEUE_ERR_PARAM           3u	//��������

#ifdef __cplusplus
extern "C" {
#endif

typedef struct queue
{
    uint8_t *   buff_ptr;       	/* Pointer to queue data area ��������������ʼ��ַ */
    uint32_t    unit_size;      	/* Size of each message �������ݵ�Ԫ��С */
    uint32_t    max_num_msgs;   	/* Max number of storable messages ���ɴ洢���ݵ�Ԫ�� */
    uint32_t    insert_index;   	/* Next byte index to insert into ���ݵ�Ԫ����λ�� */
    uint32_t    remove_index;   	/* Next byte index to remove from ���ݵ�Ԫɾ��λ�� */
    uint32_t    num_msgs_stored;	/* Number of messages stored �Ѵ������ݵ�Ԫ�� */
} QUEUE;

extern uint8_t QueueCreate (QUEUE *qptr, uint8_t *buff_ptr, uint32_t unit_size, uint32_t max_num_msgs);
extern uint8_t QueueDestory (QUEUE *qptr);
extern uint8_t QueueGet (QUEUE *qptr, uint32_t timeout, uint8_t *msgptr);
extern uint8_t QueuePut (QUEUE *qptr, uint32_t timeout, uint8_t *msgptr);



#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H */
