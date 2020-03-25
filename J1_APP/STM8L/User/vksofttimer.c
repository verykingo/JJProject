/******************************************************************************
 * 文件  ：vksofttimer.c
 * 描述    ：软定时器，允许多次插入同一个定时器，一次性删除队列中名字相同的多个定时器
           如果要确保定时器链表中只有一个特定名字的定时器，则必须在插入之前，先删除
           该定时器。软定时器依赖于系统硬定时器tick
 * 平台    ：ALL
 * 时间  ：2019-04-01

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

/* 定时器链表表头指针 */
static vkTIMER *timer_list = NULL;

/* 定时器链表表头指针 */
static vkTIMER *while_list = NULL;

#if USING_TIMER_TICKS==1u
/* 当前定时器Ticks */
static uint32_t timer_ticks = 0;
#endif

/* 预函数声明 */
static void TimerCallbacks (void);
void* timer_mem_malloc(uint32_t size);
void  timer_mem_free(void *ptr);

/*******************************************************************************
 * 名称: vkTimerInsert
 * 功能: 插入一个软定时器
 * 形参: 软定时器指针
 * 返回: 成功vkTIMER_OK，失败负数
 * 说明: 无 
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
 * 名称: vkTimerCancel
 * 功能: 删除一个软定时器
 * 形参: 软定时器指针
 * 返回: 成功vkTIMER_OK，失败负数
 * 说明: 无 
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
 * 名称: vkTimerClear
 * 功能: 删除所有软定时器
 * 形参: 软定时器指针
 * 返回: 成功vkTIMER_OK
 * 说明: 无 
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
 * 名称: vkTimerRemain
 * 功能: 剩余多少时间
 * 形参: 软定时器指针
 * 返回: 成功剩余Tick数
 * 说明: 无 
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
 * 名称: vkTimerGetTicks
 * 功能: 获取软定时器Ticks
 * 形参: 
 * 返回: 
 * 说明: 无 
 ******************************************************************************/
int8_t vkTimerGetTicks(uint32_t *new_time)
{
	*new_time = timer_ticks;

	return 0;
}

/*******************************************************************************
 * 名称: vkTimerSetTicks
 * 功能: 设置软定时器Ticks
 * 形参: 
 * 返回: 
 * 说明: 无 
 ******************************************************************************/
int8_t vkTimerSetTicks(uint32_t new_time)
{
    timer_ticks = new_time;

	return 0;
}

/*******************************************************************************
 * 名称: vkTimerDelayMS
 * 功能: 定时器延时毫秒
 * 形参: 
 * 返回: 
 * 说明: 不能再定时器回调函数里面调用，TIMER_US_PER_TICK不大于1ms,才能精确延时到毫秒
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
 * 名称: vkTimerDelayS
 * 功能: 定时器延时秒
 * 形参: 
 * 返回: 
 * 说明: 不能再定时器回调函数里面调用 
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
 * 定时器Tick中断回调函数
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
 * 在主函数while中执行回调函数
 * 此时无需备份一个定时器
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
 * 定时器Tick调用函数
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
        /* 定时时间到 Is this entry due? */
        if (--(next_ptr->cb_ticks) == 0)
        {
            /* 链表首个定时器 Remove the entry from the timer list */
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
					
					/* 插入while_list队列 */
					if(while_list == NULL)
					{
						while_list = temp_ptr;
					}
					else
					{	
						/* 插入链表头 */
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
					
					/* 插入while_list队列 */
					if(while_list == NULL)
					{
						while_list = temp_ptr;
					}
					else
					{	
						/* 插入链表头 */
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
 * 定时器内存空间申请
 *
 * malloc memory
 *
 * @return menory pointer
 */
static inline void* timer_mem_malloc(uint32_t size)
{	
	uint8_t i;
	uint8_t *ptr = NULL;

	/* 数据块大小错误 */
	if(size != TIMER_STRUCT_SIZE)
	{
		printf("%s size error.\r\n", __FUNCTION__);
		return NULL;		
	}	
	
	/* 没有可用数据块 */
	if(stack_count==0)
	{
		printf("%s no memony.\r\n", __FUNCTION__);
		return NULL;
	}

	CRITICAL_STORE;

	/* Protect the list */
	CRITICAL_START ();
	
#if 1
	/* 查表可用数据块 */
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

	/* 查表可用数据块 */
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
 * 定时器内存空间释放
 *
 * malloc memory
 *
 * @return none
 */
static inline void timer_mem_free(void *ptr)
{
	/* 指针是空     */
	if(ptr == NULL)
	{
		printf("%s is null ptr.\r\n", __FUNCTION__);
		return;
	}

	/* 指针没在定时器堆栈范围内 */
	if(ptr < STACK_TIMER || ptr > STACK_TIMER+TIMER_STATCK_SIZE)
	{
		printf("%s is out of memony.\r\n", __FUNCTION__);
		return;
	}

	/* 数据大小是数据块整数倍 */
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


