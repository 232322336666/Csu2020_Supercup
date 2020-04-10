#include "makos_critical.h"

U32	irq_stat[IRQ_STAT_STACK_SIZE];
U8	irq_pos;

void	irq_close(void)
{
	irq_stat[irq_pos] = __get_PRIMASK();//PRIMASK,单一寄存器，置1关闭所有可屏蔽中断，默认为0
	__disable_irq();
	irq_pos++;
	if(irq_pos > IRQ_STAT_STACK_SIZE)
		printf("irq closed too much\r\n");
}

void	irq_restore(void)
{
	if(irq_pos == 0)
		printf("irq restored too much\r\n");
	irq_pos--;
	__set_PRIMASK(irq_stat[irq_pos]);
}








