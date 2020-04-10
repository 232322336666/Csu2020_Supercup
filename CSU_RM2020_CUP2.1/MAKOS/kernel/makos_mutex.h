#ifndef _MAKOS_MUTEX_H
#define _MAKOS_MUTEX_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"

typedef	struct	_mutex_t
{
	p_TCB	owner;
	dnode_t	dhead_wait;
	U8	ceiling;//上限
	U8	level;	//当前值
}mutex_t, *p_mutex;

void	mutex_init(p_mutex pm);
void	mutex_lock(p_mutex pm);
void	mutex_unlock(p_mutex pm);
U32		mutex_lock_try(p_mutex pm);

#endif
