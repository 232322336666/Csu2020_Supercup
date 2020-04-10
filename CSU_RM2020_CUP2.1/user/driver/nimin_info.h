#ifndef _NIMIN_INFO_H
#define _NIMIN_INFO_H
/*
@file:
    ���ڽ����ܵ���ͨ��֡ת��Ϊ�û�����
    ��λ��������
*/

#include "makos_includes.h"

typedef __packed struct
{
  uint8_t p_h;
  uint8_t p_l;
  uint8_t i_h;
  uint8_t i_l;
  uint8_t d_h;
  uint8_t d_l;
} pid_data_temp_t, *pid_data_temp_p;

/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  pid_data_temp_t pid_data1; //��һ��PID
  pid_data_temp_t pid_data2; //�ڶ���PID
  pid_data_temp_t pid_data3; //������PID
} nimin_data_temp_t;         //����ϵͳ��������-���ݽṹ

typedef __packed struct
{
  u16 p;
  u16 i;
  u16 d;
} pid_data_t, *pid_data_p;

/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  pid_data_t pid_data1;        //��һ��PID
  pid_data_t pid_data2;        //�ڶ���PID
  pid_data_t pid_data3;        //������PID
} nimin_data_t, *nimin_data_p; //����ϵͳ��������-���ݽṹ

extern nimin_data_t nimin_data;

void nimin_data_handler(uint8_t *p_frame);

#endif
