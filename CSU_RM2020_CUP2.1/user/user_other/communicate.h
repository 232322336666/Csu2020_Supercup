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
/** @file communicate.h
 *  @version 1.0
 *  @date Oct 2017
 *
 *  @brief the communication interface of main control with
 *         judge system and computer
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __COMMUNICATE_H__
#define __COMMUNICATE_H__

#include "data_fifo.h"
#include "protocol.h"

typedef enum
{
  STEP_HEADER_SOF  = 0,
  STEP_LENGTH_LOW  = 1,
  STEP_LENGTH_HIGH = 2,
  STEP_FRAME_SEQ   = 3,
  STEP_HEADER_CRC8 = 4,
  STEP_DATA_CRC16  = 5,
} unpack_step_e;

typedef enum
{
  UART_IDLE_IT     = 0,
  UART_DMA_HALF_IT = 1,
  UART_DMA_FULL_IT = 2,
} uart_it_type_e;


typedef uint8_t UART_HandleTypeDef;

typedef struct
{
  UART_HandleTypeDef *huart;
  fifo_s_t           *data_fifo;
  uint16_t           buff_size;//1个buff的大小
  uint8_t            *buff[2];
  uint16_t           read_index;//
  uint16_t           write_index;//每次读取DMA剩余个数来更新
} uart_dma_rxdata_t;

typedef struct
{
  fifo_s_t       *data_fifo;//DMA装入的fifo
  frame_header_t *p_header;
  uint16_t       data_len;//数据帧长度
  uint8_t        protocol_packet[PROTOCAL_FRAME_MAX_SIZE];//筛选后的数据
  unpack_step_e  unpack_step;//解析步骤
  uint16_t       index;//当前解析到的字节标号
} unpack_data_t;
/*
  帧结构：
    帧头数据、命令码 ID、数据、帧尾校验数据。
*/


uint8_t* protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf);
extern int fifo_overflow;

/* dma double_buffer data puts to unpack_buffer */
void dma_buffer_to_unpack_buffer(uart_dma_rxdata_t *dma_obj, uart_it_type_e it_type);
void unpack_fifo_data(unpack_data_t *p_obj, uint8_t sof);
uint32_t send_packed_fifo_data(fifo_s_t *pfifo, uint8_t sof);
void data_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof);
	
#endif
