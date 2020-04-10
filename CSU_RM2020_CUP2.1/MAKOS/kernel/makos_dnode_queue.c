#include "makos_dnode_queue.h"
#include "makos_print.h"


void	dndqueue_init(p_dnode pdhd)
{
	#ifdef	DEBUG_DNDQUEUE
	if(pdhd == NULL)
		PRINT_ERROR("");
	if(pdhd->next)
		PRINT_ERROR("dnode has already been inited");
	#endif
	
	dnode_head_init(pdhd);
}

void	dndqueue_add_first(p_dnode pdhd, p_dnode pdnd)
{
	#ifdef	DEBUG_DNDQUEUE
	if(pdhd == NULL)
		PRINT_ERROR("");
	#endif
	
	retry:
	if(pdhd->next == NULL)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	irq_close();
	
	dnode_add_first(pdhd, pdnd);
	
	irq_restore();
}

void	dndqueue_add_last(p_dnode pdhd, p_dnode pdnd)
{
	#ifdef	DEBUG_DNDQUEUE
	if(pdhd == NULL)
		PRINT_ERROR("");
	#endif
	
	retry:
	if(pdhd->next == NULL)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	irq_close();
	
	dnode_add_last(pdhd, pdnd);
	
	irq_restore();
}

void	dndqueue_remove(p_dnode pdnd)
{
	irq_close();
	
	dnode_remove(pdnd);
	
	irq_restore();
}









