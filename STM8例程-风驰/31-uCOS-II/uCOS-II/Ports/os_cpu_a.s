;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.89
; By        : Jean J. Labrosse
;             Brian Nagel
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : IAR EWARM V6.10
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************
	NAME OS_CPU_A
    
	;声明引用的外部符号--虚拟寄存器
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
        
	EXTERN OSTaskSwHook
	EXTERN OSRunning
	EXTERN OSTCBHighRdy
	EXTERN OSTCBCur
	EXTERN OSPrioCur
	EXTERN OSPrioHighRdy
	EXTERN OSIntEnter
	EXTERN OSIntExit
	EXTERN OSTimeTick
	
	PUBLIC OSStartHighRdy
	PUBLIC OSCtxSw
	PUBLIC OSIntCtxSw

;该宏用于在切换任务时保存当前任务的运行时环境入栈
SAVE_REGS  MACRO 
	pushw y
	pushw x        
	push  a
	push  cc        
	
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

;该宏用于在中断情况下切换任务时保存当前任务的运行时环境入栈
;与普通的任务切换的区别在于发生中断时，已经将y，x，a，cc这些寄存器入栈，所以无需
;再处理，只需将虚拟寄存器入栈
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
		
	IRET
	ENDM  

;该宏用于保存当前任务的堆栈指针
SAVE_OLD_STK_PTR MACRO                        
	LDW    Y,L:OSTCBCur                         //Y=OSTCBCur
	LDW    X,SP                                 //X=SP
	LDW    (Y),X                                //OSTCBCur->OSTCBStkPtr=SP       
	ENDM
      
;该宏用于读取要运行任务的堆栈指针
LOAD_NEW_STK_PTR MACRO
	LDW     Y,   L:OSTCBHighRdy                 //Y=OSTCBHighRdy     
	LDW     Y,(Y)                               //Y= OSTCBHighRdy->OSTCBStkPtr
	LDW     SP, Y         
	ENDM



;********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set the main stack to OS_CPU_ExceptStkBase
;              d) Set OSRunning to TRUE;
;              e) Trigger PendSV exception;
;              f) Enable interrupts (tasks will run with interrupts enabled).
;********************************************************************************************************
	SECTION `.far_func.text`:CODE:ROOT(0)
OSStartHighRdy:
	CALLF	OSTaskSwHook	           //OSTaskSwHook()
	MOV		L:OSRunning,#1	           //OSRunning=1
	
	LOAD_NEW_STK_PTR	
	RESTORE_REGS      
        
        
;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;********************************************************************************************************   
	SECTION `.far_func.text`:CODE:ROOT(0)
OSCtxSw: 
	SAVE_REGS
	SAVE_OLD_STK_PTR
	
	CALLF	OSTaskSwHook		            	//OSTaskSwHook()
	
	MOV     L:OSPrioCur,L:OSPrioHighRdy         //OSPrioCur=OSPrioHighRdy
	LDW     X, L:OSTCBHighRdy 
	LDW     L:OSTCBCur, X                     //OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	RESTORE_REGS 
 
 
;********************************************************************************************************
;                             PERFORM A CONTEXT SWITCH (From interrupt level)
;                                         void OSIntCtxSw(void)
;
; Notes:    1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;******************************************************************************************************** 
	SECTION `.far_func.text`:CODE:ROOT(0)
OSIntCtxSw:
	CALLF	OSTaskSwHook			   			//OSTaskSwHook()
	
	MOV     L:OSPrioCur,L:OSPrioHighRdy         //OSPrioCur=OSPrioHighRdy
	LDW		X, L:OSTCBHighRdy 
	LDW     L:OSTCBCur, X                       //OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	RESTORE_REGS 

;********************************************************************************************************
;                             			系统时钟节拍中断函数
;			由stm8的TIM1的定时器溢出中断提供uCOS运行的时钟节拍中断，10ms一次中断，
;			在该中断函数中调用OSTimeTick
;
;******************************************************************************************************** 
	PUBLIC _interrupt_13
	SECTION `.far_func.text`:CODE:NOROOT(0)
_interrupt_13:    
	SIM
	
	INT_SAVE_REGS
	
	SAVE_OLD_STK_PTR
	
	BRES	0x5255, #0	
	BRES	0x5256, #0	;清除中断标志位
	
	CALLF	OSIntEnter
	CALLF	OSTimeTick
	CALLF	OSIntExit
	
	RESTORE_REGS
	
	END