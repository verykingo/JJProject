/*
 * ѭ���������
 */
 
#include <stdlib.h>
#include <string.h>
#include "vkqueue.h"

/* Forward declarations */
static int8_t queue_remove (vkQUEUE *qptr, uint8_t* msgptr);
static int8_t queue_insert (vkQUEUE *qptr, uint8_t* msgptr);

/**
 * QueueCreate
 * ��������
 */
int8_t vkQueueCreate (vkQUEUE *qptr, uint8_t *buff_ptr, uint32_t unit_size, uint32_t max_num_msgs)
{
    int8_t status = vkQUEUE_OK;

    /* Parameter check */
    if ((qptr == NULL) || (buff_ptr == NULL))
    {
        /* Bad pointers */
        status = vkQUEUE_ERR_PARAM;
    }
    else if ((unit_size == 0) || (max_num_msgs == 0))
    {
        /* Bad values */
        status = vkQUEUE_ERR_PARAM;
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

/**
 * QueueDelete
 * ɾ������
 */
int8_t QueueDestory(vkQUEUE *qptr)
{    
    CRITICAL_STORE;
	int8_t status = vkQUEUE_OK;

	/* Parameter check */
	if (qptr == NULL)
	{
		/* Bad pointer */
		status = vkQUEUE_ERR_PARAM;
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

/**
 *  QueueGet
 *	�Ӷ�����ȡ����
 */
int8_t vkQueueGet (vkQUEUE *qptr, uint32_t timeout, uint8_t *msgptr)
{
    CRITICAL_STORE;
    int8_t status = vkQUEUE_OK;

    /* Check parameters */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;
    }
    else
    {
    	if(timeout)	//��ʱʱ���0
    	{
    		/* �ȴ����������ݻ�ʱʱ�䵽 */
		    while((!qptr->num_msgs_stored) && (--timeout));
			
			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();
			
			if(timeout == 0)			//��ʱʱ�䵽
			{				
				status = vkQUEUE_TIMEOUT;	//���س�ʱ
			}
			else				
			{
				/* There is data to copy from the queue */
				status = queue_remove (qptr, msgptr);
			}	

			/* Exit critical region */
			CRITICAL_END ();
    	}
		else	//����ȡ����
		{
	        /* Protect access to the queue object and OS queues */
	        CRITICAL_START ();

			/* If queue is empty, block the calling thread */
	        if (qptr->num_msgs_stored == 0)
	        {
				status = vkQUEUE_WOULDBLOCK;	//���ض��пհ�          
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

/**
 * QueuePut
 * �����ݷ������
 *
*/
int8_t QueuePut(vkQUEUE *qptr, uint32_t timeout, uint8_t *msgptr)
{
	CRITICAL_STORE;
	int8_t status = vkQUEUE_OK;

	/* Check parameters */
	if ((qptr == NULL) || (msgptr == NULL))
	{
		/* Bad pointer */
		status = vkQUEUE_ERR_PARAM;
	}
	else
	{
		if(timeout)	//��ʱʱ���0
		{
			/* �ȴ������пռ��ʱʱ�䵽 */
			while((qptr->num_msgs_stored == qptr->max_num_msgs) && (--timeout));

			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();
			
			if(timeout == 0)			//��ʱʱ�䵽
		 	{				
				status = vkQUEUE_TIMEOUT;	//���س�ʱ		 		
		 	}
			else
			{
				/* There is space to copy into the queue */
				status = queue_insert (qptr, msgptr);
			}

			/* Exit critical region */
			CRITICAL_END ();			
		}
		else	//����������
		{
			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();

			/* If queue is full, block the calling thread */
			if (qptr->num_msgs_stored == qptr->max_num_msgs)
			{				
				status = vkQUEUE_WOULDBLOCK;		//���ض�������
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

/**
 *  queue_remove �������Ƴ�����
 */
static int8_t queue_remove (vkQUEUE *qptr, uint8_t* msgptr)
{
    int8_t status = vkQUEUE_OK;
	
    /* ������� */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;
    }
    else
    {
        /* �������� */
        memcpy(msgptr, (qptr->buff_ptr + qptr->remove_index), qptr->unit_size);
        qptr->remove_index += qptr->unit_size;
        qptr->num_msgs_stored--;

        /* Check if the remove index should now wrap to the beginning */
        if (qptr->remove_index >= (qptr->unit_size * qptr->max_num_msgs))
            qptr->remove_index = 0;

		/* Successful */
        status = vkQUEUE_OK;
    }

    return (status);
}


/**
 *  queue_insert �����ݲ������ 
 */
static int8_t queue_insert (vkQUEUE *qptr, uint8_t* msgptr)
{
    int8_t status = vkQUEUE_OK;

    /* Check parameters */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;
    }
    else
    {
        /* There is space in the queue, copy it in */
        memcpy ((qptr->buff_ptr + qptr->insert_index), msgptr, qptr->unit_size);
        qptr->insert_index += qptr->unit_size;
        qptr->num_msgs_stored++;

        /* Check if the insert index should now wrap to the beginning */
        if (qptr->insert_index >= (qptr->unit_size * qptr->max_num_msgs))
            qptr->insert_index = 0;

		/* Successful */
		status = vkQUEUE_OK;
    }

    return (status);
}
