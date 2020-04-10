#include "makos_task.h"
#include "makos_system.h"
#include "makos_memory.h"
#include "string.h"
#include "makos_print.h"


p_TCB	task_create_full(task_f func,u8 * pre_tcb_name,void* param, task_prio_t prio,U32 stack_size,U32 print_size)
{
	p_TCB		ptcb;
	
	ptcb			= (p_TCB)mymalloc(SRAMIN,sizeof(TCB_t));
	ptcb->pr_tcb_name = pre_tcb_name;
	if(stack_size%4 != 0)
	{
		printf("error!!! the stack_size must be align with 4\r\n");
		return NULL;
	}	
	ptcb->stack_size = stack_size;
	ptcb->task_stack = (task_stack_t * )mymalloc(SRAMIN,ptcb->stack_size);//内存块为32字节且内存池为8字节对齐
	ptcb->sp		= &ptcb->task_stack[ptcb->stack_size/4-1]+1;								//故申请的内存也是8字节对齐，可保障printf浮点的正常输出	
	
	ptcb->state		= state_ready;//初始化为就绪状态
	ptcb->priority		= prio;
	ptcb->delay_tick	= 0;
	ptcb->period_usage	= disable;
	
	dnode_add_last(&system_param.dhead_ready[prio], &ptcb->dnd_state);
	dnode_add_last(&system_param.dhead_record, &ptcb->dnd_record);
	pipe_init(&ptcb->pipe_print, print_size);//为print管道分配内存

	dnode_add_last(&dhead_print, &ptcb->dnd_print);
	
	//初始化堆栈空间
	//fpu
	*(--ptcb->sp)		= (U32)0x00000000;
	*(--ptcb->sp)		= (U32)0x00001000;
	*(--ptcb->sp)		= (U32)0x15151515;
	*(--ptcb->sp)		= (U32)0x14141414;
	*(--ptcb->sp)		= (U32)0x13131313;
	*(--ptcb->sp)		= (U32)0x12121212;
	*(--ptcb->sp)		= (U32)0x11111111;
	*(--ptcb->sp)		= (U32)0x10101010;
	*(--ptcb->sp)		= (U32)0x09090909;
	*(--ptcb->sp)		= (U32)0x08080808;
	*(--ptcb->sp)		= (U32)0x07070707;
	*(--ptcb->sp)		= (U32)0x06060606;
	*(--ptcb->sp)		= (U32)0x05050505;
	*(--ptcb->sp)		= (U32)0x04040404;
	*(--ptcb->sp)		= (U32)0x03030303;
	*(--ptcb->sp)		= (U32)0x02020202;
	*(--ptcb->sp)		= (U32)0x01010101;
	*(--ptcb->sp)		= (U32)0x00000000;
	//fpu
	
	*(--ptcb->sp)		= (U32)(1<<24);		//
	*(--ptcb->sp)		= (U32)func;		//pc指针,程序的入口地址
	*(--ptcb->sp)		= (U32)0xFFFFFFED;	//R14,未用
	*(--ptcb->sp)		= (U32)0x12121212;	//R12
	*(--ptcb->sp)		= (U32)0x03030303;	//R3
	*(--ptcb->sp)		= (U32)0x02020202;	//R2
	*(--ptcb->sp)		= (U32)0x01010101;	//R1
	*(--ptcb->sp)		= (U32)param;		//R0,入口参数param
	
	//fpu
	*(--ptcb->sp)		= (U32)0x31313131;
	*(--ptcb->sp)		= (U32)0x30303030;
	*(--ptcb->sp)		= (U32)0x29292929;
	*(--ptcb->sp)		= (U32)0x28282828;
	*(--ptcb->sp)		= (U32)0x27272727;
	*(--ptcb->sp)		= (U32)0x26262626;
	*(--ptcb->sp)		= (U32)0x25252525;
	*(--ptcb->sp)		= (U32)0x24242424;
	*(--ptcb->sp)		= (U32)0x23232323;
	*(--ptcb->sp)		= (U32)0x22222222;
	*(--ptcb->sp)		= (U32)0x21212121;
	*(--ptcb->sp)		= (U32)0x20202020;
	*(--ptcb->sp)		= (U32)0x19191919;
	*(--ptcb->sp)		= (U32)0x18181818;
	*(--ptcb->sp)		= (U32)0x17171717;
	*(--ptcb->sp)		= (U32)0x16161616;
	//fpu
	
	*(--ptcb->sp)		= (U32)0x11111111;	//R11
	*(--ptcb->sp)		= (U32)0x10101010;	//R10
	*(--ptcb->sp)		= (U32)0x09090909;	//R9
	*(--ptcb->sp)		= (U32)0x08080808;	//R8
	*(--ptcb->sp)		= (U32)0x07070707;	//R7
	*(--ptcb->sp)		= (U32)0x06060606;	//R6
	*(--ptcb->sp)		= (U32)0x05050505;	//R5
	*(--ptcb->sp)		= (U32)0x04040404;	//R4
	
	
	//for debug
	printf("****%s*****\r\n",ptcb->pr_tcb_name);
	printf("new ptcb: %x\r\n", (U32)ptcb);
	printf("new ptcb stack: %x\r\n", (U32)ptcb->task_stack);
	printf("stack's end address is:%x\r\n",(U32)&ptcb->task_stack[ptcb->stack_size/4 - 1]);
	//printf("print pipe's buff is:%x\r\n",(U32)ptcb->pipe_print.start);
	//printf("the fun of the task's address is :%x\r\n",(U32)func);
	
	return ptcb;
}

p_TCB	task_create_full_CCM(task_f func,u8 * pre_tcb_name,void* param, task_prio_t prio,U32 stack_size,U32 print_size)
{
	p_TCB		ptcb;
	
	ptcb			= (p_TCB)mymalloc(SRAMCCM,sizeof(TCB_t));
	ptcb->pr_tcb_name = pre_tcb_name;
	if(stack_size%4 != 0)
	{
		printf("error!!! the stack_size must be align with 4\r\n");
		return NULL;
	}	
	ptcb->stack_size = stack_size;
	ptcb->task_stack = (task_stack_t * )mymalloc(SRAMCCM,ptcb->stack_size);//内存块为32字节且内存池为8字节对齐
	ptcb->sp		= &ptcb->task_stack[ptcb->stack_size/4-1]+1;								//故申请的内存也是8字节对齐，可保障printf浮点的正常输出	
	
	ptcb->state		= state_ready;//初始化为就绪状态
	ptcb->priority		= prio;
	ptcb->delay_tick	= 0;
	ptcb->period_usage	= disable;
	
	dnode_add_last(&system_param.dhead_ready[prio], &ptcb->dnd_state);
	dnode_add_last(&system_param.dhead_record, &ptcb->dnd_record);
	pipe_init(&ptcb->pipe_print, print_size);//为print管道分配内存

	dnode_add_last(&dhead_print, &ptcb->dnd_print);
	
	//初始化堆栈空间
	//fpu
	*(--ptcb->sp)		= (U32)0x00000000;
	*(--ptcb->sp)		= (U32)0x00001000;
	*(--ptcb->sp)		= (U32)0x15151515;
	*(--ptcb->sp)		= (U32)0x14141414;
	*(--ptcb->sp)		= (U32)0x13131313;
	*(--ptcb->sp)		= (U32)0x12121212;
	*(--ptcb->sp)		= (U32)0x11111111;
	*(--ptcb->sp)		= (U32)0x10101010;
	*(--ptcb->sp)		= (U32)0x09090909;
	*(--ptcb->sp)		= (U32)0x08080808;
	*(--ptcb->sp)		= (U32)0x07070707;
	*(--ptcb->sp)		= (U32)0x06060606;
	*(--ptcb->sp)		= (U32)0x05050505;
	*(--ptcb->sp)		= (U32)0x04040404;
	*(--ptcb->sp)		= (U32)0x03030303;
	*(--ptcb->sp)		= (U32)0x02020202;
	*(--ptcb->sp)		= (U32)0x01010101;
	*(--ptcb->sp)		= (U32)0x00000000;
	//fpu
	
	*(--ptcb->sp)		= (U32)(1<<24);		//
	*(--ptcb->sp)		= (U32)func;		//pc指针,程序的入口地址
	*(--ptcb->sp)		= (U32)0xFFFFFFED;	//R14,未用
	*(--ptcb->sp)		= (U32)0x12121212;	//R12
	*(--ptcb->sp)		= (U32)0x03030303;	//R3
	*(--ptcb->sp)		= (U32)0x02020202;	//R2
	*(--ptcb->sp)		= (U32)0x01010101;	//R1
	*(--ptcb->sp)		= (U32)param;		//R0,入口参数param
	
	//fpu
	*(--ptcb->sp)		= (U32)0x31313131;
	*(--ptcb->sp)		= (U32)0x30303030;
	*(--ptcb->sp)		= (U32)0x29292929;
	*(--ptcb->sp)		= (U32)0x28282828;
	*(--ptcb->sp)		= (U32)0x27272727;
	*(--ptcb->sp)		= (U32)0x26262626;
	*(--ptcb->sp)		= (U32)0x25252525;
	*(--ptcb->sp)		= (U32)0x24242424;
	*(--ptcb->sp)		= (U32)0x23232323;
	*(--ptcb->sp)		= (U32)0x22222222;
	*(--ptcb->sp)		= (U32)0x21212121;
	*(--ptcb->sp)		= (U32)0x20202020;
	*(--ptcb->sp)		= (U32)0x19191919;
	*(--ptcb->sp)		= (U32)0x18181818;
	*(--ptcb->sp)		= (U32)0x17171717;
	*(--ptcb->sp)		= (U32)0x16161616;
	//fpu
	
	*(--ptcb->sp)		= (U32)0x11111111;	//R11
	*(--ptcb->sp)		= (U32)0x10101010;	//R10
	*(--ptcb->sp)		= (U32)0x09090909;	//R9
	*(--ptcb->sp)		= (U32)0x08080808;	//R8
	*(--ptcb->sp)		= (U32)0x07070707;	//R7
	*(--ptcb->sp)		= (U32)0x06060606;	//R6
	*(--ptcb->sp)		= (U32)0x05050505;	//R5
	*(--ptcb->sp)		= (U32)0x04040404;	//R4
	
	
	//for debug
	printf("****%s*****\r\n",ptcb->pr_tcb_name);
	printf("new ptcb: %x\r\n", (U32)ptcb);
	printf("new ptcb stack: %x\r\n", (U32)ptcb->task_stack);
	printf("stack's end address is:%x\r\n",(U32)&ptcb->task_stack[ptcb->stack_size/4 - 1]);
	//printf("print pipe's buff is:%x\r\n",(U32)ptcb->pipe_print.start);
	//printf("the fun of the task's address is :%x\r\n",(U32)func);
	
	return ptcb;
}

void task_delete(u8 * pre_task_name)
{
	p_TCB ptcb;
//	stack_info_type stack_info;
	p_dnode curr;
	u8 find_task = 0;
	u8 * delete_task_name = 0;
	u8 * pr_delete_task = 0;
	
	curr			= system_param.dhead_record.next;	
	
	while(curr != &system_param.dhead_record)
	{
		ptcb = CONTAINER_OF(curr, TCB_t, dnd_record);
		if(strcmp((char*) ptcb->pr_tcb_name,(char*)pre_task_name) == 0)
		{
			pr_delete_task = (u8 *)ptcb;
			delete_task_name = ptcb->pr_tcb_name;
			find_task = 1;
			break;				
		}	
		curr = curr->next;
		if(find_task == 0)
		{
			printf("error!!! there is not such task\r\n");
		}
		else
		{
			irq_close();
			
			dnode_remove(&ptcb_curr->dnd_state);
			dnode_remove(&ptcb_curr->dnd_record);
			dnode_remove(&ptcb_curr->dnd_print);
			dnode_remove(&ptcb_curr->dnd_sus);
			
			myfree(SRAMIN,pr_delete_task);
			
			irq_restore();
			
			printf("task: %s has been delete\r\n",delete_task_name);
		}
	}	
	
	
	
}

