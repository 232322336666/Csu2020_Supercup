/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of?
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.? See the
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

#ifndef __COMMUNICATE_NIMIN_H__
#define __COMMUNICATE_NIMIN_H__

#include "data_fifo.h"
#include "protocol.h"

typedef enum
{
  STEP_HEADER1 = 0,
  STEP_HEADER2 = 1,
  STEP_HEADER3 = 2,
  STEP_HEADER4 = 3,
  STEP_DATA = 4,
  STEP_NOUSE_DATA = 5,
  STEP_SUM_CHECK = 6
} unpack_step_nimin_e;

typedef struct
{
  fifo_s_t *data_fifo; //DMAװ���fifo
  frame_header_t *p_header;
  uint16_t data_len;                                //����֡����
  uint8_t protocol_packet[PROTOCAL_FRAME_MAX_SIZE]; //ɸѡ�������
  unpack_step_nimin_e unpack_step;                  //��������
  uint16_t index;                                   //��ǰ���������ֽڱ��
} unpack_data_nimin_t;

/*
  ֡�ṹ��
    ֡ͷ���ݡ������� ID�����ݡ�֡βУ�����ݡ�
*/

void unpack_fifo_data_nimin(unpack_data_nimin_t *p_obj, uint8_t sof);

#endif
