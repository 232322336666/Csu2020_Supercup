#include "makos_irq_handle.h"
#include "makos_memory.h"
#include "makos_print.h"
#include "makos_tick_handle.h"

dnode_t		dhead_irq;
p_TCB		ptcb_irq_handle;

U32	irq_get_active(void)
{
	return	((*((U32*)(0xE000ED04))) & 0x003F) - 16;
}


void	makos_irq_handle(void)
{
	irq_id_t	id;
	p_dnode		curr;
	p_irq_info	pinfo;
	
	id	= irq_get_active();
	for(curr = dhead_irq.next; curr != &dhead_irq; curr = curr->next)
	{
		pinfo	= CONTAINER_OF(curr, irq_info_t, dnd);
		if(pinfo->id == id)
		{
			pinfo->count++;
			task_resume(ptcb_irq_handle);
			return;
		}
	}
}

//can only use this function in task functions.
void	add_irq_handle(irq_id_t id, irq_handle_f func, U32 prio)
{
	p_dnode		curr;
	p_irq_info	temp;
	p_irq_info	pinfo;
	
	pinfo		= mymalloc(SRAMIN,sizeof(irq_info_t));
	pinfo->id	= id;
	pinfo->func	= func;
	pinfo->count	= 0;
	pinfo->prio	= prio;
	
	irq_close();
	
	for(curr = dhead_irq.next; curr != &dhead_irq; curr = curr->next)
	{
		temp	= CONTAINER_OF(curr, irq_info_t, dnd);
		if(temp->prio > prio)
			break;
	}
	dndqueue_add_last(curr, &pinfo->dnd);
	
	irq_restore();
}


void	task_irq_handle(void* param)
{
	p_dnode		curr;
	p_irq_info	pinfo;
	U32		counttemp;
	
	ptcb_irq_handle	= ptcb_curr;
	dndqueue_init(&dhead_irq);
	/*
	PRINT("task_irq inited\r\n");
	*/
	while(1)
	{
		for(curr = dhead_irq.next; curr != &dhead_irq; curr = curr->next)
		{
			pinfo	= CONTAINER_OF(curr, irq_info_t, dnd);
			if(pinfo->count)
			{
				counttemp	= pinfo->count;
				pinfo->count	= 0;
				pinfo->func(counttemp);
			}
		}
		/*
		if(system_clock%1000 == 1)
			PRINT("task_irq is running\r\n");
		*/
		task_delay_ms(0);
	}
}


