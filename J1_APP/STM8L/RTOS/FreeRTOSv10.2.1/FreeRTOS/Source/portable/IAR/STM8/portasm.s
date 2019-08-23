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
	;�������õ��ⲿ����--����Ĵ�����IAR��������RAMǰ16�ֽڿռ�?b0~?b15��������Ĵ�������Ϊstm8�Ĵ���������
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

    ;�������õ��ⲿ����--�����ͺ���
	EXTERN vPortYieldFromISR
	EXTERN pxCurrentTCB						/* ��ǰ������ƿ� */
	EXTERN vTaskSwitchContext				/* ���񻷾��л� */

	;�������ȫ�ֱ�����C���Կ���
	PUBLIC vSetMSP							/* ??? */
	PUBLIC xPortPendSVHandler				/* pend�жϴ����� */
	PUBLIC vPortSVCHandler					/* SVC�жϴ����� */		
	PUBLIC vPortStartFirstTask				/* ������һ���� */
	PUBLIC ulSetInterruptMaskFromISR		/* �����ж����� */
	PUBLIC vClearInterruptMaskFromISR		/* ����ж����� */

;�ú��������л���ͨ����ʱ,���浱ǰ���������ʱ������ջ�����������PC�ĵ�ַ�Զ���ջ����ʱ��Ҫ����Y,X,A,CC,b0~b15
SAVE_REGS  MACRO
	push  #0	;����CALL,PCL,PCH�Ѿ��Զ���ջ��PCE�ֶ���ջ���൱��CALLF���ý���
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

;�ú����ڻָ���ͨ����ʱ���ָ�Ҫ��������Ķ�ջ����
;����RETFָ���ж�PC�Ĵ������¸�ֵ�Ĳ�����ʵ����ͨ������л�
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
		
	RETF		;PC��ջ���൱��CALLF�����˳�
	ENDM  

;�ú��������ж�������л��ж�����ʱ�����浱ǰ���������ʱ������ջ
;����ͨ�������л����������ڷ����ж�ʱ���Ѿ���y��x��a��cc��Щ
;�Ĵ�����ջ�����������ٴ���ֻ�轫����Ĵ�����ջ
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
        
;�ú������л�����ʱ�ָ�Ҫ��������Ķ�ջ����
;����iretָ���ж�PC�Ĵ������¸�ֵ�Ĳ�����ʵ��������л�
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

;�ú����ڱ��浱ǰ����Ķ�ջָ��
SAVE_OLD_STK_PTR MACRO                        
	LDW    Y,L:pxCurrentTCB                    	//Y=pxCurrentTCB 	��ǰ������ƿ�
	LDW    X,SP                                 //X=SP				��ջָ��
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
