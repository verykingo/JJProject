/****************************************Copyright (c)**************************************************
**                               Shenzhen VeryKing Technology Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:               os_cpu.h
** Last modified Date:      2019.08.08
** Last Version:            2.2
** Description:             Header file of the ported code  移植代码头文件
** 
**------------------------------------------------------------------------------------------------------
** Created By:              Johkkk Wen
** Created date:            2019.08.08
** Version:                 2.2
** Descriptions:            The 2th version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Description:             
**
*********************************************************************************************************/

#ifndef  __OS_CPU_H
#define  __OS_CPU_H

#include <intrinsics.h>

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

/*******************************************************************************
  Date types(Compiler specific)  数据类型（和编译器相关）                
*******************************************************************************/
typedef unsigned char   	BOOLEAN;	/*  Boolean 布尔变量            */
typedef unsigned char   	INT8U;      /*  Unsigned  8 bit quantity    */                       
typedef signed   char   	INT8S;      /*  Signed    8 bit quantity    */                         
typedef unsigned short  	INT16U;     /*  Unsigned 16 bit quantity    */
typedef signed   short  	INT16S;     /*  Signed   16 bit quantity    */
typedef unsigned long   	INT32U;     /*  Unsigned 32 bit quantity    */
typedef signed   long   	INT32S;     /*  Signed   32 bit quantity    */
typedef float           	FP32;       /*  Single precision floating point */
typedef double          	FP64;       /*  Double precision floating point */

typedef unsigned char    	OS_STK;     /*  Each stack entry is 8-bit wide  堆栈是8位宽度 */   
typedef unsigned char    	OS_CPU_SR;  /*  定义 CPU 状态寄存器为位宽为8位 (PSR = 8 bits) */

typedef signed char	     	int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef	signed long			int32;
typedef unsigned long		uint32;


/*********************************************************************************************************
  Method of critical section management  临界区管理方法                
*********************************************************************************************************/
#define  OS_CRITICAL_METHOD  3u


/*********************************************************************************************************
  Other definitions  其他定义         
*********************************************************************************************************/
#define  OS_STK_GROWTH     	1u					/* 堆栈增长方向，STM8堆栈指针递减（高地址到低地址） */            
//#define  OS_TASK_SW()      	OSCtxSw()			/* 任务切换，直接方式 */
#define  OS_TASK_SW()      	OSTrapSV()			/* 任务切换,使用Trap中断方式 */


/*********************************************************************************************************
  Prototypes(see OS_CPU_A.ASM)  原型声明（见OS_CPU_A.ASM）                
*********************************************************************************************************/

#if OS_CRITICAL_METHOD == 3u
#define OS_ENTER_CRITICAL() {cpu_sr=__get_interrupt_state(); __disable_interrupt();}
#define OS_EXIT_CRITICAL()  __set_interrupt_state(cpu_sr)
#endif   
    
void OSCtxSw(void);
void OSIntCtxSw(void);
void OSStartHighRdy(void);

void OSTimeTickHook (void);
void OSTrapSV(void);

OS_CPU_EXT INT32U OsEnterSum;                   

#endif

/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
