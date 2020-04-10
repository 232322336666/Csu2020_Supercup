#include "UI.h"
#include "keyboard.h"
#include "oled.h"
#include "UI_lib.h"
#include "task_vision.h"
//#include "task_communicate.h"
/*
ʹ��˵����
	����ҳ�����ͣ�
		FUNC(��ҳ���µ�ѡ��Ϊִ��ĳһ�Զ��庯��������������Զ��庯��)
		DIG(��ҳ���µ�ѡ��Ϊ�޸���Ӧ�Ĳ���)
		SHOW(��ҳ����û��ѡ�Ϊ����ִ��ĳһ�Զ��庯������������ʾһЩ���߻��߹۲�����仯)
	ÿ��ҳ��Ĵ������������¾���ʾ����
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
	//��ҳ��
	PAGE_SET_MAIN(main_page,FUNC,4);
	PAGE_FUNC(main_page,0,"param_set1",param_set);
	PAGE_FUNC(main_page,1,"vs_param",vs_param_set);
	PAGE_FUNC(main_page,2,"save_data",OLED_data_save);
	PAGE_FUNC(main_page,3,"show_data",data_show);
	//�����޸�ҳ��1
	PAGE_SET(param_set_page,DIG,2);
	PAGE_PARAM(param_set_page,0,"steer_PID_P",test_1,UINT32);//��һ����ʾ��������
	PAGE_PARAM(param_set_page,1,"steer_PID_D",test_2,FLOAT);//�ڶ�����ʾ��������
	
	//�����޸�ҳ��2
	PAGE_SET(vs_param_set_page,DIG,6);	
	PAGE_PARAM(vs_param_set_page,0,"init_flag",vs_send.init_flag,UINT8);
	PAGE_PARAM(vs_param_set_page,1,"set_flag",vs_send.set_flag,UINT8);
	PAGE_PARAM(vs_param_set_page,2,"debug_flag",vs_send.debug_flag,UINT8);
	PAGE_PARAM(vs_param_set_page,3,"enemy_color",vs_send.enemy_color,UINT8);
	PAGE_PARAM(vs_param_set_page,4,"aim_or_rune",vs_send.aim_or_rune,UINT8);
	PAGE_PARAM(vs_param_set_page,5,"armor_type",vs_send.armor_type,UINT8);
	
	//������ʾҳ��
	PAGE_SET(data_Show_page,SHOW,NULL);	
	PAGE_SHOW(data_Show_page,data_show_callback);
}

void OS_menu(void)//�û����õĺ��������UI��ʼ����������һҳ
{
	UI_show(&main_page);
}

void param_set(void)
{
	UI_show(&param_set_page);
}

/**
  * @brief  robot -> PC ��Ϣ����
  * @param  direc: 1- PC->ROBOT   0- ROBOT->PC
            aim_mark  0:�Զ���׼ 1��ʶ����
            pitching  ǹ�ܸ�����
            gun_point_spd��gun_cross_spd  ǹ�ܳ���/�����ٶ�
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
