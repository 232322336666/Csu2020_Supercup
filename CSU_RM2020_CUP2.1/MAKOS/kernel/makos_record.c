/*
	FILE NAME:	makos_record.c
	DESCRIPTION:	add a record system so we can use printf to store things during interupt.
			and print them out by using serial command "record" to call task_record.
	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/3/31
	MODIFICATION:	write this when having problems in dji's receiver which happens sometimes in irq.
	---------
*/

#include "makos_record.h"

#include "makos_resolver.h"

U32	record_flag = 0;
U8	record_buff[1024] = {0};
U32	record_write = 0;

U32	record_print = 0;

void	record_control(U32 param)
{
	record_print = 1;
}

void stack_print()
{
	p_TCB ptcb;
	stack_info_type stack_info;
	
	p_dnode curr;
	
	curr			= system_param.dhead_record.next;	
	
	while(curr != &system_param.dhead_record)
	{
		ptcb = CONTAINER_OF(curr, TCB_t, dnd_record);
		get_stack(ptcb,&stack_info);
		printf("****%s****\r\n",stack_info.pre_tcb_name);
		printf("stack address:%x\r\n",stack_info.stack_address);
		printf("all:%d bytes\r\n",stack_info.all_stack);
		printf("use:%d bytes\r\n",stack_info.use_stack);
		printf("remain:%d bytes\r\n,",stack_info.remain_stack);
		printf("usage:%d%%\r\n",stack_info.usage);
		
		curr = curr->next;
	}
}

void get_memory_info(void * p_param)
{
	U32 usage = 0;
	usage = my_mem_perused(0);
	printf("memory usage is %d%%\r\n",usage);
}

void	task_record(void* param)
{
////	resolve_add("record", record_control);
////	resolve_add("stack", stack_print);
////	resolve_add("memory",get_memory_info);		
	
	while(1)
	{
		if(record_print == 1)
		{
			task_delay_ms(100);
			printf("\r\nrecord buff :\r\n");
//			printf((char*)record_buff);
			printf("\r\nrecord buff end\r\n");
			record_print = 0;
		}
		task_delay_ms(100);
	}
}

void get_stack(p_TCB p_tcp,p_stack_info_type stack_info)
{
	u32 remain_stack = 0;
	U32 * p_stack;
	U32 stack_total = p_tcp->stack_size/4 - 1;
	p_stack = &p_tcp->task_stack[p_tcp->stack_size/4-1]+1;
//	U32 zero_num = 0;
	
	while(stack_total--)
	{
		if(*p_stack == 0)
		{
			remain_stack ++;
			//remain_stack += zero_num;
			//zero_num = 0;
		}
		else
		{
			remain_stack = 0;
		}
		p_stack --;
	}
	
	stack_info->remain_stack = remain_stack*4;//U8
	stack_info->use_stack = p_tcp->stack_size - stack_info->remain_stack;
	stack_info->all_stack = p_tcp->stack_size;//U8
	stack_info->usage = 100*stack_info->use_stack/stack_info->all_stack;
	stack_info->stack_address = (U32)p_tcp->task_stack;
	stack_info->pre_tcb_name = p_tcp->pr_tcb_name;
}
