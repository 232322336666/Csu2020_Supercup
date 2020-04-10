#include "user_usart.h"
#include "irq_dead_check.h"
#include "usart.h"
#include "can.h"
#include "user_can.h"
#include "task_holder.h"
#include "makos_print.h"
#include "makos_resolver.h"
#include "task_nimin.h"
#include "data_fifo.h"
#include "communicate_nimin.h"
#include "nimin_info.h"
#include "stdarg.h"
#include "task_vision.h"

extern u8 VS_Rx_Pos;                           // 用于记录接收缓存的位置（视觉通信）
//extern u8 VS_rx_Msg[vs_rxbuf_len];           //存储 PC->Robot 视觉信息
u8 VS_rx_Msg[8];
extern u8 visual_Enable;

IRQ_CHECK_DEFINE(usart1);
void	USART1_IRQHandler(void)
{
	IRQ_CHECK(usart1);
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		uif1();
	}
}

u8 TxCounter = 0;
void	USART2_IRQHandler(void)
{
	u8 com_data;
	if ( USART2->SR & USART_SR_ORE ) //ORE中断（溢出中断）
    {
        com_data = USART2->DR;
    }

    //接收中断
    if ( USART_GetITStatus ( USART2, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART2, USART_IT_RXNE ); //清除中断标志

        com_data = USART2->DR;
		//	printf("data = %x\n",com_data);
        ANO_DT_Data_Receive_Prepare ( com_data ); //接收处理
    }
    //发送（进入移位）中断
    if ( USART_GetITStatus ( USART2, USART_IT_TXE ) )
    {

        USART2->DR = TxBuffer[TxCounter++]; //写DR清除中断标志
        if ( TxCounter == count )
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }
    }
}

void	USART3_IRQHandler(void)
{
	u8 com_data;
	if ( USART3->SR & USART_SR_ORE ) //ORE中断（溢出中断）
    {
        com_data = USART3->DR;
    }

    //接收中断
    if ( USART_GetITStatus ( USART3, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART3, USART_IT_RXNE ); //清除中断标志

        com_data = USART3->DR;
		//	printf("data = %x\n",com_data);
        ANO_DT_Data_Receive_Prepare ( com_data ); //接收处理
    }
    //发送（进入移位）中断
    if ( USART_GetITStatus ( USART3, USART_IT_TXE ) )
    {

        USART3->DR = TxBuffer[TxCounter++]; //写DR清除中断标志
        if ( TxCounter == count )
        {
            USART3->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }
    }
}

IRQ_CHECK_DEFINE(usart4);
void	UART4_IRQHandler(void)
{
	IRQ_CHECK(usart4);
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		uif4();
	}
}

IRQ_CHECK_DEFINE(usart5);
void	UART5_IRQHandler(void)
{
	IRQ_CHECK(usart5);
	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART5, USART_IT_RXNE);

		uif5();
	}
}


uint8_t Camera_Uart_Buffer[256];
u8 Target_Flag,Target_Flag_last;
S16 camera_num = 0;

static float x_tar_base,x_tar_base_last,holder_x_target2;
//static float y_tar_base,y_tar_base_last;
static float vision_a,v_last;
float a_moment_x,a_moment_y;
float vision_a_k = 4.4;
float vision_a_k_pitch = 5000;
static float k = 0.250;
static float kf_q3 = 0.3,kf_r3 = 1;

u8 flag_fass2 = 1;

u8 flag_pass_x_base,flag_pass_y_base;
#define max(a, b)			(a>b? a:b)
#define min(a, b)			(a<b? a:b)
#define range(x, a, b)		(min(max(x, a), b))//a-最小值   b-最大值

IRQ_CHECK_DEFINE(usart6);
void	USART6_IRQHandler(void)
{
	static u8 count = 0;
	IRQ_CHECK(usart6);
	if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		Camera_Uart_Buffer[count] = USART_ReceiveData(USART6);	//读取接收到的数据
		
		if(Camera_Uart_Buffer[count] == 0X03 && Camera_Uart_Buffer[count-1] == 0XFC)
		{
			if(Camera_Uart_Buffer[count - 10] == 0X03 && Camera_Uart_Buffer[count - 9] == 0XFC)
			{
				vs_get = 1;
				vs_connect = 1;
				Target_Flag_last = Target_Flag;				
				Target_Flag = Camera_Uart_Buffer[count - 2];
				Camera_Z_offset = (int16_t)(Camera_Uart_Buffer[count - 3] << 8 | Camera_Uart_Buffer[count - 4]);
				Camera_Y_offset = (int16_t)(Camera_Uart_Buffer[count - 5] << 8 | Camera_Uart_Buffer[count - 6]);
				Camera_X_offset = (int16_t)(Camera_Uart_Buffer[count - 7] << 8 | Camera_Uart_Buffer[count - 8]);
								
				count = 0;
				camera_num ++;
				if(Vision_Flag)
				{	
					//********x轴处理**********
					//获取目标位置-模型
					x_tar_base = get_x_tart_base(flag_pass_x_base);
					//获取速度-修正信息
					vision_x_speed = get_speed(x_tar_base,x_tar_base_last);	
					
					vision_a = vision_x_speed - v_last;//加速度
					v_last = vision_x_speed;
					
					if(vision_x_speed > vision_xspeed_limit)
						vision_x_speed = vision_xspeed_limit;
					else if(vision_x_speed < -vision_xspeed_limit)
						vision_x_speed = -vision_xspeed_limit;
					
					a_moment_x = vision_a * vision_a_k;
					
					a_moment_x = range(a_moment_x,-200,200);
					
					vision_x_speed += a_moment_x;
					
					//获取目标位置-修正
					
					if(Target_Flag == 255)//识别到目标
						holder_x_target2 = x_tar_base + vision_x_speed*k;
					else//未识别到目标
						holder_x_target2 = holder_yaw_angle;
					
					if(flag_fass2)
					{
						holder_kalman_init(kf_q3,kf_r3,&vision_yaw_kalman_tar);
						holder_x_target = holder_KalmanFilter_cal(holder_x_target2,&vision_yaw_kalman_tar);
					}
					else
						holder_x_target = holder_x_target2;
					
					x_tar_base_last = x_tar_base;
					
//					//********y轴处理**********
//					y_tar_base = get_y_tart_base();//获取目标位置-模型
//					//获取速度-修正信息
//					vision_y_speed = get_speed_pitch(y_tar_base,y_tar_base_last);
////	
//					v_last_pitch = vision_y_speed;	
//					
//					vision_a_pitch = vision_y_speed - v_last_pitch;
//					
//					if(vision_y_speed > limit)
//						vision_y_speed = limit;
//					else if(vision_y_speed < -limit)
//						vision_y_speed = -limit;
//					
//					vision_y_speed += vision_a_pitch * vision_a_k_pitch;
//					//获取目标位置-修正
//					if(Target_Flag == 255)//识别到目标
//						holder_y_target2 = y_tar_base + vision_y_speed*k;
//					else//未识别到目标
//						holder_y_target2 = holder_pitch_angle;
//					
//						holder_y_target = KalmanFilter2_pitch(holder_y_target2,kf_q3,kf_r3,1);
//					
//					y_tar_base_last = y_tar_base;					
				}
			}
		}		
		count ++;
		if(count == 255)
			count = 0;
	}
}

void usart3_send_char(char ch)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	USART_SendData(USART3,(u8)ch);	
}

void usart3_send_string(u8 *buff, u32 len)
{
    while(len--)
    {
        usart3_send_char(*buff);
        buff++;
    }
}

void uart5_send_char(char ch)
{
	while(USART_GetFlagStatus(UART5,USART_FLAG_TXE)==RESET);
	USART_SendData(UART5,(u8)ch);	
}

void uart5_send_string(u8 *buff, u32 len)
{
    while(len--)
    {
        uart5_send_char(*buff);
        buff++;
    }
}


void usart6_send_char(char ch)
{
	while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);
	USART_SendData(USART6,(u8)ch);	
}

void usart6_send_string(u8 *buff, u32 len)
{
    while(len--)
    {
        usart6_send_char(*buff);
        buff++;
    }
}

void usart2_send_char(char ch)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	USART_SendData(USART2,(u8)ch);	
}

void usart2_send_string(u8 *buff, u32 len)
{
    while(len--)
    {
        usart2_send_char(*buff);
        buff++;
    }
}

#define CMD_WAVE 3	//山外上位机对应的波形指令
uint8_t cmdf[2] = {CMD_WAVE, ~CMD_WAVE};   
uint8_t cmdr[2] = {~CMD_WAVE, CMD_WAVE};
void usart6_send_wave_int16(u8* p,u8 num)
{
	u8 i;
	usart6_send_string(cmdf, sizeof(cmdf));    
			
	for(i = 0;i < num;i++)
	{
		usart6_send_string(p + 2 * i,2);
		//usart6_send_string((uint8_t *)&DataInfo[4],4);
	}		
	usart6_send_string(cmdr, sizeof(cmdr)); 
}

extern  S16	  get_bullet_shoot_speed;
extern 	S16		get_bullet_speed;
extern 	S16		get_bullet_temp;
extern  float	  get_bullet_circle;

extern  S16	  car_spin_engineer_shoot_speed;
extern 	S16		car_spin_engineer_speed;
extern 	S16		car_spin_engineer_temp;
extern  float	  car_spin_engineer_circle;

extern  S16	  car_spin_engineer_shoot_speedFF;
extern 	S16		car_spin_engineer_speedFF;
extern 	S16		car_spin_engineer_tempFF;
extern  float	  car_spin_engineer_circleFF;

extern  S16	  shoot_speed;
extern 	S16		speed;
extern 	S16		temp;
extern  S16	  circle;

//**************???????**************	


s16 SHOOT_CIRCLE_F,SHOOT_CIRCLE_FF,SHOOT_CIRCLE_G;

void send_wave()
{     
//	S16	pos_temp = SHOOT_CIRCLE_F;
//	S16	pos_tar_temp = car_spin_engineer_circle;
//	S16	pos_tempFF = SHOOT_CIRCLE_FF;
//	S16	pos_tar_tempFF = car_spin_engineer_circleFF;
//	
//	S16 pos_get_bullet_temp = SHOOT_CIRCLE_G;
//  S16 pos_get_bullet_tar_temp = get_bullet_circle;
//	
//    usart3_send_string(cmdf, sizeof(cmdf));    
//	
//	/***********取弹************/
//    usart3_send_string((uint8_t *)&get_bullet_shoot_speed,2);                   //速度环实际值
//		usart3_send_string((uint8_t *)&get_bullet_speed,2);                         //速度环输出值        
//		usart3_send_string((uint8_t *)&get_bullet_temp,2);                          //速度环目标值  位置环输出值  
//		
//		usart3_send_string((uint8_t *)&pos_get_bullet_tar_temp,2);                  //位置环目标值    
//		usart3_send_string((uint8_t *)&pos_get_bullet_temp,2);                      //位置环实际值
//	
	/*******上岛*******/
//    usart3_send_string((uint8_t *)&car_spin_engineer_shoot_speed,2);  //速度环实际值
//		usart3_send_string((uint8_t *)&car_spin_engineer_speed,2);        //速度环输出值        
//		usart3_send_string((uint8_t *)&car_spin_engineer_temp,2);         //速度环目标值  位置环输出值  
//		
//		usart3_send_string((uint8_t *)&pos_tar_temp,2);                  //位置环目标值    
//		usart3_send_string((uint8_t *)&pos_temp,2);                      //位置环实际值
		
//    usart3_send_string((uint8_t *)&car_spin_engineer_shoot_speedFF,2);  //速度环实际值
//		usart3_send_string((uint8_t *)&car_spin_engineer_speedFF,2);        //速度环输出值        
//		usart3_send_string((uint8_t *)&car_spin_engineer_tempFF,2);         //速度环目标值  位置环输出值  
//		
//		usart3_send_string((uint8_t *)&pos_tar_tempFF,2);                  //位置环目标值    
//		usart3_send_string((uint8_t *)&pos_tempFF,2);                      //位置环实际值
		
	/*****发弹*******/
//	  usart3_send_string((uint8_t *)&shoot_speed,2);  //速度环实际值
//		usart3_send_string((uint8_t *)&speed,2);        //速度环输出值
//		usart3_send_string((uint8_t *)&temp,2);         //速度环目标值  位置环输出值

//		usart3_send_string((uint8_t *)&circle,2);        //位置环目标值
//		usart3_send_string((uint8_t *)&SHOOT_CIRCLE,2);  //位置环实际值
		
    usart3_send_string(cmdr, sizeof(cmdr));    
		//***********************************
}

#define CMD_WARE     3
/**************
函数功能：向山外上位机输出1到8个通道的波形
形参说明：number：变量数目
		  length：变量大小，以字节为单位
备注：调用示例：print_wave(3,2,(uint8_t *)&send_date_1,(uint8_t *)&send_date_2,(uint8_t *)&send_date_3);
**************/
void print_wave(u8 number, u8 length, ...)
{
    va_list ap;
    u8 temp_number;
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};

    va_start(ap, length);

    usart3_send_string(cmdf, sizeof(cmdf));

    for(temp_number = 0; temp_number < number; temp_number++)
    {
        usart3_send_string(va_arg(ap, u8*), length);
    }

    usart3_send_string(cmdr, sizeof(cmdr));
    va_end(ap);
}

