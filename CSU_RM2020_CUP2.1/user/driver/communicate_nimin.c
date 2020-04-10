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

#include "communicate_nimin.h"
#include "nimin_info.h"
#include "data_fifo.h"

/*
    @func:
    ��У��
    @return:
        1 -- У��ɹ�
        0 -- ����ʧ��
*/
u8 check_sum(uint8_t *pchMessage, uint32_t dwLength)
{
  uint8_t sum = 0;
  uint8_t i;
  for (i = 0; i < dwLength - 1; i++)
    sum += *(pchMessage + i);

  if (sum == *(pchMessage + dwLength - 1))
    return 1;
  else
    return 0;
}

//sof:ͷ֡������ϵͳ--0xA5
void unpack_fifo_data_nimin(unpack_data_nimin_t *p_obj, uint8_t sof)
{
  uint8_t byte = 0;

  while (fifo_used_count(p_obj->data_fifo)) //ֱ��������fifo������ʣ������
  {
    byte = fifo_s_get(p_obj->data_fifo);

    switch (p_obj->unpack_step) //״̬��д��
    {
    case STEP_HEADER1:
    {
      if (byte == 0X8A) //ֻ��ƥ��ͷ֡�����½���
      {
        p_obj->unpack_step = STEP_HEADER2;
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      else
      {
        p_obj->index = 0;
      }
    }
    break;
    case STEP_HEADER2:
    {
      if (byte == 0X8B) //ֻ��ƥ��ͷ֡�����½���
      {
        p_obj->unpack_step = STEP_HEADER3;
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      else
      {
        p_obj->index = 0;
      }
    }
    break;
    case STEP_HEADER3:
    {
      if (byte == 0X1C) //ֻ��ƥ��ͷ֡�����½���
      {
        p_obj->unpack_step = STEP_HEADER4;
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      else
      {
        p_obj->index = 0;
      }
    }
    break;
    case STEP_HEADER4:
    {
      if (byte == 0XAE) //ֻ��ƥ��ͷ֡�����½���
      {
        p_obj->unpack_step = STEP_DATA;
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      else
      {
        p_obj->index = 0;
      }
    }
    break;
    case STEP_DATA: //��8λ����
    {
      p_obj->protocol_packet[p_obj->index++] = byte;

      if (p_obj->index > 22)
        p_obj->unpack_step = STEP_NOUSE_DATA;
    }
    break;

    case STEP_NOUSE_DATA: //�߰�λ����
    {
      p_obj->protocol_packet[p_obj->index++] = byte;

      if (p_obj->index > 30)
        p_obj->unpack_step = STEP_SUM_CHECK;
    }
    break;

    case STEP_SUM_CHECK: //��У��
    {
      p_obj->protocol_packet[p_obj->index++] = byte;
      //���������λ
      p_obj->unpack_step = STEP_HEADER1;
      p_obj->index = 0;
      if (check_sum(p_obj->protocol_packet, 32))
      {
        nimin_data_handler(p_obj->protocol_packet);
      }
    }
    break;
    default:
    {
      p_obj->unpack_step = STEP_HEADER1;
      p_obj->index = 0;
    }
    break;
    }
  }
}
