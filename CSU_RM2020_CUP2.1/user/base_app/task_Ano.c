/*********************(C) COPYRIGHT 2019 Du Yangyi *****************

			* author    :   Du Yangyi
			* function  :   ANO Data Transmission
			* version   :   ANO V5.0

**************************************************************/
#include "task_Ano.h"

/////////////////////////////////////////////////////////////////////////////////////
//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )
#define PID_Param 1
dt_flag_t f;					//需要发送数据的标志
u8 data_to_send[50];	//发送数据缓存
u8 checkdata_to_send, checksum_to_send;

float test_temp_kp=5;
/* *    任务    任务     任务    *  */
void task_ANO(void* param)
{
    while(1)
    {
        ANO_DT_Data_Exchange();
        task_delay_ms(5);
    }
}
//////////////////////////////////////


void ANO_DT_Send_Data(u8 *dataToSend, u8 length)
{
    Usart_Send(data_to_send, length);
}

/***************************************************

       *功能 ：发送校验帧
       *参数 ：帧头

***************************************************/
static void ANO_DT_Send_Check(u8 head, u8 check_sum)
{
    data_to_send[0] = 0xAA;
    data_to_send[1] = 0xAA;
    data_to_send[2] = 0xEF;
    data_to_send[3] = 2;
    data_to_send[4] = head;
    data_to_send[5] = check_sum;

    u8 i;
    u8 sum = 0;
    for(i = 0; i < 6; i++)
        sum += data_to_send[i];
    data_to_send[6] = sum;

    ANO_DT_Send_Data(data_to_send, 7);
}

/***************************************************

       *功能 ：发送数据
       *参数 ：无
       *注意 ：仿照PID1的模式改变参数进行发送数据

***************************************************/
void ANO_DT_Data_Exchange(void)
{
    if(f.send_check)
    {
        f.send_check = 0;
        ANO_DT_Send_Check(checkdata_to_send, checksum_to_send);
    }
    else if(f.send_pid1)
    {
        f.send_pid1 = 0;
//        ANO_DT_Send_PID(1,	kp1,ki1,kd1,		kp2,ki2,kd2,       kp3,ki3,kd3);
    }
    else if(f.send_pid2)
    {
        f.send_pid2 = 0;
    }
    else if(f.send_pid3)
    {
        f.send_pid3 = 0;

    }
    else if(f.send_pid4)
    {
		 f.send_pid4 = 0;
    }
    else if(f.send_pid5)
    {
		 f.send_pid5 = 0;
    }
    else if(f.send_pid6)
    {
		 f.send_pid6 = 0;
    }
    ANO_DT_Data_Receive_Anl_Task();
}

/***************************************************

       *功能 ：向上位机发送PID信息
       *参数 ：一次三组PID的数据
       *注意 ： 1. group最高可到6，也就是可以最多发送6套PID共18组PID数据
                2. PID参数乘以一个放大系数在宏定义PID_Param里
***************************************************/
void ANO_DT_Send_PID(u8 group, float p1_p, float p1_i, float p1_d, float p2_p, float p2_i, float p2_d, float p3_p, float p3_i, float p3_d)
{
    u8 _cnt = 0;
    vs16 _temp;

    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0x10 + group - 1;
    data_to_send[_cnt++] = 0;


    _temp = p1_p * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p1_i  * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p1_d  * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p2_p  * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p2_i  * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p2_d * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p3_p  * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p3_i  * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p3_d * PID_Param;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);

    data_to_send[3] = _cnt - 4;

    u8 sum = 0;
    u8 i;
    for(i = 0; i < _cnt; i++)
        sum += data_to_send[i];

    data_to_send[_cnt++] = sum;

    ANO_DT_Send_Data(data_to_send, _cnt);
}

static u8 DT_RxBuffer[50], DT_data_cnt = 0, ano_dt_data_ok;
void ANO_DT_Data_Receive_Anl_Task()
{
    if(ano_dt_data_ok)
    {
        ANO_DT_Data_Receive_Anl(DT_RxBuffer, DT_data_cnt + 5);
        ano_dt_data_ok = 0;
    }
}

/***************************************************

       *功能 ：接收预解析
       *参数 ：一个字节数据
       *注意 ： 每次接收一个字节数据调用一次

***************************************************/
void ANO_DT_Data_Receive_Prepare(u8 data)
{
    static u8 _data_len = 0;
    static u8 state = 0;

    if(state == 0 && data == 0xAA)
    {
        state = 1;
        DT_RxBuffer[0] = data;
    }
    else if(state == 1 && data == 0xAF)
    {
        state = 2;
        DT_RxBuffer[1] = data;
    }
    else if(state == 2 && data < 0XF1)
    {
        state = 3;
        DT_RxBuffer[2] = data; //功能字
    }
    else if(state == 3 && data < 50)
    {
        state = 4;
        DT_RxBuffer[3] = data; //长度
        _data_len = data;
        DT_data_cnt = 0;
    }
    else if(state == 4 && _data_len > 0)
    {
        _data_len--;
        DT_RxBuffer[4 + DT_data_cnt++] = data; //数据
        if(_data_len == 0)
            state = 5;
    }
    else if(state == 5)
    {
        state = 0;
        DT_RxBuffer[4 + DT_data_cnt] = data; //SUM校验帧
        ano_dt_data_ok = 1;
    }
    else
        state = 0;
}


/***************************************************

       *功能 ：接收一个数据帧
       *参数 ：数据帧，数据帧长度
       *注意 ：仿照PID1的模式接收PID数据

***************************************************/
void ANO_DT_Data_Receive_Anl(u8 *data_buf, u8 num)
{
    u8 sum = 0;
    u8 i;
    for(i = 0; i < (num - 1); i++)
        sum += *(data_buf + i);
    if(!(sum == *(data_buf + num - 1)))		return;		//判断sum
    if(!(*(data_buf) == 0xAA && *(data_buf + 1) == 0xAF))		return;		//判断帧头

    if(*(data_buf + 2) == 0X02)
    {
        if(*(data_buf + 4) == 0X01)
        {
            f.send_pid1 = 1;
            f.send_pid2 = 1;
            f.send_pid3 = 1;
            f.send_pid4 = 1;
            f.send_pid5 = 1;
            f.send_pid6 = 1;
        }

        if(*(data_buf + 4) == 0XA1)		//恢复默认PID
        {

        }
    }

    if(*(data_buf + 2) == 0X10)								//PID1
    {
        /*Kp  = 1*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
          	Ti  = 1*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
          	Td  = 1*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
        	  Kp = 1*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
          	Ti = 1*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
          	Td = 1*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
          	Kp 	= 1*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
          	Ti 	= 1*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
        		Td	= 1*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );*/

		
        if(f.send_check == 0)
        {
            f.send_check = 1;
            checkdata_to_send = *(data_buf + 2);
            checksum_to_send = sum;
        }
    }

    if(*(data_buf + 2) == 0X11)								//PID2
    {

		
        if(f.send_check == 0)
        {
            f.send_check = 1;
            checkdata_to_send = *(data_buf + 2);
            checksum_to_send = sum;
        }
    }

    if(*(data_buf + 2) == 0X12)								//PID3
    {


        if(f.send_check == 0)
        {
            f.send_check = 1;
            checkdata_to_send = *(data_buf + 2);
            checksum_to_send = sum;
        }
    }
    if(*(data_buf + 2) == 0X13)								//PID4
    {


        if(f.send_check == 0)
        {
            f.send_check = 1;
            checkdata_to_send = *(data_buf + 2);
            checksum_to_send = sum;
        }
    }

    if(*(data_buf + 2) == 0X14)								//PID5
    {


        if(f.send_check == 0)
        {
            f.send_check = 1;
            checkdata_to_send = *(data_buf + 2);
            checksum_to_send = sum;
        }
    }

    if(*(data_buf + 2) == 0X15)								//PID6
    {
		
		
        if(f.send_check == 0)
        {
            f.send_check = 1;
            checkdata_to_send = *(data_buf + 2);
            checksum_to_send = sum;
        }
    }
}
