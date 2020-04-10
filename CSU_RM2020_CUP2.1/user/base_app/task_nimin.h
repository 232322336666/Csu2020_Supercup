#ifndef _TASK_NIMIN_H
#define _TASK_NIMIN_H

#include "makos_includes.h"
#include "data_fifo.h"
#include "nimin_info.h"

#define SEND_T 10 //��������,ms

typedef void (*nimin_receiver_f)(nimin_data_t nimin_data);
extern nimin_receiver_f nimin_receiver_func;

typedef enum
{
    type_u8,
    type_s8,
    type_u16,
    type_s16,
    type_u32,
    type_s32,
    type_float
} nimin_send_type_t;

typedef struct _nimin_send_que_t
{
    dnode_t dnd;
    void *p_param;
    nimin_send_type_t type;
} nimin_send_que_t, *p_nimin_send_que;

extern fifo_s_t rx_fifo;

void nimin_send_que_add(void *p_data, nimin_send_type_t type);
void nimin_rx_callback_set(nimin_receiver_f user_func);
void task_nimin(U32 param);

#endif
