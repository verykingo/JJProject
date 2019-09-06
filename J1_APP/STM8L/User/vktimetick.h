
/******************************************************************************
 * �ļ�  ��vktimetick.h
 * ����    ��STM8L���Ķ�ʱ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKTIMETICK_H
#define __VKTIMETICK_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */

/* �Զ����������� */

/* ÿһ��Tick����10����(���ʱʱ��2^32*10ms=42949000s)ʱ�� */
#define US_PER_TICKS	((uint32_t)(10000ul))

/* ΢��ת�� */
#define vkTICKS_TO_US(x) 	((uint32_t)(x*US_PER_TICKS))
#define vkUS_TO_TICKS(x) 	((uint32_t)(x/US_PER_TICKS))

/* ����ת�� */
#define vkTICKS_TO_MS(x) 	((uint32_t)(x*US_PER_TICKS/1000ul))
#define vkMS_TO_TICKS(x) 	((uint32_t)(x*1000ul/US_PER_TICKS))

/* ��ת�� */
#define vkTICKS_TO_SS(x) 	((uint32_t)(x*US_PER_TICKS/1000ul*1000ul))
#define vkSS_TO_TICKS(x) 	((uint32_t)(x*1000ul*1000ul/US_PER_TICKS))

void vkTimeTick_Init(uint8_t TIM);

#endif

