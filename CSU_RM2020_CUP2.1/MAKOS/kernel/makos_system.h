#ifndef _MAKOS_SYSTEM_H
#define _MAKOS_SYSTEM_H

#include "makos_kernel_includes.h"
#include "makos_task.h"

//typedef	U32		exception_stack_t;

typedef struct
{
	dnode_t		dhead_ready[TASK_PRIO_LOWEST+2];//就绪任务的链表，每个优先级对应一个
	dnode_t		dhead_delay;//等待任务的链表
	dnode_t		dhead_suspend;//挂起任务的链表
	dnode_t		dhead_record;//记录所有的任务控制块
	
	U8		use_print;
} _system_param;

extern	_system_param	system_param;

extern	p_TCB		ptcb_curr;
extern	p_TCB		ptcb_next;


void	makos_init(void);
void	makos_run(void);

p_TCB	makos_get_prio_highest(void);
void	makos_update_switch(void);






#endif
