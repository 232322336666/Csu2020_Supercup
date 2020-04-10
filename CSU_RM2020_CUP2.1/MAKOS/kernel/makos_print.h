#ifndef _MAKOS_PRINT_H
#define _MAKOS_PRINT_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"

#include "makos_pipe.h"

#include "makos_record.h"


#define	PRINT(format, ...)							\
	do									\
	{									\
		irq_close();							\
		print_flag = 1;							\
		printf(format, ##__VA_ARGS__);					\
		print_flag = 0;							\
		irq_restore();							\
	}while(0)


#define	PRINT_ERROR(format, ...)						\
	do									\
	{									\
		irq_close();							\
		system_param.use_print = 0;					\
		printf("error: ");						\
		printf(format, ##__VA_ARGS__);					\
		printf("\r\nFILE: %s, LINE: %d\r\n", __FILE__, __LINE__);	\
		while(1);							\
	}while(0)


#define	PRINT_RECORD(format, ...)						\
	do									\
	{									\
		irq_close();							\
		record_flag = 1;						\
		printf(format, ##__VA_ARGS__);					\
		record_flag = 0;						\
		irq_restore();							\
	}while(0)




extern	U8	print_flag;
extern	dnode_t	dhead_print;


void	task_PRINT(void* param);






#endif
