
/******************************************************************************
 * �ļ�  ��vkspi.h
 * ����    ��STM8L SPIͨ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKSPI_H
#define __VKSPI_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

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

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */
#define vkSPI SPI_TypeDef*

int vkSPI_Init(vkSPI spi);
int vkSPI_Deinit(vkSPI spi);

int vkSPISend(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data);
int vkSPIRecv(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data);

#endif
