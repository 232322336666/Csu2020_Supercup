#include "makos_semaphore.h"
#include "makos_memory.h"
#include "makos_print.h"

void	sem_init(p_sem psem, U32 cnt)
{
	if(psem->dhead_wait.next)
		PRINT_ERROR("sem has already been inited");
	
	psem->count	= cnt;
	
	dnode_head_init(&psem->dhead_wait);//初始化该信号的任务等待列表
}


void	sem_ask(p_sem psem)//请求信号量,阻塞
{
	retry:
	irq_close();//临界区
	
	if(psem->dhead_wait.next == NULL)//未被初始化
	{
		task_delay_ms(0);
		irq_restore();
		goto retry;
	}
	
	if(psem->count)
	{
		psem->count--;
		if(psem->dhead_wait.next == &ptcb_curr->dnd_sus)//从该信号量的等待任务列表中移除该任务节点
			dnode_remove(&ptcb_curr->dnd_sus);
		irq_restore();
	}
	else
	{
		if(ptcb_curr->dnd_sus.next == NULL)//该任务不处于其他等待列表中
			dnode_add_last(&psem->dhead_wait, &ptcb_curr->dnd_sus);//加入该信号量的任务等待列表
		task_suspend(ptcb_curr);
		irq_restore();
		goto retry;//阻塞该任务，直至有信号量被释放
	}
}

void	sem_release(p_sem psem)
{
	irq_close();
	psem->count++;
	if(psem->dhead_wait.next != &psem->dhead_wait)//有任务正在等待该信号量
	{
		p_TCB ptcb = CONTAINER_OF(psem->dhead_wait.next, TCB_t, dnd_sus);
		//dnode_remove(psem->dhead_wait.next);
		task_resume(ptcb);//恢复等待的任务
	}
	irq_restore();
}














