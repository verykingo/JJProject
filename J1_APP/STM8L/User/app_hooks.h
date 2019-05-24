/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                      Application-Defined Task Hooks
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*
* File    : APP_HOOKS.H
* By      : Fabiano Kovalski
*         : Eric Shufro
*********************************************************************************************************
*/

#ifndef __APP_HOOKS_H
#define __APP_HOOKS_H

void  App_TaskCreateHook (OS_TCB *ptcb);

void  App_TaskDelHook (OS_TCB *ptcb);

void  App_TaskIdleHook (void);

void  App_TaskReturnHook (OS_TCB  *ptcb);

void  App_TaskStatHook (void);

void  App_TaskSwHook (void);

void  App_TCBInitHook (OS_TCB *ptcb);

void  App_TimeTickHook (void);

#endif
