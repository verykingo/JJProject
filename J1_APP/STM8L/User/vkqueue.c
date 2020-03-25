/******************************************************************************
 * 文件  ：vkqueue.c
 * 描述    ：循环数组队列
 * 平台    ：ALL
 * 时间  ：2019-04-01

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vkqueue.h"

/* Forward declarations */
inline int8_t queue_remove (vkQUEUE *qptr, uint8_t* msgptr);
inline int8_t queue_insert (vkQUEUE *qptr, uint8_t* msgptr);

/*******************************************************************************
 * 名称: vkQueueCreate
 * 功能: 创建循环数组队列
 * 形参: qptr 队列指针
         buff_ptr 队列存储空间首地址
         unit_size 队列单元大小,最大值255
         max_num_msgs 队列大小,最大值255
 * 返回: 成功0，失败负数
 * 说明: 无 
 ******************************************************************************/
int8_t vkQueueCreate (vkQUEUE *qptr, uint8_t *buff_ptr, uint8_t unit_size, uint8_t max_num_msgs)
{
    int8_t status = vkQUEUE_OK;

    /* Parameter check */
    if ((qptr == NULL) || (buff_ptr == NULL))
    {
        /* Bad pointers */
        status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }
    else if ((unit_size == 0) || (max_num_msgs == 0))
    {
        /* Bad values */
        status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }	
   	else
    {
       /* Store the queue details */
        qptr->buff_ptr = buff_ptr;
        qptr->unit_size = unit_size;
        qptr->max_num_msgs = max_num_msgs;

        /* Initialise the insert/remove pointers */
        qptr->insert_index = 0;
        qptr->remove_index = 0;
        qptr->num_msgs_stored = 0;

		/* Successful */
        status = vkQUEUE_OK;
    }

    return (status);
}

/*******************************************************************************
 * 名称: vkQueueDestory
 * 功能: 销毁循环数组队列
 * 形参: qptr 队列指针
 * 返回: 成功0，失败负数
 * 说明: 无 
 ******************************************************************************/
int8_t vkQueueDestory(vkQUEUE *qptr)
{    
    CRITICAL_STORE;
	int8_t status = vkQUEUE_OK;

	/* Parameter check */
	if (qptr == NULL)
	{
		/* Bad pointer */
		status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
	}
	else
	{
		/* Enter critical region */
		CRITICAL_START ();

		/* Set the queue details to NULL/0 */
        qptr->buff_ptr = NULL;
        qptr->unit_size = 0;
        qptr->max_num_msgs = 0;

        /* Initialise the insert/remove pointers */
        qptr->insert_index = 0;
        qptr->remove_index = 0;
        qptr->num_msgs_stored = 0;

        /* Successful */
        status = vkQUEUE_OK;	

		/* Exit critical region */
		CRITICAL_END ();
	}

	 return (status);
}

/*******************************************************************************
 * 名称: vkQueueGet
 * 功能: 获取一个队列单元
 * 形参: qptr 队列指针
 		 timeout 超时时间
 		 msgptr 队列单元指针
 * 返回: 成功0，失败负数
 * 说明: 无 
 ******************************************************************************/
int8_t vkQueueGet (vkQUEUE *qptr, uint32_t timeout, uint8_t *msgptr)
{
    CRITICAL_STORE;
    int8_t status = vkQUEUE_OK;

    /* Check parameters */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }
    else
    {
    	if(timeout)	//超时时间非0
    	{
    		/* 等待队列有数据或超时时间到 */
		    while((!qptr->num_msgs_stored) && (--timeout))
			{nop(); /* 防止被编译优化掉 */}
			
			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();
			
			if(timeout == 0)				//超时时间到
			{				
				status = vkQUEUE_TIMEOUT;	//返回超时
			}
			else				
			{
				/* There is data to copy from the queue */
				status = queue_remove (qptr, msgptr);
			}	

			/* Exit critical region */
			CRITICAL_END ();
    	}
		else	//立即取数据
		{
	        /* Protect access to the queue object and OS queues */
	        CRITICAL_START ();

			/* If queue is empty, block the calling thread */
	        if (qptr->num_msgs_stored == 0)
	        {
				status = vkQUEUE_WOULDBLOCK;	//返回队列空白          
	        }
			else
			{
				/* there is data to copy from the queue */
	            status = queue_remove (qptr, msgptr);	
			}

			/* Exit critical region */
			CRITICAL_END ();
		}	
    }

    return (status);
}

/*******************************************************************************
 * 名称: vkQueuePut
 * 功能: 插入一个队列单元
 * 形参: qptr 队列指针
 		 timeout 超时时间
 		 msgptr 队列单元指针
 * 返回: 成功0，失败负数
 * 说明: 无 
 ******************************************************************************/
int8_t vkQueuePut(vkQUEUE *qptr, uint32_t timeout, uint8_t *msgptr)
{
	CRITICAL_STORE;
	int8_t status = vkQUEUE_OK;

	/* Check parameters */
	if ((qptr == NULL) || (msgptr == NULL))
	{
		/* Bad pointer */
		status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
	}
	else
	{
		if(timeout)	//超时时间非0
		{
			/* 等待队列有空间或超时时间到 */
			while((qptr->num_msgs_stored == qptr->max_num_msgs) && (--timeout))
			{nop(); /* 防止被编译优化掉 */}

			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();
			
			if(timeout == 0)			//超时时间到
		 	{				
				status = vkQUEUE_TIMEOUT;	//返回超时		 		
		 	}
			else
			{
				/* There is space to copy into the queue */
				status = queue_insert (qptr, msgptr);
			}

			/* Exit critical region */
			CRITICAL_END ();			
		}
		else	//立即送数据
		{
			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();

			/* If queue is full, block the calling thread */
			if (qptr->num_msgs_stored == qptr->max_num_msgs)
			{				
				status = vkQUEUE_WOULDBLOCK;		//返回队列已满
			}
			else
			{
				/* there is space to copy into the queue */
				status = queue_insert (qptr, msgptr);
			}

			/* Exit critical region */
			CRITICAL_END ();
		}
	}

	return (status);
}


/*******************************************************************************
 * 名称: vkQueueClear
 * 功能: 清空队列
 * 形参: qptr 队列指针
 * 返回: 成功0，失败负数
 * 说明: 无 
 ******************************************************************************/
int8_t vkQueueClear (vkQUEUE *qptr)
{	 
	CRITICAL_STORE;
	int8_t status = vkQUEUE_OK;

	/* Parameter check */
	if (qptr == NULL)
	{
		/* Bad pointer */
		status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
	}
	else
	{
		/* Enter critical region */
		CRITICAL_START ();

		/* Set the queue details to 0 */
		memset(qptr->buff_ptr, 0, qptr->unit_size*qptr->max_num_msgs);

		/* Initialise the insert/remove pointers */
		qptr->insert_index = 0;
		qptr->remove_index = 0;
		qptr->num_msgs_stored = 0;

		/* Successful */
		status = vkQUEUE_OK;		

		/* Exit critical region */
		CRITICAL_END ();
	}

	 return (status);
}


/*******************************************************************************
 * 名称: vkQueueEmpty
 * 功能: 判断队列是否是空
 * 形参: qptr 队列指针
 * 返回: 成功0，失败负数
 * 说明: 无 
 ******************************************************************************/
int8_t vkQueueEmpty (vkQUEUE *qptr)
{
	if(qptr->num_msgs_stored == 0)
	{
		return vkQUEUE_NULL;
	}
	else
	{
		return vkQUEUE_BEING;
	}
}

/**
 *  queue_remove 将数据移出队列
 */
inline int8_t queue_remove (vkQUEUE *qptr, uint8_t* msgptr)
{
    int8_t status = vkQUEUE_OK;
	
    /* 参数检测 */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }
    else
    {
        /* 拷贝数据 */
        memcpy(msgptr, (qptr->buff_ptr + qptr->remove_index*qptr->unit_size), qptr->unit_size);
        qptr->remove_index = (qptr->remove_index+1)%(qptr->max_num_msgs);
        qptr->num_msgs_stored--;

		/* Successful */
        status = vkQUEUE_OK;
    }

    return (status);
}


/**
 *  queue_insert 将数据插入队列 
 */
inline int8_t queue_insert (vkQUEUE *qptr, uint8_t* msgptr)
{
    int8_t status = vkQUEUE_OK;

    /* Check parameters */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }
    else
    {
        /* There is space in the queue, copy it in */
        memcpy ((qptr->buff_ptr + qptr->insert_index*qptr->unit_size), msgptr, qptr->unit_size);
        qptr->insert_index = (qptr->insert_index+1)%(qptr->max_num_msgs);
        qptr->num_msgs_stored++;

		/* Successful */
		status = vkQUEUE_OK;
    }

    return (status);
}
