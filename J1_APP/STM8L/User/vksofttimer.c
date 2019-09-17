/******************************************************************************
 * �ļ�  ��vksofttimer.c
 * ����    ����ʱ���������β���ͬһ����ʱ����һ����ɾ��������������ͬ�Ķ����ʱ��
           ���Ҫȷ����ʱ��������ֻ��һ���ض����ֵĶ�ʱ����������ڲ���֮ǰ����ɾ��
           �ö�ʱ����
 * ƽ̨    ��ALL
 * ʱ��  ��2019-04-01

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vksofttimer.h"

/* Local data */
static uint8_t STACK_TIMER[TIMER_STATCK_SIZE]={0};
static uint8_t STACK_FLAGS[TIMER_STATCK_SIZE/TIMER_STRUCT_SIZE]={0};
static uint8_t stack_count=TIMER_STATCK_SIZE/TIMER_STRUCT_SIZE;

/* ��ʱ�������ͷָ�� */
static vkTIMER *timer_list = NULL;

/* ��ǰ��ʱ��Ticks */
static uint32_t timer_ticks = 0;

/* Ԥ�������� */
static void TimerCallbacks (void);
void* timer_mem_malloc(uint32_t size);
void  timer_mem_free(void *ptr);

/*******************************************************************************
 * ����: vkTimerInsert
 * ����: ����һ����ʱ��
 * �β�: ��ʱ��ָ��
 * ����: �ɹ�vkTIMER_OK��ʧ�ܸ���
 * ˵��: �� 
 ******************************************************************************/
int8_t vkTimerInsert (vkTIMER *timer_ptr)
{
    int8_t status;
    CRITICAL_STORE;

    /* Parameter check */
    if ((timer_ptr == NULL) || (timer_ptr->cb_func == NULL) || (timer_ptr->cb_ticks == 0))
    {
        /* Return error */
        status = vkTIMER_ERR_PARAM;
    }
    else
    {    
		/* Copy a new timer callback from timer_ptr */
		vkTIMER *new = (vkTIMER *)timer_mem_malloc(TIMER_STRUCT_SIZE);
		if(new == NULL)
		{
			return vkTIMER_ERR_PARAM;
		}
		
		/* Protect the list */
        CRITICAL_START ();
		
		new->timer_name = timer_ptr->timer_name;
		new->cb_func	= timer_ptr->cb_func;
		new->cb_data	= timer_ptr->cb_data;
		new->cb_ticks	= timer_ptr->cb_ticks;
		new->next_timer	= timer_ptr->next_timer;
			
        /*
         * Enqueue in the list of timers.
         *
         * The list is not ordered, all timers are inserted at the start
         * of the list. On each system tick increment the list is walked
         * and the remaining ticks count for that timer is decremented.
         * Once the remaining ticks reaches zero, the timer callback is
         * made.
         */
        if (timer_list == NULL)
        {
            /* List is empty, insert new head */
            new->next_timer = NULL;

			/* Update the list head with the new timer */
            timer_list = new;
        }
        else
        {
            /* List has at least one entry, enqueue new timer before */
            new->next_timer = timer_list;

			/* Update the list head with the new timer */
            timer_list = new;
        }

        /* End of list protection */
        CRITICAL_END ();

        /* Successful */
        status = vkTIMER_OK;
    }

    return (status);
}

/*******************************************************************************
 * ����: vkTimerCancel
 * ����: ɾ��һ����ʱ��
 * �β�: ��ʱ��ָ��
 * ����: �ɹ�vkTIMER_OK��ʧ�ܸ���
 * ˵��: �� 
 ******************************************************************************/
int8_t vkTimerCancel (vkTIMER *timer_ptr)
{
    int8_t status = vkTIMER_ERR_NOT_FOUND;
    vkTIMER *prev_ptr, *next_ptr;
    CRITICAL_STORE;

    /* Parameter check */
    if (timer_ptr == NULL)
    {
        /* Return error */
        status = vkTIMER_ERR_PARAM;
    }
    else
    {
        /* Protect the list */
        CRITICAL_START ();

        /* Walk the list to find the relevant timer */
        prev_ptr = next_ptr = timer_list;
        while (next_ptr)
        {
            /* Is this entry the one we're looking for? */
            if (next_ptr->timer_name == timer_ptr->timer_name)
            {
                if (next_ptr == timer_list)
                {
                    /* We're removing the list head */
                    timer_list = next_ptr->next_timer;

					/* Free the timer callback memory space*/
					timer_mem_free(next_ptr);

					/* Update prev_ptr and next_ptr with the list head */
					prev_ptr = next_ptr = timer_list;
                }
                else
                {
                    /* We're removing a mid or tail TCB */
                    prev_ptr->next_timer = next_ptr->next_timer;

					/* Free the timer callback memory space*/
					timer_mem_free(next_ptr);

					/* Update next_ptr with the next of prev_ptr */
					next_ptr = prev_ptr->next_timer;
                }
				
                /* Successful */
                status = vkTIMER_OK;               
            }
			else
			{
            	/* Move on to the next in the list */
           		prev_ptr = next_ptr;
           		next_ptr = next_ptr->next_timer;
			}
        }

        /* End of list protection */
        CRITICAL_END ();
     }

    return (status);
}

/*******************************************************************************
 * ����: vkTimerClear
 * ����: ɾ��������ʱ��
 * �β�: ��ʱ��ָ��
 * ����: �ɹ�vkTIMER_OK
 * ˵��: �� 
 ******************************************************************************/
int8_t vkTimerClear (void)
{
    vkTIMER *next_ptr;
    CRITICAL_STORE;

	/* Protect the list */
	CRITICAL_START ();

	/* Walk the list to find the relevant timer */
	next_ptr = timer_list;
	while (next_ptr)
	{
		/* We're removing the list head */
		timer_list = next_ptr->next_timer;
	
		/* Free the timer callback memory space*/
		timer_mem_free(next_ptr);	

		/* Update next_ptr with the list head */
		next_ptr = timer_list;
	}

	/* End of list protection */
	CRITICAL_END ();

	return vkTIMER_OK;	
}

/*******************************************************************************
 * ����: vkTimerGetTicks
 * ����: ��ȡ��ʱ��Ticks
 * �β�: 
 * ����: 
 * ˵��: �� 
 ******************************************************************************/
uint32_t vkTimerGetTicks(void)
{
    return (timer_ticks);
}

/*******************************************************************************
 * ����: vkTimerSetTicks
 * ����: ������ʱ��Ticks
 * �β�: 
 * ����: 
 * ˵��: �� 
 ******************************************************************************/
int8_t vkTimerSetTicks(uint32_t new_time)
{
    timer_ticks = new_time;

	return 0;
}

/*******************************************************************************
 * ����: vkTimerDelayMS
 * ����: ��ʱ����ʱ����
 * �β�: 
 * ����: 
 * ˵��: TIMER_US_PER_TICK������1ms,���ܾ�ȷ��ʱ������
 ******************************************************************************/
void vkTimerDelayMS(uint32_t msec)
{
	uint32_t tick_start = vkTimerGetTicks();

	while(vkTimerGetTicks() <= (tick_start + (uint32_t)(msec*1000)/TIMER_US_PER_TICK))

	return;
}

/*******************************************************************************
 * ����: vkTimerDelayS
 * ����: ��ʱ����ʱ��
 * �β�: 
 * ����: 
 * ˵��: �� 
 ******************************************************************************/
void vkTimerDelaySS(uint32_t s)
{
	uint32_t tick_start = vkTimerGetTicks();

	while(vkTimerGetTicks() <= (tick_start + (uint32_t)(s*1000*1000)/TIMER_US_PER_TICK));

	return;
}


/**
 * \b vkTimerTick
 *
 * ��ʱ��Tick�жϻص�����
 *
 * User ports are responsible for calling this routine once per system tick.
 *
 * On each system tick this routine is called to do the following:
 *  1. Increase the system tick count
 *  2. Call back to any registered timer callbacks
 *
 * @return None
 */
void vkTimerTick (void)
{
    /* Increment the system tick count */
    timer_ticks++;

    /* Check for any callbacks that are due */
    TimerCallbacks ();
}

/**
 * \b TimerCallbacks
 *
 * ��ʱ��Tick���ú���
 *
 * Find any callbacks that are due and call them up.
 *
 * @return None
 */
static void TimerCallbacks (void)
{
    vkTIMER *prev_ptr, *next_ptr;

    /*
     * Walk the list decrementing each timer's remaining ticks count and
     * looking for due callbacks.
     */
    prev_ptr = next_ptr = timer_list;
    while (next_ptr)
    {
        /* ��ʱʱ�䵽 Is this entry due? */
        if (--(next_ptr->cb_ticks) == 0)
        {
            /* �����׸���ʱ�� Remove the entry from the timer list */
            if (next_ptr == timer_list)
            {
                /* We're removing the list head */
                timer_list = next_ptr->next_timer;

				/* Call the registered callback */
				if (next_ptr->cb_func)
				{
					next_ptr->cb_func (next_ptr->cb_data);
				}

				/* Free the timer callback memory space*/
				timer_mem_free(next_ptr);	

				/* Update prev_ptr and next_ptr with the list head*/
				prev_ptr = next_ptr = timer_list;			
            }
            else
            {
                /* We're removing a mid or tail timer */
                prev_ptr->next_timer = next_ptr->next_timer;

				/* Call the registered callback */
				if (next_ptr->cb_func)
				{
					next_ptr->cb_func (next_ptr->cb_data);
				}

				/* Free the timer callback memory space*/
				timer_mem_free(next_ptr);	

				/* Update next_ptr with the next of prev_ptr */
				next_ptr = prev_ptr->next_timer;
            }	
        }
        /* Entry is not due, leave it in there with its count decremented */
        else
        {
            /*
             * Update prev_ptr to this entry. We will need it if we want
             * to remove a mid or tail timer.
             */
            prev_ptr = next_ptr;

			/* Move on to the next in the list */
        	next_ptr = next_ptr->next_timer;
        }
    }
}

/**
 * \b timer_mem_malloc
 *
 * ��ʱ���ڴ�ռ�����
 *
 * malloc memory
 *
 * @return menory pointer
 */
void* timer_mem_malloc(uint32_t size)
{	
	int i;
	uint8_t *ptr = NULL;

	/* ���ݿ��С���� */
	if(size != TIMER_STRUCT_SIZE)
	{
		return NULL;
	}	
	
	/* û�п������ݿ� */
	if(stack_count==0)
	{
		return NULL;
	}

	/* ���������ݿ� */
	for(i=0; i<TIMER_STATCK_SIZE/TIMER_STRUCT_SIZE; i++)
	{
		if(STACK_FLAGS[i] == 0)
		{		
			break;
		}
	}	

	STACK_FLAGS[i] = 1;
	stack_count--;
	
	ptr = STACK_TIMER+i*TIMER_STRUCT_SIZE;

	return ptr;	
}

/**
 * \b timer_mem_free
 *
 * ��ʱ���ڴ�ռ��ͷ�
 *
 * malloc memory
 *
 * @return none
 */
void timer_mem_free(void *ptr)
{
	/* ָ���ǿ�     */
	if(ptr == NULL)
	{
		return;
	}

	/* ָ��û�ڶ�ʱ����ջ��Χ�� */
	if(ptr < STACK_TIMER || ptr > STACK_TIMER+TIMER_STATCK_SIZE)
	{
		return;
	}

	/* ���ݴ�С�����ݿ������� */
	if(((uint8_t *)ptr-STACK_TIMER)%TIMER_STRUCT_SIZE != 0)
	{
		return;
	}

	STACK_FLAGS[((uint8_t *)ptr-STACK_TIMER)/TIMER_STRUCT_SIZE] = 0;
	stack_count++;		
	memset(ptr, 0, TIMER_STRUCT_SIZE);

	return;
}


