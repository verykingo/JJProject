/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
 
	NAME PORTASM

; Get definitions for virtual registers used by the compiler, ?b0~?b15
#include "vregs.inc"
//#include <FreeRTOSConfig.h>
	;声明引用的外部符号--虚拟寄存器，IAR编译器将RAM前16字节空间?b0~?b15用作虚拟寄存器，因为stm8寄存器不够用
	EXTERN ?b0
	EXTERN ?b1  
	EXTERN ?b2        
	EXTERN ?b3
	EXTERN ?b4
	EXTERN ?b5
	EXTERN ?b6
	EXTERN ?b7
	EXTERN ?b8
	EXTERN ?b9
	EXTERN ?b10
	EXTERN ?b11
	EXTERN ?b12
	EXTERN ?b13
	EXTERN ?b14       
	EXTERN ?b15    

//	RSEG    CODE:CODE(2)
//	thumb

    ;声明引用的外部符号--变量和函数
	EXTERN vPortYieldFromISR
	EXTERN pxCurrentTCB						/* 当前任务控制块 */
	EXTERN vTaskSwitchContext				/* 任务环境切换 */

	;汇编声明全局变量，C语言可用
	PUBLIC vSetMSP							/* ??? */
	PUBLIC xPortPendSVHandler				/* pend中断处理函数 */
	PUBLIC vPortSVCHandler					/* SVC中断处理函数 */		
	PUBLIC vPortStartFirstTask				/* 启动第一任务 */
	PUBLIC ulSetInterruptMaskFromISR		/* 设置中断掩码 */
	PUBLIC vClearInterruptMaskFromISR		/* 清除中断掩码 */

;该宏用于在切换普通任务时,保存当前任务的运行时环境入栈，程序计数器PC的地址自动入栈，此时需要保存Y,X,A,CC,b0~b15
SAVE_REGS  MACRO
	push  #0	;调用CALL,PCL,PCH已经自动入栈，PCE手动入栈，相当于CALLF调用进入
	pushw Y
	pushw X        
	push  A
	push  CC        
	
	push s:?b0
	push s:?b1
	
	push s:?b2
	push s:?b3
	
	push s:?b4
	push s:?b5
	
	push s:?b6
	push s:?b7       
	
	push s:?b8
	push s:?b9
	
	push s:?b10
	push s:?b11
	
	push s:?b12
	push s:?b13
	
	push s:?b14
	push s:?b15 
	
	ENDM

;该宏用于恢复普通任务时，恢复要运行任务的堆栈环境
;借助RETF指令中对PC寄存器重新赋值的操作来实现普通任务的切换
RESTORE_REGS  MACRO
	pop s:?b15
	pop s:?b14
	
	pop s:?b13
	pop s:?b12
	
	pop s:?b11
	pop s:?b10
	
	pop s:?b9
	pop s:?b8      
	
	pop s:?b7
	pop s:?b6
	
	pop s:?b5
	pop s:?b4
	
	pop s:?b3
	pop s:?b2
	
	pop s:?b1
	pop s:?b0

	pop  CC
	pop  A
	popw X
	popw Y
		
	RETF		;PC出栈，相当于CALLF调用退出
	ENDM  

;该宏用于在中断情况下切换中断任务时，保存当前任务的运行时环境入栈
;与普通的任务切换的区别在于发生中断时，已经将y，x，a，cc这些
;寄存器入栈，所以无需再处理，只需将虚拟寄存器入栈
INT_SAVE_REGS  MACRO        
	push s:?b0
	push s:?b1
	
	push s:?b2
	push s:?b3
	
	push s:?b4
	push s:?b5
	
	push s:?b6
	push s:?b7       
	
	push s:?b8
	push s:?b9
	
	push s:?b10
	push s:?b11
	
	push s:?b12
	push s:?b13
	
	push s:?b14
	push s:?b15 
	
	ENDM        
        
;该宏用于切换任务时恢复要运行任务的堆栈环境
;借助iret指令中对PC寄存器重新赋值的操作来实现任务的切换
INT_RESTORE_REGS  MACRO     
	pop s:?b15
	pop s:?b14
	
	pop s:?b13
	pop s:?b12
	
	pop s:?b11
	pop s:?b10
	
	pop s:?b9
	pop s:?b8      
	
	pop s:?b7
	pop s:?b6
	
	pop s:?b5
	pop s:?b4
	
	pop s:?b3
	pop s:?b2
	
	pop s:?b1
	pop s:?b0
		
	IRET
	ENDM 

;该宏用于保存当前任务的堆栈指针
SAVE_OLD_STK_PTR MACRO                        
	LDW    Y,L:pxCurrentTCB                    	//Y=pxCurrentTCB 	当前任务控制块
	LDW    X,SP                                 //X=SP				堆栈指针
	LDW    (Y),X                                //pxCurrentTCB->pxTopOfStack = SP       
	ENDM	

/*-----------------------------------------------------------*/

vSetMSP
	msr msp, r0
	bx lr

/*-----------------------------------------------------------*/

xPortPendSVHandler:
	mrs r0, psp

	ldr	r3, =pxCurrentTCB	/* Get the location of the current TCB. */
	ldr	r2, [r3]

	subs r0, r0, #32		/* Make space for the remaining low registers. */
	str r0, [r2]			/* Save the new top of stack. */
	stmia r0!, {r4-r7}		/* Store the low registers that are not saved automatically. */
	mov r4, r8				/* Store the high registers. */
	mov r5, r9
	mov r6, r10
	mov r7, r11
	stmia r0!, {r4-r7}

	push {r3, r14}
	cpsid i
	bl vTaskSwitchContext
	cpsie i
	pop {r2, r3}			/* lr goes in r3. r2 now holds tcb pointer. */

	ldr r1, [r2]
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	adds r0, r0, #16		/* Move to the high registers. */
	ldmia r0!, {r4-r7}		/* Pop the high registers. */
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r11, r7

	msr psp, r0				/* Remember the new top of stack for the task. */

	subs r0, r0, #32		/* Go back for the low registers that are not automatically restored. */
	ldmia r0!, {r4-r7}		/* Pop low registers.  */

	bx r3

/*-----------------------------------------------------------*/

vPortSVCHandler;
	/* This function is no longer used, but retained for backward
	compatibility. */
	bx lr

/*-----------------------------------------------------------*/

vPortStartFirstTask
	/* The MSP stack is not reset as, unlike on M3/4 parts, there is no vector
	table offset register that can be used to locate the initial stack value.
	Not all M0 parts have the application vector table at address 0. */

	ldr	r3, =pxCurrentTCB	/* Obtain location of pxCurrentTCB. */
	ldr r1, [r3]
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	adds r0, #32			/* Discard everything up to r0. */
	msr psp, r0				/* This is now the new top of stack to use in the task. */
	movs r0, #2				/* Switch to the psp stack. */
	msr CONTROL, r0
	isb
	pop {r0-r5}				/* Pop the registers that are saved automatically. */
	mov lr, r5				/* lr is now in r5. */
	pop {r3}				/* The return address is now in r3. */
	pop {r2}				/* Pop and discard the XPSR. */
	cpsie i					/* The first task has its context and interrupts can be enabled. */
	bx r3					/* Jump to the user defined task code. */

/*-----------------------------------------------------------*/

ulSetInterruptMaskFromISR
	mrs r0, PRIMASK
	cpsid i
	bx lr

/*-----------------------------------------------------------*/

vClearInterruptMaskFromISR
	msr PRIMASK, r0
	bx lr

	END
