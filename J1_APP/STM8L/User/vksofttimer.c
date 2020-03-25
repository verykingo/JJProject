/******************************************************************************
 * �ļ�  ��vksofttimer.c
 * ����    ����ʱ���������β���ͬһ����ʱ����һ����ɾ��������������ͬ�Ķ����ʱ��
           ���Ҫȷ����ʱ��������ֻ��һ���ض����ֵĶ�ʱ����������ڲ���֮ǰ����ɾ��
           �ö�ʱ������ʱ��������ϵͳӲ��ʱ��tick
 * ƽ̨    ��ALL
 * ʱ��  ��2019-04-01

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vksofttimer.h"

/* Local data */
static uint8_t STACK_TIMER[TIMER_STATCK_SIZE]={0};
static uint8_t STACK_FLAGS[TIMER_STRUCT_NUMS]={0};
static uint8_t stack_count=TIMER_STRUCT_NUMS;
static uint8_t stack_index=0;

/* ��ʱ�������ͷָ�� */
static vkTIMER *timer_list = NULL;

/* ��ʱ�������ͷָ�� */
static vkTIMER *while_list = NULL;

#if USING_TIMER_TICKS==1u
/* ��ǰ��ʱ��Ticks */
static uint32_t timer_ticks = 0;
#endif

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
		printf("%s param error!!!, timer_ptr=0x%X, cb_func=0x%X, cb_ticks=%d\r\n", __FUNCTION__,timer_ptr,(void*)timer_ptr->cb_func,(int)timer_ptr->cb_ticks);
    }
    else
    {
		/* Create a new timer callback from timer_ptr */
		vkTIMER *new = (vkTIMER *)timer_mem_malloc(TIMER_STRUCT_SIZE);
		if(new == NULL)
		{			
			return vkTIMER_ERR_PARAM;
		}
		memcpy(new, timer_ptr, TIMER_STRUCT_SIZE);
		new->cb_mode	= CB_RUNING_MODE_ONCE;
		new->prev_timer = NULL;
		new->next_timer = NULL;

		
    	/* Protect the list */
        CRITICAL_START ();
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
			/* Update the list head with the new timer */
            timer_list = new;
        }
        else
        {
            /* List has at least one entry, enqueue new timer before */
			new->next_timer = timer_list;
			timer_list->prev_timer = new;          

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
    vkTIMER *next_ptr, *temp_ptr;
	
    CRITICAL_STORE;

    /* Parameter check */
    if (timer_ptr == NULL)
    {
        /* Return error */
        status = vkTIMER_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }
    else
    {
        /* Protect the list */
        CRITICAL_START ();

        /* Walk the list to find the relevant timer */
        next_ptr = timer_list;
        while (next_ptr)
        {
            /* Is this entry the one we're looking for? */
            if (next_ptr->timer_name == timer_ptr->timer_name)
            {
                if (next_ptr == timer_list)
                {
                    /* We're removing the list head */
                    timer_list = next_ptr->next_timer;
					timer_list->prev_timer = NULL;

					/* Free the timer callback memory space*/
					timer_mem_free(next_ptr);
					
					/* Update next_ptr with the list head */
					next_ptr = timer_list;
                }
                else
                {
                    /* We're removing a mid or tail timer callback */
                    next_ptr->prev_timer->next_timer = next_ptr->next_timer;
					next_ptr->next_timer->prev_timer = next_ptr->prev_timer;

					/* Copy a temp ptr for free */
					temp_ptr = next_ptr;
					
					/* Update next_ptr with the next of prev_ptr */
					next_ptr = next_ptr->next_timer;
							
					/* Free the timer callback memory space*/
					timer_mem_free(temp_ptr);
                }
				
                /* Successful */
                status = vkTIMER_OK;               
            }
			else
			{
            	/* Move on to the next in the list */
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
 * ����: vkTimerRemain
 * ����: ʣ�����ʱ��
 * �β�: ��ʱ��ָ��
 * ����: �ɹ�ʣ��Tick��
 * ˵��: �� 
 ******************************************************************************/
extern uint32_t	vkTimerRemain(vkTIMER *timer_ptr)
{
	uint32_t remain_ticks = 0;
	
	vkTIMER *next_ptr;
	
	CRITICAL_STORE;

	/* Parameter check */
	if (timer_ptr == NULL)
	{
		printf("%s param error.\r\n", __FUNCTION__);
	}
	else
	{
		/* Protect the list */
		CRITICAL_START ();

		/* Walk the list to find the relevant timer */
		next_ptr = timer_list;
		while (next_ptr)
		{
			/* Is this entry the one we're looking for? */
			if (next_ptr->timer_name == timer_ptr->timer_name)
			{				
				/* Successful */
				remain_ticks = next_ptr->cb_ticks;
				
				break;
			}
			else
			{
				/* Move on to the next in the list */
				next_ptr = next_ptr->next_timer;
			}
		}	

		/* End of list protection */
		CRITICAL_END ();
	}

	return (remain_ticks);
}

uint32_t vkTimerTaskCreate(void (*task)(void *p_arg),void *p_arg,uint8_t type,uint32_t slice )
{	
	CRITICAL_STORE;
	
	if(task==NULL||(type!=CB_FUNC_TYPE_TIMER&&type!=CB_FUNC_TYPE_WHILE)||slice==0)
	{		
		printf("%s param error!!!, task=0x%X, type=%d, slice=%d\r\n", __FUNCTION__,(void *)task,(int)type,(int)slice);
		return 0;
	}

	/* Create a new timer callback from timer_ptr */
	vkTIMER *new = (vkTIMER *)timer_mem_malloc(TIMER_STRUCT_SIZE);
	if(new == NULL)
	{			
		return 0;
	}
	new->timer_name	= (void *)task;
	new->cb_type	= type;	
	new->cb_mode	= CB_RUNING_MODE_KEEP;
	new->cb_func	= task;
	new->cb_ticks	= vkMS_TO_TICKS(slice);
	new->prev_timer = NULL;
	new->next_timer = NULL;
	
	/* Protect the list */
    CRITICAL_START ();
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
		/* Update the list head with the new timer */
        timer_list = new;
    }
    else
    {
        /* List has at least one entry, enqueue new timer before */
		new->next_timer = timer_list;
		timer_list->prev_timer = new;          

		/* Update the list head with the new timer */
        timer_list = new;
    }

    /* End of list protection */
    CRITICAL_END ();

    /* Successful */
	return (uint32_t)task;
}

void vkTimerTaskDestory(uint32_t handle)
{
	vkTIMER *next_ptr, *temp_ptr;
	
	CRITICAL_STORE;

	/* Parameter check */
	if (handle == 0)
	{
		printf("%s handle error.\r\n", __FUNCTION__);
	}
	else
	{
		/* Protect the list */
		CRITICAL_START ();

		/* Walk the list to find the relevant timer */
		next_ptr = timer_list;
		while (next_ptr)
		{
			/* Is this entry the one we're looking for? */
			if ((uint32_t)(next_ptr->timer_name) == handle)
			{
				if (next_ptr == timer_list)
				{
					/* We're removing the list head */
					timer_list = next_ptr->next_timer;
					timer_list->prev_timer = NULL;

					/* Free the timer callback memory space*/
					timer_mem_free(next_ptr);
					
					/* Update next_ptr with the list head */
					next_ptr = timer_list;
				}
				else
				{
					/* We're removing a mid or tail timer callback */
					next_ptr->prev_timer->next_timer = next_ptr->next_timer;
					next_ptr->next_timer->prev_timer = next_ptr->prev_timer;

					/* Copy a temp ptr for free */
					temp_ptr = next_ptr;
					
					/* Update next_ptr with the next of prev_ptr */
					next_ptr = next_ptr->next_timer;
							
					/* Free the timer callback memory space*/
					timer_mem_free(temp_ptr);
				}		   
			}
			else
			{
				/* Move on to the next in the list */
				next_ptr = next_ptr->next_timer;
			}
		}

		/* End of list protection */
		CRITICAL_END ();
	 }
}

#if USING_TIMER_TICKS==1u
/*******************************************************************************
 * ����: vkTimerGetTicks
 * ����: ��ȡ��ʱ��Ticks
 * �β�: 
 * ����: 
 * ˵��: �� 
 ******************************************************************************/
int8_t vkTimerGetTicks(uint32_t *new_time)
{
	*new_time = timer_ticks;

	return 0;
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
 * ˵��: �����ٶ�ʱ���ص�����������ã�TIMER_US_PER_TICK������1ms,���ܾ�ȷ��ʱ������
 ******************************************************************************/
void vkTimerDelayMS(uint32_t msec)
{
	uint32_t tick_start=0;
	uint32_t tick_now=0;
	vkTimerGetTicks(&tick_start);
	vkTimerGetTicks(&tick_now);

	while(tick_now <= (tick_start + (uint32_t)(msec*1000)/TIMER_US_PER_TICK))
	{
		vkTimerGetTicks(&tick_now);
	}

	return;
}

/*******************************************************************************
 * ����: vkTimerDelayS
 * ����: ��ʱ����ʱ��
 * �β�: 
 * ����: 
 * ˵��: �����ٶ�ʱ���ص������������ 
 ******************************************************************************/
void vkTimerDelaySS(uint32_t s)
{
	uint32_t tick_start=0;
	uint32_t tick_now=0;
	vkTimerGetTicks(&tick_start);
	vkTimerGetTicks(&tick_now);

	while( tick_now <= (tick_start + (uint32_t)(s*1000*1000)/TIMER_US_PER_TICK))
	{
		vkTimerGetTicks(&tick_now);
	}

	return;
}
#endif

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
#if USING_TIMER_TICKS==1u
    /* Increment the system tick count */
    timer_ticks++;
#endif

    /* Check for any callbacks that are due */
    TimerCallbacks ();
}

/**
 * \b vkTimerDone
 *
 * ��������while��ִ�лص�����
 * ��ʱ���豸��һ����ʱ��
 *
 * @return None
 */
void vkTimerDone (void)
{
    vkTIMER *next_ptr;
	vkTIMER expire;
    CRITICAL_STORE;

	/* Protect the list */
	CRITICAL_START ();

	/* Walk the list to find the relevant timer */
	next_ptr = while_list;
	while (next_ptr)
	{
		/* We're removing the list head */
		while_list = next_ptr->next_timer;

		/* Copy timer for callback */
		memcpy(&expire, next_ptr, TIMER_STRUCT_SIZE);

		/* Free the timer callback memory space*/
		timer_mem_free(next_ptr);

		/* Update next_ptr with the list head */
		next_ptr = while_list;		

		/* Call the registered callback */
		if (expire.cb_func)
		{
			expire.cb_func (expire.cb_data);
		}
	}

	/* End of list protection */
	CRITICAL_END ();
	
	return ;	
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
    vkTIMER *next_ptr, *temp_ptr;
	vkTIMER expire;
		
    /*
     * Walk the list decrementing each timer's remaining ticks count and
     * looking for due callbacks.
     */
    next_ptr = timer_list;
    while (next_ptr && next_ptr->cb_ticks!=0)
    {
        /* ��ʱʱ�䵽 Is this entry due? */
        if (--(next_ptr->cb_ticks) == 0)
        {
            /* �����׸���ʱ�� Remove the entry from the timer list */
            if (next_ptr == timer_list)
            {
                /* We're removing the list head */
                timer_list = next_ptr->next_timer;
				timer_list->prev_timer = NULL;

				if(next_ptr->cb_type == CB_FUNC_TYPE_TIMER)
				{
					/* Copy timer for callback */
					memcpy(&expire, next_ptr, TIMER_STRUCT_SIZE);

					/* Free the timer callback memory space*/
					timer_mem_free(next_ptr);	
	                
	                /* Update next_ptr with the list head*/
					next_ptr = timer_list;
	                 
					/* Call the registered callback */
					if (expire.cb_func)
					{
						expire.cb_func (expire.cb_data);
					}
				}
				else if(next_ptr->cb_type == CB_FUNC_TYPE_WHILE)
				{
					/* Copy temp ptr for free */
					temp_ptr = next_ptr;

					/* Update next_ptr with the list head*/
					next_ptr = timer_list;
					
					/* ����while_list���� */
					if(while_list == NULL)
					{
						while_list = temp_ptr;
					}
					else
					{	
						/* ��������ͷ */
						temp_ptr->next_timer = while_list;
						while_list->prev_timer = NULL;
						while_list = temp_ptr;
					}
				}
            }
            else
            {
                /* We're removing a mid or tail timer callback */
                next_ptr->prev_timer->next_timer = next_ptr->next_timer;
				next_ptr->next_timer->prev_timer = next_ptr->prev_timer;

				if(next_ptr->cb_type == CB_FUNC_TYPE_TIMER)
				{
					/* Copy timer for callback */
					memcpy(&expire, next_ptr, TIMER_STRUCT_SIZE);

					/* Copy temp ptr for free */
					temp_ptr = next_ptr;

					/* Update next_ptr with the next of next_timer */
					next_ptr = next_ptr->next_timer;
					
					/* Free the timer callback memory space*/
					timer_mem_free(temp_ptr);
		
					/* Call the registered callback */
					if (expire.cb_func)
					{
						expire.cb_func (expire.cb_data);
					}
				}
				else if(next_ptr->cb_type == CB_FUNC_TYPE_WHILE)
				{
					/* Copy temp ptr for free */
					temp_ptr = next_ptr;

					/* Update next_ptr with the next of next_timer */
					next_ptr = next_ptr->next_timer;
					
					/* ����while_list���� */
					if(while_list == NULL)
					{
						while_list = temp_ptr;
					}
					else
					{	
						/* ��������ͷ */
						temp_ptr->next_timer = while_list;
						while_list->prev_timer = NULL;
						while_list = temp_ptr;
					}
				}
            }	
        }
        /* Entry is not due, leave it in there with its count decremented */
        else
        {
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
static inline void* timer_mem_malloc(uint32_t size)
{	
	uint8_t i;
	uint8_t *ptr = NULL;

	/* ���ݿ��С���� */
	if(size != TIMER_STRUCT_SIZE)
	{
		printf("%s size error.\r\n", __FUNCTION__);
		return NULL;		
	}	
	
	/* û�п������ݿ� */
	if(stack_count==0)
	{
		printf("%s no memony.\r\n", __FUNCTION__);
		return NULL;
	}

	CRITICAL_STORE;

	/* Protect the list */
	CRITICAL_START ();
	
#if 1
	/* ���������ݿ� */
	for(i=stack_index; i<TIMER_STRUCT_NUMS; i++)
	{
		if(STACK_FLAGS[i] == 0)
		{		
			goto FIND_MEMORY;
		}
	}	
	for(i=0; i<stack_index; i++)
	{
		if(STACK_FLAGS[i] == 0)
		{			
			goto FIND_MEMORY;
		}
	}

FIND_MEMORY:
	STACK_FLAGS[i] = 1;
	stack_count--;
	stack_index = (i+1)%(TIMER_STRUCT_NUMS);
	
	ptr = STACK_TIMER+i*TIMER_STRUCT_SIZE;

#else

	/* ���������ݿ� */
	for(i=0; i<TIMER_STRUCT_NUMS; i++)
	{
		if(STACK_FLAGS[i] == 0)
		{	
			STACK_FLAGS[i] = 1;
			stack_count--;
			ptr = STACK_TIMER+i*TIMER_STRUCT_SIZE;	
			
			break;
		}
	}
#endif

	/* End of list protection */
	CRITICAL_END();
	
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
static inline void timer_mem_free(void *ptr)
{
	/* ָ���ǿ�     */
	if(ptr == NULL)
	{
		printf("%s is null ptr.\r\n", __FUNCTION__);
		return;
	}

	/* ָ��û�ڶ�ʱ����ջ��Χ�� */
	if(ptr < STACK_TIMER || ptr > STACK_TIMER+TIMER_STATCK_SIZE)
	{
		printf("%s is out of memony.\r\n", __FUNCTION__);
		return;
	}

	/* ���ݴ�С�����ݿ������� */
	if(((uint8_t *)ptr-STACK_TIMER)%TIMER_STRUCT_SIZE != 0)
	{
		printf("%s size error.\r\n", __FUNCTION__);
		return;
	}

	CRITICAL_STORE;

	/* Protect the list */
	CRITICAL_START ();

	STACK_FLAGS[((uint8_t *)ptr-STACK_TIMER)/TIMER_STRUCT_SIZE] = 0;
	stack_count++;		
	memset(ptr, 0, TIMER_STRUCT_SIZE);

	/* End of list protection */
	CRITICAL_END();

	return;
}


