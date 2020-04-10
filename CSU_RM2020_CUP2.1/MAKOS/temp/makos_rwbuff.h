#ifndef _MAKOS_RWBUFF_H
#define _MAKOS_RWBUFF_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"
#include "makos_atomic.h"

typedef	struct	_rwbuff_t
{
	U32	size;
	atomic	count0;
	atomic	count1;
	void*	buff_start;
}rwbuff_t, *p_rwbuff;

void	rwbuff_init(p_rwbuff pb, U32 size);
void	rwbuff_write(p_rwbuff pb, void* data);
void	rwbuff_read(p_rwbuff pb, void* data);


#endif
