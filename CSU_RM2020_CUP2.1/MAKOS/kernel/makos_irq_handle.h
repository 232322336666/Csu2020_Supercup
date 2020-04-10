#ifndef _MAKOS_IRQ_HANDLE_H
#define _MAKOS_IRQ_HANDLE_H

#include "makos_kernel_includes.h"
#include "makos_schedule.h"
#include "makos_dnode_queue.h"

typedef	void	(*irq_handle_f)(U32 count);
typedef	U32	irq_id_t;

typedef	struct	_irq_info_t
{
	irq_id_t	id;
	irq_handle_f	func;
	U32		count;
	U32		prio;
	dnode_t		dnd;
}irq_info_t, *p_irq_info;


U32	irq_get_active(void);
void	makos_irq_handle(void);

//can only use this function in task functions.
void	add_irq_handle(irq_id_t id, irq_handle_f func, U32 prio);
void	task_irq_handle(void* param);



#endif
