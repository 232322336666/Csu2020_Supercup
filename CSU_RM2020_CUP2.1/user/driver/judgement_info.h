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
/** @file judgement_info.h
 *  @version 1.0
 *  @date June 2017
 *
 *  @brief the information from judgement system
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __JUDGEMENT_INFO_H__
#define __JUDGEMENT_INFO_H__

#include "makos_includes.h"

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
#define RED1_senderID 1
#define RED2_senderID 2
#define RED3_senderID 3
#define RED4_senderID 4
#define RED5_senderID 5
#define RED6_senderID 6
#define RED7_senderID 7

#define BLUE1_senderID 11
#define BLUE2_senderID 12
#define BLUE3_senderID 13
#define BLUE4_senderID 14
#define BLUE5_senderID 15
#define BLUE6_senderID 16
#define BLUE7_senderID 17

#define RED1_clientID 0x0101
#define RED2_clientID 0x0102
#define RED3_clientID 0x0103
#define RED4_clientID 0x0104
#define RED5_clientID 0x0105
#define RED6_clientID 0x0106
#define RED7_clientID 0x0107

#define BLUE1_clientID 0x0111
#define BLUE2_clientID 0x0112
#define BLUE3_clientID 0x0113
#define BLUE4_clientID 0x0114
#define BLUE5_clientID 0x0115
#define BLUE6_clientID 0x0116
#define BLUE7_clientID 0x0117

//**************************接收部分***********************
/** 
  * @brief  judgement data command id
  */
typedef enum
{
  GAME_STATE_ID       = 0x0001,
  GAME_RESULT_ID 	= 0x0002,
  GAME_ROBOT_SURVIVORS_ID = 0x0003,
  EVENT_DATA_ID = 0x0101,
  supply_projectile_actio_ID = 0x0102,
  supply_projectile_bookin     = 0x0103,
  game_robot_state_ID       = 0x0201,
  power_heat_data_ID  = 0x0202,
  game_robot_pos_ID = 0x0203,
  buff_musk_ID = 0x0204,
  aerial_robot_energy_ID = 0x0205,
robot_hur_ID = 0x206,
	shoot_dat_ID = 0x207,
	
} judge_data_id_e;


/** 
  * @brief  比赛状态数据：0x0001。发送频率：1Hz         
  */
typedef __packed struct {   
	uint8_t game_type : 4;//比赛类型 1：RM 对抗赛；2：单项赛；3：RM ICRA
	uint8_t game_progress : 4;//当前比赛阶段 0：未开始比赛；1：准备阶段；2：自检阶段；3：5s 倒计\
								时；4：对战中；5：比赛结算中 
	uint16_t stage_remain_time; //当前阶段剩余时间，单位 s 
} ext_game_state_t;

/** 
  * @brief  比赛结果数据：0x0002。发送频率：比赛结束后发送
  */
typedef __packed struct {    
	uint8_t winner;//0 平局 1 红方胜利 2 蓝方胜利 
} ext_game_result_t; 

/** 
  * @brief  机器人存活数据：0x0003。发送频率：1Hz 
	bit 0：红方英雄机器人； 
	bit 1：红方工程机器人； 
	bit 2：红方步兵机器人 1； 
	bit 3：红方步兵机器人 2； 
	bit 4：红方步兵机器人 3； 
	bit 5：红方空中机器人； 
	bit 6：红方哨兵机器人； 
	bit 7：保留 
	bit 8：蓝方英雄机器人； 
	bit 9：蓝方工程机器人； 
	bit 10：蓝方步兵机器人 1； 
	bit 11：蓝方步兵机器人 2； 
	bit 12：蓝方步兵机器人 3； 
	bit 13：蓝方空中机器人； 
	bit 14：蓝方哨兵机器人； 
	bit 15：保留 

	对应的 bit 数值置 1 代表机器人存活，数值置 0 代表机器人死亡或者未上场。 
  */
typedef __packed struct {   
	uint16_t robot_legion; 
} ext_game_robot_survivors_t; 

/** 
  * @brief  场地事件数据：0x0101。发送频率：事件改变后发送
  */
typedef __packed struct { 
  uint32_t event_type; 
} ext_event_data_t;



/** 
  * @brief  补给站动作标识：0x0102。发送频率：动作改变后发送 
  */
typedef __packed struct {   
	uint8_t supply_projectile_id;    
	uint8_t supply_robot_id;    
	uint8_t supply_projectile_step;  
} ext_supply_projectile_action_t;

/** 
  * @brief  补给站预约子弹：cmd_id (0x0103)。发送频率：上限 10Hz。RM 对抗赛尚未开放
  */
typedef __packed struct {   
	uint8_t supply_projectile_id;    
	uint8_t supply_num;  
} ext_supply_projectile_booking_t; 

/** 
  * @brief  比赛机器人状态：0x0201。发送频率：10Hz 
  */
typedef __packed struct {  
	uint8_t robot_id;   
	uint8_t robot_level;   
	uint16_t remain_HP; //机器人剩余血量
	uint16_t max_HP;    //机器人满血量
	uint16_t shooter_heat0_cooling_rate;  //机器人 17mm 子弹热量冷却速度 单位 /s 
	uint16_t shooter_heat0_cooling_limit; //机器人 17mm 子弹热量上限
	uint16_t shooter_heat1_cooling_rate;  //机器人 42mm 子弹热量冷却速度 单位 /s 
	uint16_t shooter_heat1_cooling_limit; //机器人 42mm 子弹热量上限   
	uint8_t mains_power_gimbal_output : 1; //1 为有 24V 输出，0 为无 24v 输出  
	uint8_t mains_power_chassis_output : 1;  //1 为有 24V 输出，0 为无 24v 输出
	uint8_t mains_power_shooter_output : 1;  //1 为有 24V 输出，0 为无 24v 输出
} ext_game_robot_state_t;

/** 
  * @brief  比赛机器人状态：0x0202。发送频率：10Hz 
  */
typedef __packed struct {  
	uint16_t chassis_volt;   //底盘输出电压
	uint16_t chassis_current;    //底盘电流
	float chassis_power;    //底盘功率
	uint16_t chassis_power_buffer;    
	uint16_t shooter_heat0;    //17mm枪口热量
	uint16_t shooter_heat1;  //42mm枪口热量
} ext_power_heat_data_t; 

/** 
  * @brief  机器人位置：0x0203。发送频率：10Hz
  */
typedef __packed struct {
	float x;   
	float y;   
	float z;   
	float yaw; 
} ext_game_robot_pos_t;

/** 
  * @brief  机器人增益：0x0204。发送频率：状态改变后发送
  */
typedef __packed struct {   
	uint8_t power_rune_buff; 
}ext_buff_musk_t; 

/** 
  * @brief  空中机器人能量状态：0x0205。发送频率：10Hz
  */
typedef __packed struct {   
	uint8_t energy_point;   
	uint8_t attack_time; 
} aerial_robot_energy_t; 

/** 
  * @brief  伤害状态：0x0206。发送频率：伤害发生后发送
  */
typedef __packed struct {   
	uint8_t armor_id : 4;   
	uint8_t hurt_type : 4; 
} ext_robot_hurt_t;

/** 
  * @brief  实时射击信息：0x0207。发送频率：射击后发送
  */
typedef __packed struct {   
	uint8_t bullet_type;    
	uint8_t bullet_freq;    
	float bullet_speed;  
} ext_shoot_data_t;


/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
	ext_game_state_t   				game_state;   //0x0001,比赛状态数据，1Hz 周期发送 
	ext_game_result_t  				game_result; //0x0002,比赛结果数据，比赛结束后发送
	ext_game_robot_survivors_t        game_robot_survivors;    //0x0003,比赛机器人存活数据，1Hz 发送 
	ext_event_data_t				    event_data;    //0x0101,场地事件数据，事件改变后发送
	ext_supply_projectile_action_t    supply_projectile_action;          //0x0102,场地补给站动作标识数据，动作改变后发送
	ext_supply_projectile_booking_t   supply_projectile_booking;   //0x0103,场地补给站预约子弹数据，由参赛队发送，上限 10Hz。（ RM 对抗赛尚未开放
	ext_game_robot_state_t         	game_robot_state;      //0x0201,机器人状态数据，10Hz 周期发送 
	ext_power_heat_data_t   			ext_power_heat_data;     //0x0202,实时功率热量数据，50Hz 周期发送
	ext_game_robot_pos_t	  			ext_game_robot_pos;
	ext_buff_musk_t						buff_musk;
	aerial_robot_energy_t			aerial_robot_energy;
	ext_robot_hurt_t			robot_hurt; 
	ext_shoot_data_t			ext_shoot_data;
} receive_judge_t;//裁判系统接受数据-数据结构

//**************************发送部分***********************
/** 
  * @brief  the data structure receive from judgement
  */
typedef __packed struct { 
	uint16_t	comment_ID;
	uint16_t	sender_ID;
	uint16_t	client_ID;
	float data1; 
	float data2; 
	float data3; 
	uint8_t masks; 
} client_custom_data_t;


/* data send (forward) */
/* data receive */
extern receive_judge_t judge_recv_mesg;
extern client_custom_data_t client_custom_data;

//for debug
extern uint16_t cmd_ID;
extern uint16_t data_Length;

//void  judgement_data_handler(uint8_t *p_frame);
void judgement_data_handler(uint8_t *p_frame);//裁判系统数据解析
void judge_send_float(u8 sender_ID,u16 client_ID,float data1,float data2,float data3,u8 light);

#endif
