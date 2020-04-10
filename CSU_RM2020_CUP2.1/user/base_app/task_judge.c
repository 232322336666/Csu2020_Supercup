#include "task_judge.h"
#include "headfile.h"
#include "communicate.h"
#include "judgement_info.h"
#include "task_remote.h"
/*
ʵ��ƽ̨:
	RM2019�ڶ���
	DMA_UART�շ�����:USART3(�Ӳ���ϵͳ)
	���Դ���:UART5(�������ϵͳ����,ps:main�и�Ϊserial_init(5, 115200))
ʵ������
	����ϵͳ��ȡ����ÿ1.5s���Ͳ��ֲ���ϵͳ��ȡ���ݣ�ͨ���۲����С�Ƿ��������жϽ����Ƿ����	
	����3�������ϵͳ�������ݣ�����5���ؽ��յ�������
author:hhh
date:9/11/2018
�ײ����ã�����ԭ��DMA

*/

#define JUDGE_FIFO_BUFLEN      500

static unpack_data_t judge_unpack_obj;
static uart_dma_rxdata_t judge_rx_obj;
static fifo_s_t  judge_rxdata_fifo;
static uint8_t   judge_rxdata_buf[JUDGE_FIFO_BUFLEN];

u8 senderID;
u16 clientID;

u8 color;//��--0����--1��
extern u8 Soilder_ID;
extern DMA_InitTypeDef DMA_InitStructure_tx;

u8 send_judge_G = 1;//�Զ������ݷ��Ϳ�����־

/**
  * @brief  ����ϵͳͨ��
  * @param  
            
  * @note   ��������3�����ϵͳͨ�� ����DMA
  * @retval 
  */
void task_judge()
{
//	u8 light=0;
	int16_t i = 0;
	  /* ��ʼ��USART */
  Debug_USART_Config(); 

  /* ����ʹ��DMAģʽ */
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
	
  //USART_SendData(DEBUG_USART,0x11);//��������

  for(i=0;i<READBUFF_SIZE;i++)
  {
    ReadBuff[i]	 = '0';
    ReadBuff2[i] = '0';
  }
	//judge_send_float(19.1,100.53,1564.9,0x00);//�Զ������ݷ���
	
	/* USART3 �� DMA����RX���� */
	USART_DMACmd(DEBUG_USART, USART_DMAReq_Rx, ENABLE);
	/* USART3 �� DMA����TX���� */
	USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
  
	while(1)
	{	
		
		if(uart_idle_G == 1)//���ڿ����ж�
		{
			uart_idle_G = 0;
			//��DMA������ת����fifo_buff
			dma_buffer_to_unpack_buffer(&judge_rx_obj, UART_IDLE_IT);
			unpack_fifo_data(&judge_unpack_obj, DN_REG_ID);

		}
		else if(uart_DMA_full_G == 1)//DMA��������ж�(��������ж�)
		{
			uart_DMA_full_G = 0;
			dma_buffer_to_unpack_buffer(&judge_rx_obj, UART_DMA_FULL_IT);
		}
		
		if(send_judge_G == 1)
		{
			/*�ڲ��������ƣ���������
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
		/*//���Բ���ϵͳ����
		if(system_clock%1500 < 50)//ÿ1.5s����һ�ν�������
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
  * @brief  ����ϵͳ��ȡ���ݲ�������.
  * @param  NULL.
  * @note   ����������󴮿�������յ������ݡ�
  * @retval NULL.
  */
void	judge_test(void* param)
{
	while(1)
	{
		if(system_clock%1500 < 50)//ÿ1.5s����һ�ν�������
		{
			printf("-----------Judgement Rx data------------------\r\n");
			printf("max_HP:      %d\r\n",judge_recv_mesg.game_robot_state.max_HP);
			printf("remain_HP:   %d\r\n",judge_recv_mesg.game_robot_state.remain_HP);			
		}	
		task_delay_ms(10);
	}
}

