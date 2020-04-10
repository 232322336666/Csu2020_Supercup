#include "makos_schedule.h"
#include "makos_print.h"

extern U32 system_clock;

void	task_delay_ms(U32 tick)
{
	irq_close();
	
	ptcb_curr->delay_tick = tick;
	dnode_remove(&ptcb_curr->dnd_state);//从原节点中移除
	dnode_add_last(&system_param.dhead_delay, &ptcb_curr->dnd_state);//该任务节点加入到等待任务链表中
	
	#ifdef	DEBUG_TICK
	if(tick != 0)
		printf("%d: ds\r\n", system_clock);
	#endif
	
	makos_update_switch();
	
	//printf("pn: %x\r\n", (U32)ptcb_next);
	
	irq_restore();
}

void	task_period_set(U32 tick)
{
	
}

void	task_period_delay(void)
{
	
}

void	task_yield(void)
{
	
}

void	task_suspend(p_TCB ptcb)
{
	irq_close();
	
	#ifdef		DEBUG_SUSPEND
	printf("%d: sus\r\n", system_clock);
	#endif
	
	dnode_remove(&ptcb->dnd_state);//从原链表中移除
	dnode_add_last(&system_param.dhead_suspend, &ptcb->dnd_state);//该任务节点加入到挂起任务链表中
	
	makos_update_switch();
	
	irq_restore();
}

void	task_resume(p_TCB ptcb)//任务恢复
{
	if(ptcb == NULL)
		return;
	
	irq_close();
	
	#ifdef		DEBUG_SUSPEND
	printf("%d: res\r\n", system_clock);
	#endif
	
	dnode_remove(&ptcb->dnd_state);//从原链表中移除
	dnode_add_first(&system_param.dhead_ready[ptcb->priority], &ptcb->dnd_state);//该任务节点加入对应优先级的就绪任务链表中
	
	makos_update_switch();
	
	irq_restore();
}

void	task_set_prio(p_TCB ptcb, task_prio_t prio)
{
	irq_close();
	
	#ifdef	DEBUG_SCHEDULE
	if(dnode_inlist(&system_param.dhead_delay, &ptcb->dnd_state))
		PRINT_ERROR("try to set prio of delayed task");
	if(dnode_inlist(&system_param.dhead_suspend, &ptcb->dnd_state))
		PRINT_ERROR("try to set prio of suspended task");
	if(prio > TASK_PRIO_LOWEST)
		PRINT_ERROR("priority out of range");
	#endif
	
	dnode_remove(&ptcb->dnd_state);
	dnode_add_last(&system_param.dhead_ready[prio], &ptcb->dnd_state);
	
	irq_restore();
}

