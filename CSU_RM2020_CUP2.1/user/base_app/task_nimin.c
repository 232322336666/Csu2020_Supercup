#include "task_nimin.h"
#include "data_fifo.h"
#include "communicate_nimin.h"
#include "nimin_info.h"
#include "usart.h"
#include "user_usart.h"

#define RX_FIFO_BUFLEN 500
fifo_s_t rx_fifo;
static uint8_t rx_buf[RX_FIFO_BUFLEN]; //����ϵͳfifo�����ݻ�����

#define NIMIN_FIFO_BUFLEN 500
static unpack_data_nimin_t nimin_unpack_obj;
static fifo_s_t nimin_fifo;
static uint8_t nimin_buf[NIMIN_FIFO_BUFLEN]; //����ϵͳfifo�����ݻ�����

dnode_t dhead_nimin_send_que;
nimin_receiver_f nimin_receiver_func;

void nimin_send_que_init(void)
{
    dndqueue_init(&dhead_nimin_send_que);
}

void nimin_send_que_add(void *p_data, nimin_send_type_t type)
{
    p_nimin_send_que pr;

    pr = (p_nimin_send_que)mymalloc(SRAMIN, sizeof(nimin_send_que_t));
    pr->type = type;
    pr->p_param = p_data;
    dndqueue_add_last(&dhead_nimin_send_que, &pr->dnd);
}

static U32 Float2U32(float data) ///////???32?
{
    u8 *point;
    point = (u8 *)(&data);
    return (((u32)point[3] << 24) | ((u32)point[2] << 16) | ((u32)point[1] << 8) | ((u32)point[0]));
}

/*
@func��
    ��Ҫ���͵�����������һ�� -> ͨ��֡�е����ݲ���
@return 
    ���ݷ��͵Ĵ�С(�ֽ�)
*/
u8 pack_nimin_send_buff(u8 *data_buff)
{
    p_dnode curr;
    p_nimin_send_que pr;
    u16 temp_u16;
    u32 temp_u32;
    s32 temp_s32;
    u8 index = 0;

    curr = dhead_nimin_send_que.next;
    while (curr != &dhead_nimin_send_que) //��������resolve
    {
        pr = CONTAINER_OF(curr, nimin_send_que_t, dnd);

        switch (pr->type)
        {
        case type_u8:
            data_buff[index++] = *(u8 *)(pr->p_param);
            ;
            break;
            //                    case    type_s8:
            //                        data_buff[index++] = *(s8 *)(pr->p_param);
            //                        ;break;
        case type_u16:
            temp_u16 = *(u16 *)(pr->p_param);
            data_buff[index++] = (u8)(temp_u16 >> 8);
            data_buff[index++] = (u8)(temp_u16 & 0xff);
            ;
            break;
        case type_s16:
            temp_u16 = *(s16 *)(pr->p_param);
            data_buff[index++] = (u8)(temp_u16 >> 8);
            data_buff[index++] = (u8)(temp_u16 & 0xff);
            ;
            break;
        case type_u32:
            temp_u32 = *(u32 *)(pr->p_param);
            data_buff[index++] = (u8)(temp_u32 >> 24);
            data_buff[index++] = (u8)(temp_u32 >> 16);
            data_buff[index++] = (u8)(temp_u32 >> 8);
            data_buff[index++] = (u8)(temp_u32 & 0xff);
            ;
            break;
        case type_s32:
            temp_s32 = *(s32 *)(pr->p_param);
            data_buff[index++] = (u8)(temp_s32 >> 24);
            data_buff[index++] = (u8)(temp_s32 >> 16);
            data_buff[index++] = (u8)(temp_s32 >> 8);
            data_buff[index++] = (u8)(temp_s32 & 0xff);
            ;
            break;
        case type_float:
            temp_u32 = Float2U32(*(float *)(pr->p_param));
            data_buff[index++] = (u8)(temp_u32 >> 24);
            data_buff[index++] = (u8)(temp_u32 >> 16);
            data_buff[index++] = (u8)(temp_u32 >> 8);
            data_buff[index++] = (u8)(temp_u32 & 0xff);
            ;
            break;
        }
        curr = curr->next;
    }
    return index;
}

void nimin_send_wave()
{
    static u8 send_buff[36];
    static u8 data_buff[32];
    u8 i = 0;
    u8 sum = 0;
    u8 total_length;
    u8 data_length;

    data_length = pack_nimin_send_buff(data_buff);

    send_buff[0] = 0x88;
    send_buff[1] = 0xA1;
    send_buff[2] = data_length;
    for (i = 0; i < data_length; i++)
        send_buff[3 + i] = data_buff[i];

    for (i = 0; i < data_length + 3; i++)
        sum += send_buff[i];
    send_buff[data_length + 3] = sum;

    total_length = data_length + 4;
    usart3_send_string(send_buff, total_length);
}

//*****************receiver**********************
void nimin_rx_callback_set(nimin_receiver_f user_func)
{
    nimin_receiver_func = user_func;
}

void task_nimin(U32 param)
{
    u8 send_tick = 0;

    nimin_send_que_init();
    fifo_s_init(&rx_fifo, rx_buf, RX_FIFO_BUFLEN);
    fifo_s_init(&nimin_fifo, nimin_buf, NIMIN_FIFO_BUFLEN);

    /* initial nimin data unpack object */
    nimin_unpack_obj.data_fifo = &nimin_fifo;
    nimin_unpack_obj.p_header = (frame_header_t *)nimin_unpack_obj.protocol_packet;
    nimin_unpack_obj.index = 0;
    nimin_unpack_obj.data_len = 0;
    nimin_unpack_obj.unpack_step = STEP_HEADER1;

    while (1)
    {
        //�������fifoת���������ݴ�fifo
        fifo_trans(&nimin_fifo, &rx_fifo);
        //����fifo
        unpack_fifo_data_nimin(&nimin_unpack_obj, DN_REG_ID);

        send_tick++;
        if (send_tick == SEND_T)
        {
            send_tick = 0;
            nimin_send_wave();
        }
        task_delay_ms(1);
    }
}

