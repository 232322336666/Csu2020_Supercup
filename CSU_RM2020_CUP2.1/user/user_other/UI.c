#include "UI.h"
#include "keyboard.h"
#include "oled.h"
#include "UI_lib.h"
#include "task_vision.h"
//#include "task_communicate.h"
/*
使用说明：
	三种页面类型：
		FUNC(该页面下的选项为执行某一自定义函数，需自行添加自定义函数)
		DIG(该页面下的选项为修改相应的参数)
		SHOW(该页面下没有选项，为反复执行某一自定义函数，常用来显示一些曲线或者观察变量变化)
	每种页面的创建及设置以下均有示例。
*/

Page_TypeDef main_page;
Page_TypeDef param_set_page;
Page_TypeDef data_Show_page;
Page_TypeDef vs_param_set_page;

int test_1 = 1;
float test_2 = 2;
int test_3 = 1;
float test_4 = 2;

void param_set(void);
void OLED_data_save(void);
void data_show_callback(void);
void data_show(void);

void vs_param_set(void);

void page_init()
{	
	//主页面
	PAGE_SET_MAIN(main_page,FUNC,4);
	PAGE_FUNC(main_page,0,"param_set1",param_set);
	PAGE_FUNC(main_page,1,"vs_param",vs_param_set);
	PAGE_FUNC(main_page,2,"save_data",OLED_data_save);
	PAGE_FUNC(main_page,3,"show_data",data_show);
	//参数修改页面1
	PAGE_SET(param_set_page,DIG,2);
	PAGE_PARAM(param_set_page,0,"steer_PID_P",test_1,UINT32);//第一行显示参数设置
	PAGE_PARAM(param_set_page,1,"steer_PID_D",test_2,FLOAT);//第二行显示参数设置
	
	//参数修改页面2
	PAGE_SET(vs_param_set_page,DIG,6);	
	PAGE_PARAM(vs_param_set_page,0,"init_flag",vs_send.init_flag,UINT8);
	PAGE_PARAM(vs_param_set_page,1,"set_flag",vs_send.set_flag,UINT8);
	PAGE_PARAM(vs_param_set_page,2,"debug_flag",vs_send.debug_flag,UINT8);
	PAGE_PARAM(vs_param_set_page,3,"enemy_color",vs_send.enemy_color,UINT8);
	PAGE_PARAM(vs_param_set_page,4,"aim_or_rune",vs_send.aim_or_rune,UINT8);
	PAGE_PARAM(vs_param_set_page,5,"armor_type",vs_send.armor_type,UINT8);
	
	//变量显示页面
	PAGE_SET(data_Show_page,SHOW,NULL);	
	PAGE_SHOW(data_Show_page,data_show_callback);
}

void OS_menu(void)//用户调用的函数，完成UI初始化，开启第一页
{
	UI_show(&main_page);
}

void param_set(void)
{
	UI_show(&param_set_page);
}

/**
  * @brief  robot -> PC 信息发送
  * @param  direc: 1- PC->ROBOT   0- ROBOT->PC
            aim_mark  0:自动瞄准 1：识别大符
            pitching  枪管俯仰角
            gun_point_spd、gun_cross_spd  枪管朝向/横向速度
  * @note   
  * @retval 
  */
void vs_param_set(void)
{
	UI_show(&vs_param_set_page);
	pack_vs_info_send(vs_send);
}

void OLED_data_save()
{
	OLED_ShowString(45,3, "save ok",1);
	task_delay_ms(500);
	data_save();
	OLED_Clear();
}

void data_show(void)
{	
	//OLED_Display_Off();
	OLED_Clear();
	UI_show(&data_Show_page);
	OLED_Clear();
	//OLED_Display_On();
}

void data_show_callback(void)
{
	OLED_printf(0,0,"can1tx:%d",canrate.send.can1_tx);
	OLED_printf(64,0,"can2tx:%d",canrate.send.can2_tx);
	OLED_printf(0,1,"can2_201:%d",canrate.send.rx_motor[0]);
	OLED_printf(0,2,"can2_202:%d",canrate.send.rx_motor[1]);
	OLED_printf(0,3,"can2_203:%d",canrate.send.rx_motor[2]);
	OLED_printf(0,4,"can2_204:%d",canrate.send.rx_motor[3]);
	OLED_printf(0,5,"can2_205:%d",canrate.send.rx_motor[4]);
	OLED_printf(0,6,"can2_206:%d",canrate.send.rx_motor[5]);
}
