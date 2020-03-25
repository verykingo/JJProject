/******************************************************************************
 * �ļ�  ��vkqueue.c
 * ����    ��ѭ���������
 * ƽ̨    ��ALL
 * ʱ��  ��2019-04-01

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vkqueue.h"

/* Forward declarations */
inline int8_t queue_remove (vkQUEUE *qptr, uint8_t* msgptr);
inline int8_t queue_insert (vkQUEUE *qptr, uint8_t* msgptr);

/*******************************************************************************
 * ����: vkQueueCreate
 * ����: ����ѭ���������
 * �β�: qptr ����ָ��
         buff_ptr ���д洢�ռ��׵�ַ
         unit_size ���е�Ԫ��С,���ֵ255
         max_num_msgs ���д�С,���ֵ255
 * ����: �ɹ�0��ʧ�ܸ���
 * ˵��: �� 
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
 * ����: vkQueueDestory
 * ����: ����ѭ���������
 * �β�: qptr ����ָ��
 * ����: �ɹ�0��ʧ�ܸ���
 * ˵��: �� 
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
 * ����: vkQueueGet
 * ����: ��ȡһ�����е�Ԫ
 * �β�: qptr ����ָ��
 		 timeout ��ʱʱ��
 		 msgptr ���е�Ԫָ��
 * ����: �ɹ�0��ʧ�ܸ���
 * ˵��: �� 
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
    	if(timeout)	//��ʱʱ���0
    	{
    		/* �ȴ����������ݻ�ʱʱ�䵽 */
		    while((!qptr->num_msgs_stored) && (--timeout))
			{nop(); /* ��ֹ�������Ż��� */}
			
			/* Protect access to the queue object and OS queues */
			CRITICAL_START ();
			
			if(timeout == 0)				//��ʱʱ�䵽
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

/*******************************************************************************
 * ����: vkQueuePut
 * ����: ����һ�����е�Ԫ
 * �β�: qptr ����ָ��
 		 timeout ��ʱʱ��
 		 msgptr ���е�Ԫָ��
 * ����: �ɹ�0��ʧ�ܸ���
 * ˵��: �� 
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
		if(timeout)	//��ʱʱ���0
		{
			/* �ȴ������пռ��ʱʱ�䵽 */
			while((qptr->num_msgs_stored == qptr->max_num_msgs) && (--timeout))
			{nop(); /* ��ֹ�������Ż��� */}

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


/*******************************************************************************
 * ����: vkQueueClear
 * ����: ��ն���
 * �β�: qptr ����ָ��
 * ����: �ɹ�0��ʧ�ܸ���
 * ˵��: �� 
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
 * ����: vkQueueEmpty
 * ����: �ж϶����Ƿ��ǿ�
 * �β�: qptr ����ָ��
 * ����: �ɹ�0��ʧ�ܸ���
 * ˵��: �� 
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
 *  queue_remove �������Ƴ�����
 */
inline int8_t queue_remove (vkQUEUE *qptr, uint8_t* msgptr)
{
    int8_t status = vkQUEUE_OK;
	
    /* ������� */
    if ((qptr == NULL) || (msgptr == NULL))
    {
        /* Bad pointer */
        status = vkQUEUE_ERR_PARAM;

		printf("%s param error.\r\n", __FUNCTION__);
    }
    else
    {
        /* �������� */
        memcpy(msgptr, (qptr->buff_ptr + qptr->remove_index*qptr->unit_size), qptr->unit_size);
        qptr->remove_index = (qptr->remove_index+1)%(qptr->max_num_msgs);
        qptr->num_msgs_stored--;

		/* Successful */
        status = vkQUEUE_OK;
    }

    return (status);
}


/**
 *  queue_insert �����ݲ������ 
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
