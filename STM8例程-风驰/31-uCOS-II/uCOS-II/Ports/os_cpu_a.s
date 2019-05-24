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
    
	;�������õ��ⲿ����--����Ĵ���
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

;�ú��������л�����ʱ���浱ǰ���������ʱ������ջ
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

;�ú��������ж�������л�����ʱ���浱ǰ���������ʱ������ջ
;����ͨ�������л����������ڷ����ж�ʱ���Ѿ���y��x��a��cc��Щ�Ĵ�����ջ����������
;�ٴ���ֻ�轫����Ĵ�����ջ
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

;�ú����ڱ��浱ǰ����Ķ�ջָ��
SAVE_OLD_STK_PTR MACRO                        
	LDW    Y,L:OSTCBCur                         //Y=OSTCBCur
	LDW    X,SP                                 //X=SP
	LDW    (Y),X                                //OSTCBCur->OSTCBStkPtr=SP       
	ENDM
      
;�ú����ڶ�ȡҪ��������Ķ�ջָ��
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
;                             			ϵͳʱ�ӽ����жϺ���
;			��stm8��TIM1�Ķ�ʱ������ж��ṩuCOS���е�ʱ�ӽ����жϣ�10msһ���жϣ�
;			�ڸ��жϺ����е���OSTimeTick
;
;******************************************************************************************************** 
	PUBLIC _interrupt_13
	SECTION `.far_func.text`:CODE:NOROOT(0)
_interrupt_13:    
	SIM
	
	INT_SAVE_REGS
	
	SAVE_OLD_STK_PTR
	
	BRES	0x5255, #0	
	BRES	0x5256, #0	;����жϱ�־λ
	
	CALLF	OSIntEnter
	CALLF	OSTimeTick
	CALLF	OSIntExit
	
	RESTORE_REGS
	
	END