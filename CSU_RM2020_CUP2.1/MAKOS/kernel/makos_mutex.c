#include "makos_mutex.h"
#include "makos_memory.h"
#include "makos_print.h"

extern U32 system_clock;

void	mutex_init(p_mutex pm)
{
	if(pm->dhead_wait.next)
		PRINT_ERROR("mutex has already been inited");//防止重复定义
		
	pm->owner	= NULL;//标记为未被拥有
	pm->ceiling	= TASK_PRIO_LOWEST;
	pm->level	= 0;
	
	dnode_head_init(&pm->dhead_wait);
}


//更新互斥量的最高优先级
void	mutex_update_ceiling(p_mutex pm)
{
	p_dnode		curr;
	p_TCB		ptcb;
	task_prio_t	res;
	
	curr		= pm->dhead_wait.next;
	pm->ceiling	= TASK_PRIO_LOWEST;
	while(curr != &pm->dhead_wait)//遍历互斥量的等待队列
	{
		ptcb	= CONTAINER_OF(curr, TCB_t, dnd_sus);
		if(ptcb->priority < res)
			pm->ceiling = ptcb->priority;
		curr	= curr->next;
	}
}


//获取互斥量，阻塞版
void	mutex_lock(p_mutex pm)
{
	retry:
	irq_close();
	
	if(pm->dhead_wait.next == NULL)//未被初始化
	{
		task_delay_ms(0);
		irq_restore();
		goto retry;
	}
	
	if(pm->owner == NULL)//未被占有
	{
		pm->owner = ptcb_curr;//占有
		/*
		if(pm->dhead_wait.next == &ptcb_curr->dnd_sus)
		{
			dnode_remove(&ptcb_curr->dnd_sus);
		}
		*/
		#ifdef		DEBUG_MUTEX
		printf("%d: ml\r\n", system_clock);
		#endif
	}

	
	if(pm->owner == ptcb_curr)//当前任务首次占用
	{
		pm->level++;
	}
	else//其他任务试图占用已被占用的互斥量
	{
		if(ptcb_curr->dnd_sus.next != NULL)
			PRINT_ERROR("task got suspended for more than one reason");
		dnode_add_last(&pm->dhead_wait, &ptcb_curr->dnd_sus);//挂起节点加入到互斥量的等待队列中
		
		//mutex_update_ceiling(pm);
		if(ptcb_curr->priority < pm->ceiling)//提高互斥量的最高优先级，任务本身的优先级并不会改变
			pm->ceiling	= ptcb_curr->priority;
		if(pm->owner->priority > pm->ceiling)
		{
			task_set_prio(pm->owner, pm->ceiling);//提高当前任务的优先级
			/*
			dnode_remove(&pm->owner->dnd_state);
			dnode_add_last(&system_param.dhead_ready[pm->ceiling], &pm->owner->dnd_state);
			*/
		}
		
		task_suspend(ptcb_curr);//挂起当前试图占有已被占有的互斥量的任务

		irq_restore();
		goto retry;
	}
	irq_restore();
}
	
void	mutex_unlock(p_mutex pm)
{
	irq_close();
	if(pm->owner != ptcb_curr)//出错
		PRINT_ERROR("try to unlock mutex by non-owner");
	pm->level--;
	if(!pm->level)//完全释放
	{
		#ifdef		DEBUG_MUTEX
		printf("%d: mu\r\n", system_clock);
		#endif
		
		pm->owner = NULL;//解除占有
		if(ptcb_curr->priority != pm->ceiling)
			task_set_prio(ptcb_curr, ptcb_curr->priority);
		if(pm->dhead_wait.next != &pm->dhead_wait)//等待队列有任务
		{
			p_TCB ptcb;

			ptcb	= CONTAINER_OF(pm->dhead_wait.next, TCB_t, dnd_sus);
			dnode_remove(&ptcb->dnd_sus);//从互斥量的等待队列中取出该节点
			mutex_update_ceiling(pm);//有任务从互斥量等待队列中取出，故需重新获取互斥量的最高优先级
			
			task_resume(ptcb);
		}
	}
	irq_restore();
}


U32	mutex_lock_try(p_mutex pm)
{
	irq_close();
	
	if(pm->owner == NULL)
	{
		pm->owner = ptcb_curr;
	}
	if(pm->owner == ptcb_curr)
	{
		pm->level++;
		irq_restore();
		return	1U;
	}
	else
	{
		irq_restore();
		return	0U;
	}
}











