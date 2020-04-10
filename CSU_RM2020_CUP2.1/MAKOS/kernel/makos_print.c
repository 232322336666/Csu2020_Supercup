#include "makos_print.h"
#include "makos_dnode_queue.h"

#include "usart.h"
#include "user_usart.h"

U8	print_flag;
dnode_t	dhead_print;


void	task_PRINT(void* param)
{
	p_dnode curr;
	p_TCB	ptcb;
	p_pipe	ppipe;
	
	curr			= dhead_print.next;
	
	while(1)
	{
		if(curr != &dhead_print)
		{
			ptcb	= CONTAINER_OF(curr, TCB_t, dnd_print);
			ppipe	= &ptcb->pipe_print;
			
			//if(ppipe->length)
			//	USART1->DR = '#';
			send_not_over:
			if(ppipe->length)
			{
				retry:
				if((pserial->SR&0X40)==0)
				{
					task_delay_ms(0);
					goto retry;
				}
				pserial->DR = (U8)(*ppipe->read);
				
				ppipe->read++;
				if(ppipe->read ==  ppipe->start+ppipe->size)
					ppipe->read = ppipe->start;
				ppipe->length--;
				
				goto send_not_over;
			}
		}
		else
		{
			task_delay_ms(0);
		}
		curr = curr->next;
	}
}
