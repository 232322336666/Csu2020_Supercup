#include "makos_tick_handle.h"

volatile	system_tick_t	system_clock;


os_error	init_tick_callback(void)
{
	tick_callback = tick_handle;
	
	return os_noerror;
}

void keep_clock_awake(U32 clock)
{
	clock = clock;
}

void		tick_handle(void)//节拍服务函数
{
	volatile p_dnode	curr, pdnd;//curr,当前节点，pdnd,上一节点
	p_TCB	ptcb;
	
	irq_close();//进入临界区
	
	//keep_clock_awake(system_clock);
	system_clock++;
	
	#ifdef	DEBUG_TICK
	if(!(system_clock%1000))
		printf("%d: tick\r\n", system_clock);
	#endif
	
	curr = system_param.dhead_delay.next;//设置为下一节点，方便找一圈

	while(curr != &system_param.dhead_delay)//遍历等待任务链表
	{
		pdnd	= curr;
		curr	= curr->next;
		ptcb	= CONTAINER_OF(pdnd, TCB_t, dnd_state);//根据链表反找任务控制块
		
		
		if(ptcb->delay_tick)
			ptcb->delay_tick--;

		if(!ptcb->delay_tick)//延时时间到，将该任务加入到就绪任务链表中
		{
			//printf("%d: c\r\n", system_clock);
			/*
			if(system_clock >= 100)
			{
				printf("curr: %x\r\n", (U32)curr);
				printf("ptcb: %x\r\n", (U32)ptcb);
				printf("dnd: %x\r\n", (U32)&ptcb->dnd_state);
			}
			*/
			dnode_remove(&ptcb->dnd_state);
			dnode_add_last(&system_param.dhead_ready[ptcb->priority], &ptcb->dnd_state);
			#ifdef	DEBUG_TICK
				//printf("%d: do\r\n", system_clock);
			#endif
		}
	}
	
	irq_restore();//退出临界区
	
	makos_update_switch();
}


