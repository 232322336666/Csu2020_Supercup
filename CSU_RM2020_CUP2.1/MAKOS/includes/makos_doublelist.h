#ifndef _MAKOS_DOUBLELIST_H
#define _MAKOS_DOUBLELIST_H

#include "makos_type.h"

typedef struct _dnode
{
	struct _dnode*	next;
	struct _dnode*	prev;
}dnode_t, *p_dnode;

#define	DHEAD_DEFINE(name)	dnode_t name = {&name, &name};

void	dnode_head_init(p_dnode hd);
void	dnode_clear(p_dnode nd);
void	dnode_add_first(p_dnode hd, p_dnode nd);
void 	dnode_add_last(p_dnode hd, p_dnode nd);
void	dnode_remove(p_dnode nd);
U8	dnode_inlist(p_dnode hd, p_dnode nd);
U8	dnode_get_count(p_dnode hd);

#endif
