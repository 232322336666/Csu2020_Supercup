#include "makos_semaphore.h"
#include "makos_memory.h"
#include "makos_print.h"

void	sem_init(p_sem psem, U32 cnt)
{
	if(psem->dhead_wait.next)
		PRINT_ERROR("sem has already been inited");
	
	psem->count	= cnt;
	
	dnode_head_init(&psem->dhead_wait);//��ʼ�����źŵ�����ȴ��б�
}


void	sem_ask(p_sem psem)//�����ź���,����
{
	retry:
	irq_close();//�ٽ���
	
	if(psem->dhead_wait.next == NULL)//δ����ʼ��
	{
		task_delay_ms(0);
		irq_restore();
		goto retry;
	}
	
	if(psem->count)
	{
		psem->count--;
		if(psem->dhead_wait.next == &ptcb_curr->dnd_sus)//�Ӹ��ź����ĵȴ������б����Ƴ�������ڵ�
			dnode_remove(&ptcb_curr->dnd_sus);
		irq_restore();
	}
	else
	{
		if(ptcb_curr->dnd_sus.next == NULL)//�����񲻴��������ȴ��б���
			dnode_add_last(&psem->dhead_wait, &ptcb_curr->dnd_sus);//������ź���������ȴ��б�
		task_suspend(ptcb_curr);
		irq_restore();
		goto retry;//����������ֱ�����ź������ͷ�
	}
}

void	sem_release(p_sem psem)
{
	irq_close();
	psem->count++;
	if(psem->dhead_wait.next != &psem->dhead_wait)//���������ڵȴ����ź���
	{
		p_TCB ptcb = CONTAINER_OF(psem->dhead_wait.next, TCB_t, dnd_sus);
		//dnode_remove(psem->dhead_wait.next);
		task_resume(ptcb);//�ָ��ȴ�������
	}
	irq_restore();
}














