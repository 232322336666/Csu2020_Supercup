#include "makos_pipe.h"
#include "makos_memory.h"
#include "makos_print.h"

void	pipe_init(p_pipe pp, U16 sz)
{
	if(pp->start)
		PRINT_ERROR("pipe has already been inited\r\nor you defined a pipe object instead of a pointer");
	
	pp->size	= sz;
	pp->start	= (U8*)mymalloc(SRAMIN,sz);
	
	pp->write	= pp->start;
	pp->read	= pp->start;
	pp->length	= 0;
}

void	pipe_write(p_pipe pp, u8 data)
{
	retry:
	if(pp->start == NULL)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	pp->length++;
	*pp->write		= data;
	pp->write++;
	if(pp->write == pp->start+pp->size)
		pp->write	= pp->start;
	if(pp->length > pp->size)
	{
		pp->length	= pp->size;
		pp->read	= pp->write;
	}
}

U8	pipe_read(p_pipe pp)
{
	u8 res;
	
	retry:
	if(pp->start == NULL)
	{
		task_delay_ms(0);
		goto retry;
	}
	
	if(!pp->length)
		return 0;
	pp->length--;
	res			= *pp->read;
	pp->read++;
	if(pp->read == pp->start+pp->size)
		pp->read	= pp->start;
	
	return res;
}














