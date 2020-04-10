#ifndef _MAKOS_MESSAGE_H
#define _MAKOS_MESSAGE_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"

#include "makos_mutex.h"

typedef struct _msg_t
{
	void*	start;
	U8	amount;//消息个数
	U8	size;//单个消息大小
	
	mutex_t	rwlock;
	U8	write;
	U8	read;
	VU8	usage;//已使用消息个数
}msg_t, *p_msg;

void	msg_init(p_msg pmsg, U8 amount, U8 size);
void*	msg_get_write(p_msg pmsg);
void	msg_write_finish(p_msg pmsg);
void*	msg_get_read(p_msg pmsg);
void	msg_read_finish(p_msg pmsg);

void*	msg_get_write_last(p_msg pmsg);	//only use for irq_handle in order to react fast
					//do not use get_write on the same msg
					//if used, then only one writer is allowed
void	msg_write_finish_last(p_msg pmsg);
void*	msg_get_read_try(p_msg pmsg);
void	msg_read_finish_try(p_msg pmsg);

void*	msg_get_write_try(p_msg pmsg);

void*	msg_get_write_some(p_msg pmsg);
void	msg_write_finish_some(p_msg pmsg);
void*	msg_get_read_some(p_msg pmsg);
void	msg_read_finish_some(p_msg pmsg);

#endif
