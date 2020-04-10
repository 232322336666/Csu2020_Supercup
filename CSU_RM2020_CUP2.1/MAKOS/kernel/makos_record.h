#ifndef _MAKOS_RECORD_H
#define _MAKOS_RECORD_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"


extern
U32	record_flag;
extern
U8	record_buff[1024];
extern
U32	record_write;

typedef struct _stack_info
{
	U32 usage;
	U32 use_stack;
	U32 remain_stack;
	U32 all_stack;
	U32 stack_address;
	u8 * pre_tcb_name;
}stack_info_type, *p_stack_info_type;

void	task_record(void* param);
void get_stack(p_TCB p_tcp,p_stack_info_type stack_info);//获取某个人物的堆栈使用情况





#endif
