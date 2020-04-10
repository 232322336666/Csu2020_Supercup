#ifndef _MAKOS_FUNC_LIST_H
#define _MAKOS_FUNC_LIST_H

#include "makos_type.h"
#include "makos_doublelist.h"
#include "makos_memory.h"

typedef	struct	_fl_node_t
{
	dnode_t	node;
	void41f	func;
}fl_node_t, *p_fl_node;

#define	FUNC_LIST_DEFINE(name)						\
	DHEAD_DEFINE(name##_fl_head);					\
	void	name##_fl_run(void* p1, void* p2, void* p3, void* p4)	\
	{								\
		p_dnode		curr;					\
		for(curr = name##_fl_head.next;				\
		curr != &name##_fl_head;				\
		curr = curr->next){					\
			(CONTAINER_OF(curr, fl_node_t, node)->func)(	\
				p1, p2, p3, p4);			\
		}							\
	}								\
	void	name##_fl_add(void41f func)				\
	{								\
		p_fl_node	pnode = mymalloc(SRAMIN,sizeof(fl_node_t));	\
		pnode->func	= func;					\
		irq_close();						\
		dnode_add_last(&name##_fl_head, &pnode->node);		\
		irq_restore();						\
	}

#define	FUNC_LIST_DECLARE(name)						\
	void	name##_fl_run(void* p1, void* p2, void* p3, void* p4);	\
	void	name##_fl_add(void41f func);
	

#endif
