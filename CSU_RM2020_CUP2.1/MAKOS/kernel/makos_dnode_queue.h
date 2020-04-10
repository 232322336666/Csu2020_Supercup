#ifndef _MAKOS_DNODE_QUEUE_H
#define _MAKOS_DNODE_QUEUE_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"




void	dndqueue_init(p_dnode pdhd);
void	dndqueue_add_first(p_dnode pdhd, p_dnode pdnd);
void	dndqueue_add_last(p_dnode pdhd, p_dnode pdnd);
void	dndqueue_remove(p_dnode pdnd);



#endif
