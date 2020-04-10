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
/** @file communicate.c
 *  @version 1.0
 *  @date Oct 2017
 *
 *  @brief the communication interface of main control with
 *         judge system and computer
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#include "communicate.h"
#include "judgement_info.h"
#include "headfile.h"


//sof:头帧，裁判系统--0xA5
void unpack_fifo_data(unpack_data_t *p_obj, uint8_t sof)
{
  uint8_t byte = 0;

  while ( fifo_used_count(p_obj->data_fifo) )
  {
    byte = fifo_s_get(p_obj->data_fifo);
    switch(p_obj->unpack_step)//状态机写法
    {
      case STEP_HEADER_SOF:
      {
        if(byte == sof)//只有匹配头帧才向下解析
        {
          p_obj->unpack_step = STEP_LENGTH_LOW;
          p_obj->protocol_packet[p_obj->index++] = byte;
        }
        else
        {
          p_obj->index = 0;
        }
      }break;

      case STEP_LENGTH_LOW://低8位解析
      {
        p_obj->data_len = byte;
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_LENGTH_HIGH;
      }break;

      case STEP_LENGTH_HIGH://高八位解析
      {
        p_obj->data_len |= (byte << 8);
        p_obj->protocol_packet[p_obj->index++] = byte;

        if(p_obj->data_len < (PROTOCAL_FRAME_MAX_SIZE - HEADER_LEN - CRC_LEN))//数据长度合理
        {
          p_obj->unpack_step = STEP_FRAME_SEQ;//包序号
        }
        else//出错，本次解析失败，数据记录归零
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }break;

      case STEP_FRAME_SEQ:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_HEADER_CRC8;
      }break;

      case STEP_HEADER_CRC8:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;

        if (p_obj->index == HEADER_LEN)//帧头数据长度正确
        {
          if ( verify_crc8_check_sum(p_obj->protocol_packet, HEADER_LEN) )
          {
            p_obj->unpack_step = STEP_DATA_CRC16;
          }
          else//不合理，归零
          {
            p_obj->unpack_step = STEP_HEADER_SOF;
            p_obj->index = 0;
          }
        }
        else
        {
          //
        }
      }break;

      case STEP_DATA_CRC16:
      {
        if (p_obj->index < (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))//继续缓存包数据
        {
           p_obj->protocol_packet[p_obj->index++] = byte;
        }
        if (p_obj->index >= (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))//全部缓存成功
        {
          //解析步骤归位
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
          //对整个包数据进行CRC检验
          if ( verify_crc16_check_sum(p_obj->protocol_packet, HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN) )
          {
            if (sof == UP_REG_ID)
            {
              //pc_data_handler(p_obj->protocol_packet);
            }
            else  //DN_REG_ID
            {
              judgement_data_handler(p_obj->protocol_packet);
            }
          }
        }
      }break;

      default:
      {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;
      }break;
  }
    }
}

//for debug
int dma_write_len = 0;
int fifo_overflow = 0;



void get_dma_memory_msg(uint8_t *mem_id, uint16_t *remain_cnt)
{
  *mem_id     = DMA_GetCurrentMemoryTarget(DEBUG_USART_RX_DMA_STREAM);
  *remain_cnt = DMA_GetCurrDataCounter(DEBUG_USART_RX_DMA_STREAM);
}


void dma_buffer_to_unpack_buffer(uart_dma_rxdata_t *dma_obj, uart_it_type_e it_type)
{
  int16_t  tmp_len;
  uint8_t  current_memory_id;
  uint16_t remain_data_counter;
  uint8_t  *pdata = dma_obj->buff[0];//DMA原始缓冲区基地址

  //获取当前DMA的缓冲区标号，剩余量
  get_dma_memory_msg(&current_memory_id, &remain_data_counter);

  if (UART_IDLE_IT == it_type)//空闲中断
  {
    if (current_memory_id)//缓冲区1
    {
      dma_obj->write_index = dma_obj->buff_size*2 - remain_data_counter;//remain_data_counter:单个DMA剩余传输个数
    }
    else//缓冲区0
    {
      dma_obj->write_index = dma_obj->buff_size - remain_data_counter;
    }
  }
  else if (UART_DMA_FULL_IT == it_type)//DMA满中断
  {
#if 0
    if (current_memory_id)//缓冲区1
    {
      dma_obj->write_index = dma_obj->buff_size;
    }
    else//缓冲区0
    {
      dma_obj->write_index = dma_obj->buff_size*2;
    }
#endif
  }


  if (dma_obj->write_index < dma_obj->read_index)//跨DMA缓冲区
  {
    //先读read_index到缓冲区结束部分的dma
    dma_write_len = dma_obj->buff_size*2 - dma_obj->read_index + dma_obj->write_index;
    tmp_len = dma_obj->buff_size*2 - dma_obj->read_index;
    if (tmp_len != fifo_s_puts(dma_obj->data_fifo, &pdata[dma_obj->read_index], tmp_len))
      fifo_overflow = 1;
    else
      fifo_overflow = 0;
    dma_obj->read_index = 0;

    //再读0到write_index部分的dma
    tmp_len = dma_obj->write_index;
    if (tmp_len != fifo_s_puts(dma_obj->data_fifo, &pdata[dma_obj->read_index], tmp_len))
      fifo_overflow = 1;
    else
      fifo_overflow = 0;
    dma_obj->read_index = dma_obj->write_index;
  }
  else
  {
    dma_write_len = dma_obj->write_index - dma_obj->read_index;

    tmp_len = dma_obj->write_index - dma_obj->read_index;

    if (tmp_len != fifo_s_puts(dma_obj->data_fifo, &pdata[dma_obj->read_index], tmp_len))//read_index用于记录当前读到的数据位置
      fifo_overflow = 1;//fifo成功记录的数据不等于需要记录的数据
    else
      fifo_overflow = 0;
    dma_obj->read_index = (dma_obj->write_index) % (dma_obj->buff_size*2);//可能有bug这句话
    //dma_obj->read_index = dma_obj->write_index;
  }
}

#define JUDGE_FIFO_BUFLEN 500

//static fifo_s_t  judge_txdata_fifo;
//static uint8_t   judge_txdata_buf[JUDGE_FIFO_BUFLEN];

#define COMPUTER_FIFO_BUFLEN 500

//static fifo_s_t  pc_txdata_fifo;
//static uint8_t   pc_txdata_buf[COMPUTER_FIFO_BUFLEN];

/**
  * @brief  裁判系统发送数据打包.
  * @param  p_data 发送数据段地址
			cmd_id 命令码ID
			sof		数据帧起始字节，固定值为 0xA5 
  * @note   发送数据->发送给fifo中，含头帧尾帧CRC等的添加.
  * @retval NULL.
  */
void data_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof)
{
  uint8_t tx_buf[PROTOCAL_FRAME_MAX_SIZE];
  
  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN;
  
	//尾部追加CRC校验
  protocol_packet_pack(cmd_id, p_data, len, sof, tx_buf);
  
  //打包放进fifo中
  // fifo_s_puts(&judge_txdata_fifo, tx_buf, frame_length);//裁判系统
	
	memcpy(SendBuff, tx_buf, frame_length);
    return ;
}

/**
  * @brief  数据包尾部追加CRC.
  * @param  
* @note   内部调用.
  * @retval NULL.
  */
uint8_t* protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf)
{
  //memset(tx_buf, 0, 100);
  static uint8_t seq;

  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN;
  frame_header_t *p_header = (frame_header_t*)tx_buf;

  p_header->sof          = sof;
  p_header->data_length  = len;

  if (sof == UP_REG_ID)
  {
    if (seq++ >= 255)
      seq = 0;

    p_header->seq = seq;
  }
  else
  {
    p_header->seq = 0;
  }

  memcpy(&tx_buf[HEADER_LEN], (uint8_t*)&cmd_id, CMD_LEN);//复制帧头数据段
  append_crc8_check_sum(tx_buf, HEADER_LEN);//追加帧头的CRC校验
  memcpy(&tx_buf[HEADER_LEN + CMD_LEN], p_data, len);//复制数据段
  append_crc16_check_sum(tx_buf, frame_length);//追加尾帧的CRC校验

  return tx_buf;
}


//uint32_t send_packed_fifo_data(fifo_s_t *pfifo, uint8_t sof)
//{
//#if (JUDGE_FIFO_BUFLEN > COMPUTER_FIFO_BUFLEN)
//  uint8_t  tx_buf[JUDGE_FIFO_BUFLEN];
//#else
//  uint8_t  tx_buf[COMPUTER_FIFO_BUFLEN];
//#endif
//  
//  uint32_t fifo_count = fifo_used_count(pfifo);
//  
//  if (fifo_count)
//  {
//    fifo_s_gets(pfifo, tx_buf, fifo_count);
//    
//    if (sof == UP_REG_ID)
//      write_uart_blocking(&COMPUTER_HUART, tx_buf, fifo_count);
//    else if (sof == DN_REG_ID)
//      write_uart_blocking(&JUDGE_HUART, tx_buf, fifo_count);
//    else
//      return 0;
//  }
//  
//  return fifo_count;
//}
