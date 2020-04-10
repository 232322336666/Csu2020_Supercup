#include "task_judge.h"
#include "headfile.h"
#include "communicate.h"
#include "judgement_info.h"
#include "task_remote.h"
/*
实验平台:
	RM2019第二版
	DMA_UART收发串口:USART3(接裁判系统)
	调试串口:UART5(输出裁判系统数据,ps:main中改为serial_init(5, 115200))
实验现象：
	裁判系统读取任务每1.5s发送部分裁判系统读取数据，通过观察其大小是否正常来判断解析是否合理	
	串口3输入裁判系统数据数据，串口5返回接收到的数据
author:hhh
date:9/11/2018
底层引用：正点原子DMA

*/

#define JUDGE_FIFO_BUFLEN      500

static unpack_data_t judge_unpack_obj;
static uart_dma_rxdata_t judge_rx_obj;
static fifo_s_t  judge_rxdata_fifo;
static uint8_t   judge_rxdata_buf[JUDGE_FIFO_BUFLEN];

u8 senderID;
u16 clientID;

u8 color;//红--0；蓝--1；
extern u8 Soilder_ID;
extern DMA_InitTypeDef DMA_InitStructure_tx;

u8 send_judge_G = 1;//自定义数据发送开启标志

/**
  * @brief  裁判系统通信
  * @param  
            
  * @note   保留串口3与裁判系统通信 配置DMA
  * @retval 
  */
void task_judge()
{
//	u8 light=0;
	int16_t i = 0;
	  /* 初始化USART */
  Debug_USART_Config(); 

  /* 配置使用DMA模式 */
  USART_DMA_TX_Config();
  USART_DoubleDMA_RX_Config();
	
  /* judge data fifo init */
  fifo_s_init(&judge_rxdata_fifo, judge_rxdata_buf, JUDGE_FIFO_BUFLEN);	
  /* initial judge data dma receiver object */
  judge_rx_obj.data_fifo = &judge_rxdata_fifo;
  judge_rx_obj.buff_size = READBUFF_SIZE;
  judge_rx_obj.buff[0] = ReadBuff;
  judge_rx_obj.buff[1] = ReadBuff2;		

  /* initial judge data unpack object */
  judge_unpack_obj.data_fifo = &judge_rxdata_fifo;
  judge_unpack_obj.p_header = (frame_header_t *)judge_unpack_obj.protocol_packet;
  judge_unpack_obj.index = 0;
  judge_unpack_obj.data_len = 0;
  judge_unpack_obj.unpack_step = STEP_HEADER_SOF;	
	
  //USART_SendData(DEBUG_USART,0x11);//测试连接

  for(i=0;i<READBUFF_SIZE;i++)
  {
    ReadBuff[i]	 = '0';
    ReadBuff2[i] = '0';
  }
	//judge_send_float(19.1,100.53,1564.9,0x00);//自定义数据发送
	
	/* USART3 向 DMA发出RX请求 */
	USART_DMACmd(DEBUG_USART, USART_DMAReq_Rx, ENABLE);
	/* USART3 向 DMA发出TX请求 */
	USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
  
	while(1)
	{	
		
		if(uart_idle_G == 1)//串口空闲中断
		{
			uart_idle_G = 0;
			//由DMA缓冲区转移至fifo_buff
			dma_buffer_to_unpack_buffer(&judge_rx_obj, UART_IDLE_IT);
			unpack_fifo_data(&judge_unpack_obj, DN_REG_ID);

		}
		else if(uart_DMA_full_G == 1)//DMA传输完成中断(或传输过半中断)
		{
			uart_DMA_full_G = 0;
			dma_buffer_to_unpack_buffer(&judge_rx_obj, UART_DMA_FULL_IT);
		}
		
		if(send_judge_G == 1)
		{
			/*在操作界面点灯，传输数据
			if(magazine)						light=light|0x10;
			else 								light=light&0x2f;
			
			if(mode_flag==CHASSIS_FOLLOW)		light=light|0x01;
			else 								light=light&0x3e;
			
			if(mode_flag==CHASSIS_NOHEAD)		light=light|0x02; 
			else 								light=light&0x3d;
			
			if(mode_flag==CHASSIS_CATWALK)		light=light|0x04;
			else 								light=light&0x3b;
			
			if(mode_flag==CHASSIS_LITTLETOP)	light=light|0x08;
			else 								light=light&0x37;
			
			judge_send_float(senderID,clientID,cup_percent,0,0,light);
			*/
		}
		/*//测试裁判系统读数
		if(system_clock%1500 < 50)//每1.5s发送一次接收数据
		{
			printf("-----------Judgement Rx data------------------\r\n");
			printf("max_HP:      %d\r\n",judge_recv_mesg.game_robot_state.max_HP);
			printf("remain_HP:   %d\r\n",judge_recv_mesg.game_robot_state.remain_HP);			
			printf("power:       %f\r\n",judge_recv_mesg.ext_power_heat_data.chassis_power);	
			printf("heat:       %d\r\n",judge_recv_mesg.ext_power_heat_data.shooter_heat0);			
		}
		*/
		task_delay_ms(10);
	}
	
}

/**
  * @brief  裁判系统读取数据测试任务.
  * @param  NULL.
  * @note   开启此任务后串口输出接收到的数据。
  * @retval NULL.
  */
void	judge_test(void* param)
{
	while(1)
	{
		if(system_clock%1500 < 50)//每1.5s发送一次接收数据
		{
			printf("-----------Judgement Rx data------------------\r\n");
			printf("max_HP:      %d\r\n",judge_recv_mesg.game_robot_state.max_HP);
			printf("remain_HP:   %d\r\n",judge_recv_mesg.game_robot_state.remain_HP);			
		}	
		task_delay_ms(10);
	}
}

