;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                             STM8L Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.91
; By        : Johkkk Wen
;             
;
; For       : STM8L
; Mode      : Thumb2
; Toolchain : IAR EWARM
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************
	NAME OS_CPU_A

; Get definitions for virtual registers used by the compiler, ?b0~?b15
#include "vregs.inc"

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

    ;声明引用的外部符号--变量和函数
	EXTERN OSTaskSwHook			;任务切换钩子
	EXTERN OSRunning			;系统运行状态
	EXTERN OSTCBHighRdy			;已就绪最高优先级任务
	EXTERN OSPrioHighRdy		;已就绪最高优先级任务优先级
	EXTERN OSTCBCur				;当前任务
	EXTERN OSPrioCur			;当前任务优先级
	
	EXTERN OSIntEnter			;系统Tick中断进入函数
	EXTERN OSIntExit			;系统Tick中断退出函数
	EXTERN OSTimeTick			;系统时间Tick函数

	;汇编声明全局变量，C语言可用
	PUBLIC OSStartHighRdy		
	PUBLIC OSCtxSw
	PUBLIC OSIntCtxSw

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
	LDW    Y,L:OSTCBCur                         //Y=OSTCBCur 	当前任务控制块
	LDW    X,SP                                 //X=SP			堆栈指针
	LDW    (Y),X                                //OSTCBCur->OSTCBStkPtr = SP       
	ENDM
      
;该宏用于读取要运行任务的堆栈指针
LOAD_NEW_STK_PTR MACRO
	LDW     Y,   L:OSTCBHighRdy                 //Y=OSTCBHighRdy     
	LDW     Y,(Y)                               //Y= OSTCBHighRdy->OSTCBStkPtr
	LDW     SP, Y         						//SP = OSTCBCur->OSTCBStkPtr
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
	CALL	OSTaskSwHook	           //OSTaskSwHook()
	MOV		L:OSRunning,#1	           //OSRunning=1
	
	LOAD_NEW_STK_PTR	
	RESTORE_REGS      
        
        
;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              is where the real work is done.
;********************************************************************************************************   
	SECTION `.far_func.text`:CODE:ROOT(0)
OSCtxSw: 
	SAVE_REGS
	SAVE_OLD_STK_PTR
	
	CALL	OSTaskSwHook		            	//OSTaskSwHook()
	
	MOV     L:OSPrioCur,L:OSPrioHighRdy         //OSPrioCur=OSPrioHighRdy
	LDW     X, L:OSTCBHighRdy 
	LDW     L:OSTCBCur, X                     	//OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	RESTORE_REGS


;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the TrapSV exception which is where the real work is done.
;
;           Trap中断 <=> INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler) <=> _interrupt_1
;********************************************************************************************************   
	PUBLIC _interrupt_1
	SECTION `.far_func.text`:CODE:NOROOT(0)
_interrupt_1: 
	INT_SAVE_REGS
	SAVE_OLD_STK_PTR
	
	CALL	OSTaskSwHook		            	//OSTaskSwHook()
	
	MOV     L:OSPrioCur,L:OSPrioHighRdy         //OSPrioCur=OSPrioHighRdy
	LDW     X, L:OSTCBHighRdy 
	LDW     L:OSTCBCur, X                     	//OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	INT_RESTORE_REGS 
 
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
	CALL	OSTaskSwHook			   			//OSTaskSwHook()
	
	MOV     L:OSPrioCur,L:OSPrioHighRdy         //OSPrioCur=OSPrioHighRdy
	LDW		X, L:OSTCBHighRdy 
	LDW     L:OSTCBCur, X                       //OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	INT_RESTORE_REGS
	
;********************************************************************************************************
;                             			系统时钟节拍中断函数
;			由STM8L的TIM1的定时器溢出中断提供uCOS运行的时钟节拍中断，10ms一次中断，
;			在该中断函数中调用OSTimeTick
;			TIME1 <=> INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,23) <=> _interrupt_25
;
;******************************************************************************************************** 
	PUBLIC _interrupt_25
	SECTION `.far_func.text`:CODE:NOROOT(0)
_interrupt_25:    
	SIM					;禁止中断
	
	INT_SAVE_REGS	
	SAVE_OLD_STK_PTR
	
	BRES	0x52B6, #0	;清除中断标志位
	BRES	0x52B7, #0	;清除中断标志位
	
	CALL	OSIntEnter
	CALL	OSTimeTick
	CALL	OSIntExit

	INT_RESTORE_REGS
	
	END