#ifndef _MAKOS_CONFIG_H
#define _MAKOS_CONFIG_H

/*********************************************/

#define		CLOCK_FREQ		168000000U

/*********************************************/

#define		TASK_PRIO_LOWEST	5

#define		TASK_STACK_SIZE		240*4//U8,保证4字节对齐(32位机)
#define		EXCEPTION_STACK_SIZE	//unuse

#define		IRQ_STAT_STACK_SIZE	15//U32

#define		MEMORY_POOL_SIZE	(100*1024/4)//U32

#define		PIPE_PRINT_SIZE		150//U8

#define		RESOLVER_TYPE		0//1 -- 使用电脑，0 -- 使用手机app	

/*********************************************/





/*********************************************/

//#define		DEBUG_TICK
//#define		DEBUG_SUSPEND
//#define			DEBUG_SCHEDULE

//#define		DEBUG_MUTEX

//#define			DEBUG_DNDQUEUE
#define			DEBUG_RESOLVER


#endif
