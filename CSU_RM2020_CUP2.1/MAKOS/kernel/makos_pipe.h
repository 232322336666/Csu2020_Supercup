#ifndef _MAKOS_PIPE_H
#define _MAKOS_PIPE_H

#include "makos_kernel_includes.h"

typedef	struct	_pipe_t 
{
	U16	size;
	U8*	start;
	
	U8*	write;
	U8*	read;
	VU16	length;
}pipe_t, *p_pipe;



void	pipe_init(p_pipe pp, U16 sz);
void	pipe_write(p_pipe pp, U8 data);
U8	pipe_read(p_pipe pp);

#endif
