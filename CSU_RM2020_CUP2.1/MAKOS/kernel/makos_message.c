#include "makos_message.h"
#include "makos_memory.h"
#include "makos_print.h"
#include "timer.h"

void	msg_init(p_msg pmsg, U8 amount, U8 size)
{
	if(pmsg->size)
		PRINT_ERROR("message has already been inited");
	
	irq_close();
	
	pmsg->start	= mymalloc(SRAMIN,size*amount);
	pmsg->amount	= amount;
	pmsg->size	= size;
	
	mutex_init(&pmsg->rwlock);
	pmsg->write	= 0;
	pmsg->read	= 0;
	pmsg->usage	= 0;
	
	irq_restore();
}

//发送消息
void*	msg_get_write(p_msg pmsg)
{
	retry:
	if(pmsg->start == NULL)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	mutex_lock(&pmsg->rwlock);
	
	if(pmsg->usage >= pmsg->amount)
		task_suspend(ptcb_curr);
	
	return (void*)((U8*)pmsg->start + (pmsg->write)*(pmsg->size));
}

void	msg_write_finish(p_msg pmsg)
{
	irq_close();
	
	pmsg->usage++;
	pmsg->write++;
	if(pmsg->write >= pmsg->amount)//循环缓冲
		pmsg->write = 0;
	
	mutex_unlock(&pmsg->rwlock);
	
	irq_restore();
}


//阻塞版本等待消息
void*	msg_get_read(p_msg pmsg)
{
	retry:
	if(pmsg == NULL || pmsg->start == NULL)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	if(pmsg->usage == 0)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	return (void*)((U8*)pmsg->start + (pmsg->read)*(pmsg->size));
}

void	msg_read_finish(p_msg pmsg)
{
	irq_close();
	
	pmsg->usage--;
	pmsg->read++;
	if(pmsg->read >= pmsg->amount)
		pmsg->read = 0;
	
	if(pmsg->usage == pmsg->amount-1)
		task_resume(pmsg->rwlock.owner);
	
	irq_restore();
}

void*	msg_get_write_last(p_msg pmsg)
{
	void*	res;
	
	irq_close();
	
	if(pmsg->usage >= pmsg->amount)//超出部分舍弃
	{
		pmsg->usage		= pmsg->amount - 1;
		
		if(pmsg->write == 0)//环形缓冲的临界处理
			pmsg->write	= pmsg->amount;
		else
			pmsg->write	= pmsg->write - 1;
	}
	res	= (void*)((U8*)pmsg->start + (pmsg->write)*(pmsg->size));
	
	irq_restore();
	
	return	res;
}

void	msg_write_finish_last(p_msg pmsg)
{
	irq_close();
	
	pmsg->usage++;
	pmsg->write++;
	if(pmsg->write >= pmsg->amount)//环形缓冲机制
		pmsg->write = 0;
	
	irq_restore();
}

//不阻塞版等待邮箱
void*	msg_get_read_try(p_msg pmsg)
{
	if(pmsg->usage == 0)
	{
		return	NULL;
	}
	else
	{
		return (void*)((U8*)pmsg->start + (pmsg->read)*(pmsg->size));
	}
}

void	msg_read_finish_try(p_msg pmsg)
{
	irq_close();
	
	pmsg->usage--;
	pmsg->read++;
	if(pmsg->read >= pmsg->amount)//环形缓冲机制
		pmsg->read = 0;
	
	if(pmsg->usage == pmsg->amount-1)
		task_resume(pmsg->rwlock.owner);
	
	irq_restore();
}



void*	msg_get_write_try(p_msg pmsg)
{
	if(pmsg->start	== NULL)
		return	NULL;
	if(mutex_lock_try(&pmsg->rwlock))
	{
		if(pmsg->usage >= pmsg->amount)
			//task_suspend(ptcb_curr);
			return	NULL;
		
		return (void*)((U8*)pmsg->start + (pmsg->write)*(pmsg->size));
	}
	else
		return	NULL;
}

/*************遥控器一对多的消息函数***************/
void*	msg_get_write_some(p_msg pmsg)
{
	void*	res;
	
	irq_close();
	
	res	= (void*)((U8*)pmsg->start + (pmsg->write)*(pmsg->size));
	
	irq_restore();
	
	return	res;
}

void	msg_write_finish_some(p_msg pmsg)
{
	irq_close();
	
	pmsg->write++;
	if(pmsg->write >= pmsg->amount)
		pmsg->write = 0;
	
	irq_restore();
}
void*	msg_get_read_some(p_msg pmsg)
{
	if(canrate.send.dbus<10)
	{
		return NULL;
	}
	return (void*)((U8*)pmsg->start + (pmsg->read)*(pmsg->size));
}

void	msg_read_finish_some(p_msg pmsg)
{
	irq_close();
	
	pmsg->read++;
	if(pmsg->read >= pmsg->amount)//环形缓冲机制
		pmsg->read = 0;
	
	irq_restore();
}
