/******************************************************************************
 * 文件  ：vksofttimer.c
 * 描述    ：软定时器，允许多次插入同一个定时器，一次性删除队列中名字相同的多个定时器
           如果要确保定时器链表中只有一个特定名字的定时器，则必须在插入之前，先删除
           该定时器。
 * 平台    ：ALL
 * 时间  ：2019-04-01

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vksofttimer.h"

/* Local data */
static uint8_t STACK_TIMER[TIMER_STATCK_SIZE]={0};
static uint8_t STACK_FLAGS[TIMER_STATCK_SIZE/TIMER_STRUCT_SIZE]={0};
static uint8_t stack_count=TIMER_STATCK_SIZE/TIMER_STRUCT_SIZE;

/* 定时器链表表头指针 */
static vkTIMER *timer_list = NULL;

/* 当前定时器Ticks */
static uint32_t timer_ticks = 0;

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
 * 名称: vkTimerCancel
 * 功能: 删除一个软定时器
 * 形参: 软定时器指针
 * 返回: 成功vkTIMER_OK，失败负数
 * 说明: 无 
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
 * 名称: vkTimerGetTicks
 * 功能: 获取软定时器Ticks
 * 形参: 
 * 返回: 
 * 说明: 无 
 ******************************************************************************/
uint32_t vkTimerGetTicks(void)
{
    return (timer_ticks);
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
 * 说明: TIMER_US_PER_TICK不大于1ms,才能精确延时到毫秒
 ******************************************************************************/
void vkTimerDelayMS(uint32_t msec)
{
	uint32_t tick_start = vkTimerGetTicks();

	while(vkTimerGetTicks() <= (tick_start + (uint32_t)(msec*1000)/TIMER_US_PER_TICK))

	return;
}

/*******************************************************************************
 * 名称: vkTimerDelayS
 * 功能: 定时器延时秒
 * 形参: 
 * 返回: 
 * 说明: 无 
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
    /* Increment the system tick count */
    timer_ticks++;

    /* Check for any callbacks that are due */
    TimerCallbacks ();
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
    vkTIMER *prev_ptr, *next_ptr;

    /*
     * Walk the list decrementing each timer's remaining ticks count and
     * looking for due callbacks.
     */
    prev_ptr = next_ptr = timer_list;
    while (next_ptr)
    {
        /* 定时时间到 Is this entry due? */
        if (--(next_ptr->cb_ticks) == 0)
        {
            /* 链表首个定时器 Remove the entry from the timer list */
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
 * 定时器内存空间申请
 *
 * malloc memory
 *
 * @return menory pointer
 */
void* timer_mem_malloc(uint32_t size)
{	
	int i;
	uint8_t *ptr = NULL;

	/* 数据块大小错误 */
	if(size != TIMER_STRUCT_SIZE)
	{
		return NULL;
	}	
	
	/* 没有可用数据块 */
	if(stack_count==0)
	{
		return NULL;
	}

	/* 查表可用数据块 */
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
 * 定时器内存空间释放
 *
 * malloc memory
 *
 * @return none
 */
void timer_mem_free(void *ptr)
{
	/* 指针是空     */
	if(ptr == NULL)
	{
		return;
	}

	/* 指针没在定时器堆栈范围内 */
	if(ptr < STACK_TIMER || ptr > STACK_TIMER+TIMER_STATCK_SIZE)
	{
		return;
	}

	/* 数据大小是数据块整数倍 */
	if(((uint8_t *)ptr-STACK_TIMER)%TIMER_STRUCT_SIZE != 0)
	{
		return;
	}

	STACK_FLAGS[((uint8_t *)ptr-STACK_TIMER)/TIMER_STRUCT_SIZE] = 0;
	stack_count++;		
	memset(ptr, 0, TIMER_STRUCT_SIZE);

	return;
}


