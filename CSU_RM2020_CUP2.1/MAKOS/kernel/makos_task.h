#ifndef _MAKOS_TASK_H
#define _MAKOS_TASK_H

#include "makos_kernel_includes.h"

#include "makos_pipe.h"

typedef	void	(*task_f)(void*);
typedef		U32		task_stack_t;
typedef		U32		task_tick_t;
typedef 	U8		task_prio_t;
typedef enum
{
	state_run,
	state_ready,
	state_delay,
	state_suspend,
} task_state_t;

#pragma pack(8) 
typedef struct _TCB_t
{
	task_stack_t*	sp;
	
	task_state_t	state;
	task_prio_t	priority;
	task_tick_t	delay_tick;
	usage_t		period_usage;//p
	
	dnode_t		dnd_state;
	dnode_t		dnd_sus;
	dnode_t		dnd_record;
	
	pipe_t		pipe_print;
	dnode_t		dnd_print;
	
	U8 * pr_tcb_name;

	int temp;//用于解决任务堆栈8字节对齐	
	
	U32 stack_size;//U8
	task_stack_t	* task_stack;
} TCB_t, *p_TCB;
#pragma pack() 

#define	task_create(func, param, prio)	task_create_full(func, #func, param, prio, TASK_STACK_SIZE,PIPE_PRINT_SIZE)

p_TCB	task_create_full(task_f func,u8 * pre_tcb_name,void* param, task_prio_t prio,U32 stack_size,U32 print_size);
void task_delete(u8 * pre_task_name);

#define	task_insert_full(func, param, prio, stack_size,pipe)		\
	do							\
	{							\
		irq_close();					\
		task_create_full(func, #func,param, prio, stack_size,pipe);	\
		irq_restore();					\
	}while(0)				

#define	task_insert(func, param, prio)	task_insert_full((task_f)func,(u8*)param, prio, TASK_STACK_SIZE,PIPE_PRINT_SIZE)

/*******************************************使用CCM内存运行的任务************************************************************************************/
	
#define	task_createC_CCM(func, param, prio)	task_create_full_CCM(func, #func, param, prio, TASK_STACK_SIZE,PIPE_PRINT_SIZE)

p_TCB	task_create_full_CCM(task_f func,u8 * pre_tcb_name,void* param, task_prio_t prio,U32 stack_size,U32 print_size);
void task_delete_CCM(u8 * pre_task_name);

#define	task_insert_full_CCM(func, param, prio, stack_size,pipe)		\
	do							\
	{							\
		irq_close();					\
		task_create_full_CCM(func, #func,param, prio, stack_size,pipe);	\
		irq_restore();					\
	}while(0)				

#define	task_insert_CCM(func, param, prio)	task_insert_full_CCM(func, param, prio, TASK_STACK_SIZE,PIPE_PRINT_SIZE)


#endif
