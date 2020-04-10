/*
	FILE NAME:	task_remote.c
	DESCRIPTION:	the task dispose remote messages by the control rules
			and send the result to the relatively tasks
	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2017/11/6-21:25
	MODIFICATION:	built the frame of remote task, simply send the msg to chassis and holder.
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/3/30
	MODIFICATION:	rewrote the shit code of receiver, add and change the romoting logic.
	---------
*/

#include "task_remote.h"
#include "task_init.h"
#include "dbus.h"
#include "task_holder.h"
#include "usart.h"
#include "timer.h"

#define	CHASSIS_FOLLOW_ENABLE

#define	SP_F	(400U)
#define	SP_R	(SP_F)
#define	SP_C	(SP_F)

#define	SPH_F	(600U)
#define	SPH_R	(SPH_F)
#define	SPH_C	(SPH_F)

#define	SPL_F	(100U)
#define	SPL_R	(SPL_F)
#define	SPL_C	(SPL_F)


msg_t	chassis_msg;
msg_t	holder_msg;
msg_t	shoot_msg;

msg_t	remote_msg;
msg_t	remotekf_msg;
unsigned char magazine = 0;

void	remote_send_msg_init(void)
{
	msg_init(&remote_msg,	1,	sizeof(remote_data_t));
	msg_init(&remotekf_msg,	1,	sizeof(remote_data_t));
	msg_init(&chassis_msg,	4,	sizeof(chassis_ctrl_t));
	msg_init(&holder_msg,	4,	sizeof(holder_ctrl_t));
}


void	remote_data_dispose(p_remote_data tg)
{
	tg->JR_LR	= (U16)((remote_buff[0]		| (remote_buff[1]<<8))				& 0x07ff);
	tg->JR_UD	= (U16)(((remote_buff[1]>>3)	| (remote_buff[2]<<5))				& 0x07ff);
	tg->JL_LR	= (U16)(((remote_buff[2]>>6)	| (remote_buff[3]<<2)	| (remote_buff[4]<<10))	& 0x07ff);
	tg->JL_UD	= (U16)(((remote_buff[4]>>1)	| (remote_buff[5]<<7))				& 0x07ff);
	tg->SL		= (U8)	((remote_buff[5]>>6)							& 0x03);
	tg->SR		= (U8)	((remote_buff[5]>>4)							& 0x03);
	
	tg->LL		= (U16)((remote_buff[16] | (remote_buff[17]<<8))&0x07FF);
	
	tg->MX		= (U16)(remote_buff[6]		| (remote_buff[7]<<8));
	tg->MY		= (U16)(remote_buff[8]		| (remote_buff[9]<<8));
	tg->MCL		= (U8)	remote_buff[12];
	tg->MCR		= (U8)	remote_buff[13];
	
	tg->KEY		= (U16)(remote_buff[14]		| (remote_buff[15]<<8));
	
	tg->KEY_data.Key_W=_K_CHK(W);
	tg->KEY_data.Key_A=_K_CHK(A);
	tg->KEY_data.Key_S=_K_CHK(S);
	tg->KEY_data.Key_D=_K_CHK(D);
	tg->KEY_data.Key_Q=_K_CHK(Q);
	tg->KEY_data.Key_E=_K_CHK(E);
	tg->KEY_data.Key_SHIFT=_K_CHK(SHIFT);
	tg->KEY_data.Key_CTRL=_K_CHK(CTRL);
	tg->KEY_data.Key_R=_K_CHK(R);
	tg->KEY_data.Key_F=_K_CHK(F);
	tg->KEY_data.Key_G=_K_CHK(G);
	tg->KEY_data.Key_Z=_K_CHK(Z);
	tg->KEY_data.Key_X=_K_CHK(X);
	tg->KEY_data.Key_C=_K_CHK(C);
	tg->KEY_data.Key_V=_K_CHK(V);
	tg->KEY_data.Key_B=_K_CHK(B);
}

void	remote_handle(void)
{
	irq_close();
	if(dbus_buff_checked())
	{
		p_remote_data	data = msg_get_write_some(&remote_msg);
		remote_data_dispose(data);
		msg_write_finish_some(&remote_msg);
	}
	irq_restore();
}

static
void	memcopy(void* target, void* source, U32 size)
{
	U32	i;
	U8	*pt = target;
	U8	*ps = source;
	for(i=0; i<size; i++){
		*pt	= *ps;
		pt++;
		ps++;
	}
}
void	task_remotesome1(void* param)
{
	p_remote_data	data;
	S16 temp,temp_key;
	while(1)
	{
		data=msg_get_read_some(&remote_msg);
		if(data!=NULL)
		{
			temp=((S16)data->LL)-1024;
			temp_key=data->KEY;
//			printf("%d\r\n",canrate.send.dbus);
//			printf("remote1:%d\r\n",temp);
//			remote_test(data);
//			print_wave(8,1,&data->KEY_data.Key_W,&data->KEY_data.Key_A,&data->KEY_data.Key_S,&data->KEY_data.Key_D,&data->KEY_data.Key_Q,&data->KEY_data.Key_E,&data->KEY_data.Key_SHIFT,&data->KEY_data.Key_CTRL);
			msg_read_finish_some(&remote_msg);
		}
		data=msg_get_read_some(&remotekf_msg);
		if(data!=NULL)
		{
//			print_wave(2,1,&data->SL,&data->SR);
			msg_read_finish_some(&remotekf_msg);
		}
		task_delay_ms(5);
	}
}
void	task_remotesome2(void* param)
{
	p_remote_data	data;
	S16 temp;
	while(1)
	{
		data=msg_get_read_some(&remote_msg);
		if(data!=NULL)
		{
			temp=((S16)data->LL)-1024;
//			printf("remote2:%d\r\n",temp);
//			remote_test(data);
			msg_read_finish_some(&remote_msg);
		}
		data=msg_get_read_some(&remotekf_msg);
		if(data!=NULL)
		{
//			print_wave(2,1,&data->SL,&data->SR);
			msg_read_finish_some(&remotekf_msg);
		}
		task_delay_ms(5);
	}
}
void	task_remotesome3(void* param)
{
	p_remote_data	data;
	S16 temp;
	if(temp);
	while(1)
	{
		data=msg_get_read_some(&remote_msg);
		if(data!=NULL)
		{
			temp=((S16)data->LL)-1024;
//			printf("remote3:%d\r\n",temp);
//			remote_test(data);
			msg_read_finish_some(&remote_msg);
		}
		data=msg_get_read_some(&remotekf_msg);
		if(data!=NULL)
		{
//			print_wave(2,1,&data->SL,&data->SR);
			msg_read_finish_some(&remotekf_msg);
		}
		task_delay_ms(5);
	}
}

remote_data_t	task_remote_buff;
uint32_t remote_connect = 0;

void	task_remote(void* param)
{
	p_remote_data	data;
	
	dbus_init(1, remote_handle);
	
//	task_insert(task_remotesome1, NULL, 3);
//	task_insert(task_remotesome2, NULL, 3);
//	task_insert(task_remotesome3, NULL, 3);
	
	while(1)
	{
		data	= msg_get_read_some(&remote_msg);
		if(data)
		{
			irq_close();
			memcopy(&task_remote_buff, data, sizeof(remote_data_t));
			msg_read_finish_some(&remote_msg);
			
			remote_KF(&task_remote_buff);
			
//			remote_test(&task_remote_buff);
			irq_restore();
			remote_connect = 0;
		}else{
			remote_connect++;
			if(remote_connect == 1500)
			{
				remote_connect = 0;
				dbus_init(1, remote_handle);
			}
		}
		task_delay_ms(0);
	}
}


U32	mode_flag = CHASSIS_NOHEAD;
u8 mood = 0;
u8 key_down_G = 0;
u8 key_down_G_B = 0;
u8 key_down_G_B_SHIFT = 0;
uint8_t VS_TEST;


S16	remote_mouse_x	= 0;
S16	remote_mouse_y	= 0;

U32	shoot_flag	= SHOOT_HOLD;
U32	shoot_single	= 0;
U32 last_state,last_state2;

float temp_test = 5.5f;

void shoot_speed_flag(S16 tar_speed,u8 shoot_single_G,u8 shoot_multi_G);//拨弹电机控制上传
static u8 flag_continuou_MCL;
static u8 stop_count=0;
static u8 mcl[shoot_mode_buffer];


U32	receiver_print		= 0;
U32	receiver_print_mark	= 0;
void	receiver_control(void * p_param)
{
	U32 param = TO_U32(p_param);
	
	if(param <= 2)
		receiver_print	= param;
	else if(param == 3)
	{
		remote_mouse_x	= 0;
		remote_mouse_y	= 0;
	}
}

void remote_test(p_remote_data data)
{
	U16 temp_SL,temp_SR;
	temp_SL=(U16)data->SL;
	temp_SR=(U16)data->SR;
	print_wave(8,2,&data->JL_LR,&data->JL_UD,&data->JR_LR,&data->JR_UD,&data->LL,&temp_SL,&temp_SR,&data->KEY);
}

void remote_KF(p_remote_data origin_data)
{
	p_remote_data data = msg_get_write_some(&remotekf_msg);
	/*一些卡尔曼滤波处理，先占坑
	
	*/
	data->KEY=origin_data->KEY;
	data->KEY_data=origin_data->KEY_data;
	data->SL=origin_data->SL;
	data->SR=origin_data->SR;
	data->MCL=origin_data->MCL;
	data->MCR=origin_data->MCR;
	msg_write_finish_some(&remotekf_msg);
}
