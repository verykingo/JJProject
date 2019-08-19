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

    ;�������õ��ⲿ����--�����ͺ���
	EXTERN OSTaskSwHook			;�����л�����
	EXTERN OSRunning			;ϵͳ����״̬
	EXTERN OSTCBHighRdy			;�Ѿ���������ȼ�����
	EXTERN OSPrioHighRdy		;�Ѿ���������ȼ��������ȼ�
	EXTERN OSTCBCur				;��ǰ����
	EXTERN OSPrioCur			;��ǰ�������ȼ�
	
	EXTERN OSIntEnter			;ϵͳTick�жϽ��뺯��
	EXTERN OSIntExit			;ϵͳTick�ж��˳�����
	EXTERN OSTimeTick			;ϵͳʱ��Tick����

	;�������ȫ�ֱ�����C���Կ���
	PUBLIC OSStartHighRdy		
	PUBLIC OSCtxSw
	PUBLIC OSIntCtxSw

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
	LDW    Y,L:OSTCBCur                         //Y=OSTCBCur 	��ǰ������ƿ�
	LDW    X,SP                                 //X=SP			��ջָ��
	LDW    (Y),X                                //OSTCBCur->OSTCBStkPtr = SP       
	ENDM
      
;�ú����ڶ�ȡҪ��������Ķ�ջָ��
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
;           Trap�ж� <=> INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler) <=> _interrupt_1
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
;                             			ϵͳʱ�ӽ����жϺ���
;			��STM8L��TIM1�Ķ�ʱ������ж��ṩuCOS���е�ʱ�ӽ����жϣ�10msһ���жϣ�
;			�ڸ��жϺ����е���OSTimeTick
;			TIME1 <=> INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,23) <=> _interrupt_25
;
;******************************************************************************************************** 
	PUBLIC _interrupt_25
	SECTION `.far_func.text`:CODE:NOROOT(0)
_interrupt_25:    
	SIM					;��ֹ�ж�
	
	INT_SAVE_REGS	
	SAVE_OLD_STK_PTR
	
	BRES	0x52B6, #0	;����жϱ�־λ
	BRES	0x52B7, #0	;����жϱ�־λ
	
	CALL	OSIntEnter
	CALL	OSTimeTick
	CALL	OSIntExit

	INT_RESTORE_REGS
	
	END