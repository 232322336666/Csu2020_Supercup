 #ifndef _ANO_H_
 #define _ANO_H_
 #include "usart.h"
 #include "makos_includes.h"
 typedef struct
{
    u8 msg_id;
    u8 msg_data;
    u8 send_check;
    u8 send_pid1;
    u8 send_pid2;
    u8 send_pid3;
    u8 send_pid4;
    u8 send_pid5;
    u8 send_pid6;
} dt_flag_t;
 
void ANO_DT_Data_Exchange(void);
void ANO_DT_Send_PID(u8 group, float p1_p, float p1_i, float p1_d, float p2_p, float p2_i, float p2_d, float p3_p, float p3_i, float p3_d);
void ANO_DT_Data_Receive_Prepare(u8 data);
void ANO_DT_Data_Receive_Anl_Task(void);
void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num);
void task_ANO( void* param);

#endif



