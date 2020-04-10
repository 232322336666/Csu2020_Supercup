#include "makos_rwbuff.h"
#include "makos_memory.h"
#include "makos_print.h"

void	rwbuff_init(p_rwbuff pb, U32 size)
{
	pb->size	= size;
	atomic_set(&pb->count0, 0);
	atomic_set(&pb->count1, 0);
	pb->buff_start	= MCB_get(size*2);
}

void	rwbuff_write(p_rwbuff pb, void* data)
{
	U8*	pwrite;
	U32	i;
	
	if(pb->buff_start == NULL)
		PRINT_ERROR("rwbuff needs initiation");
	
	if(atomic_get(&pb->count0) == 0)
		pwrite	= pb->buff_start;
	else
		pwrite	= pb->buff_start + pb->size;
	
	for(i=0; i < pb->size; i++)
		*(pwrite++)	= *(((U8*)data)++);
	
}

void	rwbuff_read(p_rwbuff pb, void* data)
{
	U8*	pread;
	U32	i;
	U32	flag;
	
	if(pb->buff_start == NULL)
		PRINT_ERROR("rwbuff needs initiation");
	
	changed:
	
	if(pb->write == 0)
		pread	= pb->buff_start + pb->size;
	else
		pread	= pb->buff_start;
	
	for(i=0; i < pb->size; i++)
		*(((U8*)data)++) = *(pread++);
	
	if(pb->write != flag)
}













