#ifndef __UI_LIB_H
#define __UI_LIB_H	

#include "headfile.h"

//typedef struct _dnode
//{
//	struct _dnode*	next;
//	struct _dnode*	prev;
//}dnode_t, *p_dnode;

void	dnode_head_init(p_dnode hd);
void	dnode_add_first(p_dnode hd, p_dnode nd);

typedef enum
{
	UINT32,
	INT32,
	UINT16,
	INT16,
	UINT8,
	INT8,
	FLOAT,
}datatype_TypeDef;

typedef	struct
{
	dnode_t		dnd;
	datatype_TypeDef datatype;
	uint32_t * p_uint32_t;
	float * p_float;
}flash_list_t, *p_flash_list;

void	flash_list_init(void);
void	flash_list_add(datatype_TypeDef datatype, void * p_data);
void 	data_read(void);
void 	data_save(void);

/*
PAGE:�����õ�ҳ��
X�������õ�ҳ������
CONTENT��������ʾ���ַ���
PARAM�����޸ĵĲ���
PARAM_TYPE���������ͣ���֧�� float u32
*/
#define   PAGE_PARAM(PAGE,X,CONTENT,PARAM,PARAM_TYPE)   do{   \
   \
	PAGE.Page_content[X] = (uint8_t *)CONTENT;\
	PAGE.param[X] = (void *)&PARAM;	\
	PAGE.param_type[X] = PARAM_TYPE;\
	flash_list_add(PARAM_TYPE, &PARAM);\
   \
}while(0) 

/*
PAGE:�����õ�ҳ��
X�������õ�ҳ������
CONTENT��������ʾ���ַ���
FUNC: ����ѡ��ʱ���õ��Զ��庯��
*/

#define   PAGE_FUNC(PAGE,X,CONTENT,FUNC)   do{   \
   \
	PAGE.Page_content[X] = (uint8_t *)CONTENT;\
	PAGE.func[X] = FUNC;\
   \
}while(0) 

/*
PAGE:�����õ�ҳ��
FUNC: ��ҳ���в��ϵ��õĻص�����(�Զ���),����������ʾҳ��
*/

#define   PAGE_SHOW(PAGE,FUNC)   do{   \
   \
	PAGE.show_func = FUNC;\
   \
}while(0) 
/*

���ڷ�ҳ�������

PAGE:�����õ�ҳ��
TYPE:����ҳ������
LENGTH:���ø�ҳ�������
*/
#define   PAGE_SET(PAGE,TYPE,LENGTH)   do{   \
   \
	PAGE.page_type = TYPE;\
	PAGE.Page_H = LENGTH;\
   \
}while(0) 

/*

������ҳ�������

PAGE:�����õ�ҳ��
TYPE:����ҳ������
LENGTH:���ø�ҳ�������
*/

#define   PAGE_SET_MAIN(PAGE,TYPE,LENGTH)   do{   \
   \
	PAGE.page_type = TYPE;\
	PAGE.Page_H = LENGTH;\
	PAGE.is_mainmenu = 1;\
   \
}while(0) 

#define PAGE_FUNC_MAX	10//FUNC��ҳ�����֧�ֵ��Զ��庯����Ŀ
typedef	void	(*page_f_array[PAGE_FUNC_MAX])();
typedef	void	(*page_f)();

#define Page_L 15//ҳ����ַ������������
#define PARAM_MAX	10//DIG��ҳ�����֧�ֵĿ��޸ı�����Ŀ

typedef enum
{ 
  DIG = 1,
  FUNC,
	SHOW
}Pagetype_TypeDef;

typedef struct
{
	uint8_t Page_H;
	uint8_t * Page_content[Page_L];    
	page_f_array  func;
	page_f	show_func;
	Pagetype_TypeDef page_type;
	void * param[PARAM_MAX];
	datatype_TypeDef param_type[PARAM_MAX];
	uint8_t is_mainmenu;
}Page_TypeDef,* P_Page_TypeDef;

/*UI_SYSTEM��������*/
#define USE_OS	
#define	FLASH_INIT	//eeprom_init

/*�ڴ����*/
#define	MEMORY_GET	mymalloc
/*д��һ��4�ֽڵ�ַ��Ԫ*/
#define FLASH_WRITE_4BYTES(OFFSET_START,data_save_buff,DATA_LEN)  STM32_FLASH_Write(STM32_FLASH_SAVE_ADDR,(u16 *)data_save_buff,2*data_num);	
/*д��һ��4�ֽڵ�Ԫ*/
#define FLASH_READ_4BYTES(OFFSET_START,DATA_SAVE_BUFF,DATA_LEN)  STM32_FLASH_Write(STM32_FLASH_SAVE_ADDR,(DATA_SAVE_BUFF,2*data_num);	

/*���ݴ洢����ʼƫ����*/
#define	OFFSET_START	0
/*���ݴ洢����С(��λ4�ֽ�)*/
#define FLASH_BUFF_LENTH 20

void UI_init(void);
void UI_show(P_Page_TypeDef p_page);
void set_mainpage_callback(void  (*user_callback)(void));

#endif
