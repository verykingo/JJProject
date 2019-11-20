
/******************************************************************************
 * 文件  ：vkspi.h
 * 描述    ：STM8L SPI通信
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKSPI_H
#define __VKSPI_H

/* 包含系统头文件 */
#include "stm8l15x.h"

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

/* 包含自定义头文件 */

/* 自定义数据类型 */
#define vkSPI SPI_TypeDef*

int vkSPI_Init(vkSPI spi);
int vkSPI_Deinit(vkSPI spi);

int vkSPISend(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data);
int vkSPIRecv(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data);

#endif
