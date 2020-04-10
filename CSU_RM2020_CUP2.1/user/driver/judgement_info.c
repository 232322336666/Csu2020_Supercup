/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file judgement_info.c
 *  @version 1.0
 *  @date June 2017
 *
 *  @brief the information from judgement system
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#include "judgement_info.h"
#include "communicate.h"
#include "string.h"
#include "headfile.h"


/* data send (forward) */
/* data receive */
receive_judge_t judge_recv_mesg;
client_custom_data_t client_custom_data;

extern int8_t recv_pc_glb;
extern int8_t glb_err_exit;

extern DMA_InitTypeDef DMA_InitStructure_tx;

/**
  * @brief  裁判系统用户数据发送.
  * @param  data1,data2,data3 客户端显示的三个浮点数.
  * @note   用户调用,非ISR调用环境均可.
  * @retval NULL.
  */
	/**
  * @brief  自定义数据发送
  * @param  comment_ID:数据的内容ID(固定0xD180)；
	          sender_ID：发送者机器人的ID（1，红英雄；2，红工程；3/4/5，红步兵；6，红空中；7，红哨兵
						                             11，蓝英雄；12，蓝工程；13/14/15,蓝步兵；16，蓝空中；17，蓝哨兵）
						client_ID：发送者机器人对应的客户端ID（0x0101，红英雄客户端；0x0102，红工程客户端
						                                       0x0103/4/5，红步兵；0x0106...
																									 0x0111，蓝英雄客户端；0x0112/3/4/5/6...）	
						data1/2/3：自定义浮点数据
            indicator_light：bit 0-5 分别控制客户端显示面板上的6个指示灯（1，绿色；0，红色）						
  * @note   
  * @retval NULL.
  */
void judge_send_float(u8 sender_ID,u16 client_ID,float data1,float data2,float data3,u8 light)
{
	client_custom_data_t client_custom_data;
	
	client_custom_data.comment_ID = 0xD180;
	
//	if(FD.car_id == CAR_ID_HERO_MAIN)
//	{
		client_custom_data.sender_ID = sender_ID;
		client_custom_data.client_ID = client_ID;
//	}
	client_custom_data.masks = light;
	
	client_custom_data.data1 = data1;
	client_custom_data.data2 = data2;
	client_custom_data.data3 = data3;
	
	data_packet_pack(0x0301, (uint8_t *)&client_custom_data, sizeof(client_custom_data_t),0xA5);
	//触发DMA发送
	//USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);	
	
	while( DMA_GetFlagStatus(DEBUG_USART_TX_DMA_STREAM,DMA_FLAG_TCIF3) == 0);//等待DMA发送完成
  DMA_ClearFlag(DEBUG_USART_TX_DMA_STREAM,DMA_FLAG_TCIF3);
	
	DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, DISABLE);
	DMA_Init(DEBUG_USART_TX_DMA_STREAM, &DMA_InitStructure_tx);
	DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, ENABLE);
}

/**
  * @brief    get judgement system message
  */


//for debug
uint16_t cmd_ID = 0;
uint16_t data_Length = 0;

void judgement_data_handler(uint8_t *p_frame)//裁判系统数据解析
{
  //从fifo中提取相应数据
  frame_header_t *p_header = (frame_header_t*)p_frame;

  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;//数据帧头地址
  
  memcpy(p_header, p_frame, HEADER_LEN);	
  //uint8_t forward_flag = 1;
  
	//for_debug
	cmd_ID = cmd_id;
	data_Length = data_length;
	
  //根据命令码将数据装入对应的用户数据结构
  switch (cmd_id)
  {
    case GAME_STATE_ID:
      memcpy(&judge_recv_mesg.game_state, data_addr, data_length);	
    break;

    case GAME_RESULT_ID:
      memcpy(&judge_recv_mesg.game_result, data_addr, data_length);
    break;

    case GAME_ROBOT_SURVIVORS_ID:
      memcpy(&judge_recv_mesg.game_robot_survivors, data_addr, data_length);
    break;
    
    case EVENT_DATA_ID:
      memcpy(&judge_recv_mesg.event_data, data_addr, data_length);
    break;

    case supply_projectile_actio_ID:
      memcpy(&judge_recv_mesg.supply_projectile_action, data_addr, data_length);
    break;

    case supply_projectile_bookin:
      memcpy(&judge_recv_mesg.supply_projectile_booking, data_addr, data_length);
    break;
    
    case game_robot_state_ID:
      memcpy(&judge_recv_mesg.game_robot_state, data_addr, data_length);
    break;

    case power_heat_data_ID:
      memcpy(&judge_recv_mesg.ext_power_heat_data, data_addr, data_length);
    break;

    case game_robot_pos_ID:
      memcpy(&judge_recv_mesg.ext_game_robot_pos, data_addr, data_length);
    break;

    case buff_musk_ID:
      memcpy(&judge_recv_mesg.buff_musk, data_addr, data_length);
    break;

    case aerial_robot_energy_ID:
      memcpy(&judge_recv_mesg.aerial_robot_energy, data_addr, data_length);
    break;

    case robot_hur_ID:
      memcpy(&judge_recv_mesg.robot_hurt, data_addr, data_length);
    break;

    case shoot_dat_ID:
      memcpy(&judge_recv_mesg.ext_shoot_data, data_addr, data_length);
    break;	

    default:
      //forward_flag = 0;
    break;
  }	
}

