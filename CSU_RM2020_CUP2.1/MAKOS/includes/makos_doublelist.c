#include "makos_doublelist.h"
#include "stdio.h"
#include "makos_print.h"


void	dnode_head_init(p_dnode hd)
{
	/*
	if(hd)
		PRINT_ERROR("p_dnode points to null");
	if(hd->next)
		PRINT_ERROR("dnode has already been inited");
	*/
	hd->next = hd;
	hd->prev = hd;
}


void	dnode_clear(p_dnode nd)
{
	nd->next = NULL;
	nd->prev = NULL;
}


void	dnode_add_first(p_dnode hd, p_dnode nd)
{
	/*
	if(!hd || !nd)
		PRINT_ERROR("p_dnode points to null");
	*/
	hd->next->prev	= nd;
	nd->next	= hd->next;
	nd->prev	= hd;
	hd->next	= nd;
}


/*
	@param:
		hd:前一个双向链表节点
		nd:后一个双向链表节点
	@fun:
		插入当前节点至倒数第二个
*/
void 	dnode_add_last(p_dnode hd, p_dnode nd)
{
	/*
	if(!hd || !nd)
		PRINT_ERROR("p_dnode points to null");
	*/
	hd->prev->next	= nd;
	nd->prev	= hd->prev;
	nd->next	= hd;
	hd->prev	= nd;
}


void	dnode_remove(p_dnode nd)
{
	/*
	if(!nd || !nd->next)
		PRINT_ERROR("p_dnode points to null");
	*/
	nd->next->prev	= nd->prev;
	nd->prev->next	= nd->next;
	nd->next = NULL;
	nd->prev = NULL;
}

U8	dnode_inlist(p_dnode hd, p_dnode nd)
{
	p_dnode curr;
	
	curr = hd->next;
	while(curr != hd)
	{
		if(curr == nd)
			return 1;
		curr = curr->next;
	}
	return 0;
}

U8	dnode_get_count(p_dnode hd)
{
	U8 res;
	p_dnode curr;
	
	res = 0;
	curr = hd->next;
	while(curr != hd)
	{
		res++;
		curr = curr->next;
	}
	return res;
}







