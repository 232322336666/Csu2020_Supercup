#include "nimin_info.h"
#include "usart.h"
#include "user_usart.h"
#include "task_nimin.h"
#include "string.h"
/*
@file:
    ���ڽ����ܵ���ͨ��֡ת��Ϊ�û�����
    ��λ��������
*/

nimin_data_temp_t nimin_data_temp;
nimin_data_t nimin_data;

void nimin_data_proc(pid_data_p p_data1, pid_data_temp_p p_data2)
{
    p_data1->p = (u16)p_data2->p_h << 8 | p_data2->p_l;
    p_data1->i = (u16)p_data2->i_h << 8 | p_data2->i_l;
    p_data1->d = (u16)p_data2->d_h << 8 | p_data2->d_l;
}

void nimin_data_handler(uint8_t *p_frame)
{
    uint8_t *data_addr = p_frame + 4; //����֡�

    //��ȡ3��PID
    memcpy(&nimin_data_temp.pid_data1, data_addr, 6);
    memcpy(&nimin_data_temp.pid_data2, data_addr + 6, 6);
    memcpy(&nimin_data_temp.pid_data3, data_addr + 12, 6);

    nimin_data_proc(&nimin_data.pid_data1, &nimin_data_temp.pid_data1);
    nimin_data_proc(&nimin_data.pid_data2, &nimin_data_temp.pid_data2);
    nimin_data_proc(&nimin_data.pid_data3, &nimin_data_temp.pid_data3);

    nimin_receiver_func(nimin_data);
}
