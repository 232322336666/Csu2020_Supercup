#include "makos_system.h"
#include "makos_tick.h"
#include "makos_tick_handle.h"
#include "makos_print.h"
#include "makos_idle.h"
#include "makos_resolver.h"
#include "makos_irq_handle.h"

p_TCB		ptcb_curr;
p_TCB		ptcb_next;

//exception_stack_t	exception_stack[EXCEPTION_STACK_SIZE];

_system_param		system_param;




void	makos_init(void)
{
	U8 i;
	
	printf("makos_init\r\n");
	
	my_mem_init(SRAMIN);		//初始化内部内存池
//	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	
	for(i = 0; i <= TASK_PRIO_LOWEST+1; i++)
		dnode_head_init(&system_param.dhead_ready[i]);
	dnode_head_init(&system_param.dhead_delay);
	dnode_head_init(&system_param.dhead_suspend);
	dnode_head_init(&system_param.dhead_record);//初始化TCB记录链表
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	tick_init(1);
	init_tick_callback();
	
	
	dnode_head_init(&dhead_print);
	
	//系统任务创建
	task_create(task_PRINT,		NULL,	0);
	system_param.use_print	= 1;
	
	task_create_full(task_resolver,"task_resolver",NULL,	TASK_PRIO_LOWEST, 400, 150);
	
	task_create(task_idle,NULL,	TASK_PRIO_LOWEST+1);
	
	//task_create(task_irq_handle,	NULL,	0);
	
	task_create(task_record,	NULL,	TASK_PRIO_LOWEST);
	
	printf("makos_init finished\r\n");
}

void	makos_run(void)
{
	
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2)	= NVIC_PENDSV_PRI;
	
	ptcb_curr		= makos_get_prio_highest();
	ptcb_next		= ptcb_curr;
	
	printf("makos start running\r\n");
	
	tick_start();
	
	PENDSV_SET();
}

p_TCB	makos_get_prio_highest(void)
{
	U8	i;
	p_dnode	pdnd;
	p_TCB	ptcb;
	
	for(i=0; i<=TASK_PRIO_LOWEST+1; i++)
	{
		pdnd = &system_param.dhead_ready[i];
		if(pdnd->next != pdnd)
			goto GOT_ONE;
	}
	return NULL;
	
GOT_ONE://成功找到一个就绪任务
	//printf("%d: ph: %d\r\n", system_clock, i);
	pdnd = pdnd->next;
	ptcb = CONTAINER_OF(pdnd, TCB_t, dnd_state);
	return ptcb;
}

void	makos_update_switch(void)
{
	p_TCB	temp_ptcb;
	
	irq_close();//进入临界区
	
	//printf("%d: swi\r\n", system_clock);
	
	temp_ptcb	= makos_get_prio_highest();
	if(temp_ptcb == NULL)
		printf("no task ready\r\n");
	if(temp_ptcb != ptcb_curr)//若最高优先级的就绪任务不为正在执行的任务，则需进行任务切换
	{
		//printf("s%d\r\n", system_clock);
		ptcb_next = temp_ptcb;
		PENDSV_SET();//触发pend_SV中断进行任务切换
	}
	
	irq_restore();//退出临界区
}

