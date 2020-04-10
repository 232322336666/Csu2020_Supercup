#include "keyboard.h"
#include "oled.h"
#include "UI_lib.h"
#include "headfile.h"
#include "UI.h"

#ifndef	NULL
    #define		 NULL		((void*)0)
#endif

//***********************************双向链表部分*******************************************
static void	dnode_head_init(p_dnode hd)
{
	hd->next = hd;
	hd->prev = hd;
}

/*
	@param:
		hd:前一个双向链表节点
		nd:后一个双向链表节点
	@fun:
		插入当前节点至倒数第二个
*/
static void 	dnode_add_last(p_dnode hd, p_dnode nd)
{
	hd->prev->next	= nd;
	nd->prev	= hd->prev;
	nd->next	= hd;
	hd->prev	= nd;
}
//**************************************************************************************************


//*****************************************UI菜单部分**********************************************
Key_msg_t KeyStatus;
int Cursor = 0;
int Cursor_last = 0;
int NowPage = 0;
int Menustatus = 0;
uint8_t uintForNow = 0;
float DataInterval = 0; 
double  uintArray[8] = {100, 10, 1, 0, 0.1, 0.01, 0.001, 0.0001};
int DataToChangeNum = 0;

void UI_init(void)
{
	flash_list_init();//必须在page_init()之前调用
	OLED_Init();
	key_init();
	//FLASH_INIT();
	page_init();
	data_read();//必须在page_init()之后调用
}

void Key_Event(void) 
{
  if (Get_KeyFifo(&KeyStatus)) 
  {
    switch (KeyStatus.key) 
    {
    case Key_U:
      if (!Menustatus) 
      {
        if (KeyStatus.status == 0)
        {
          Cursor--;       
        }
      }
      else 
      {
        if (KeyStatus.status == 0) 
        {
          DataInterval++; 
        }
      }
      break;
    case Key_D:
      if (!Menustatus) 
      {
        if (KeyStatus.status == 0) 
        {
          Cursor++; 
        }
      }
      else 
      {
        if (KeyStatus.status == 0)
        {
          DataInterval--;
        }
      }
      break;
    case Key_Y:
	
      if (KeyStatus.status == 0) 
      {
        if (!Menustatus) 
          NowPage++;     
        else             
        {
          uintForNow++; 
          if (uintForNow == 8)
            uintForNow = 0; 
        }
      }
      break;
    case Key_N:
      if (KeyStatus.status == 0) 
      {

        NowPage--;          
        if (Menustatus == 1) 
          Menustatus = 0;   
      }
      break;	  
    default:
      break;
    }
  }
  return;
}

void ChangeData(P_Page_TypeDef p_page, uint8_t Num, uint8_t LastPage, void *DataToChange[])
{
	uint8_t *Object = p_page->Page_content[Num];
	uint32_t *data_tmp_u32;
	int32_t * data_tmp_s32;
	int16_t * data_tmp_s16;
	uint16_t *data_tmp_u16;
	int8_t *data_tmp_s8;
	uint8_t *data_tmp_u8;
	
  float *data_tmp_float;

  if (p_page->param_type[Num] == UINT32)
    data_tmp_u32 = (uint32_t *)p_page->param[Num];
  else if(p_page->param_type[Num] == INT32)
	data_tmp_s32 = (int32_t *)p_page->param[Num]; 
  else if(p_page->param_type[Num] == UINT16)
	data_tmp_u16 = (uint16_t *)p_page->param[Num];  
  else if(p_page->param_type[Num] == INT16)
	data_tmp_s16 = (int16_t *)p_page->param[Num];
  else if(p_page->param_type[Num] == UINT8)
	data_tmp_u8 = (uint8_t *)p_page->param[Num];  
  else if(p_page->param_type[Num] == INT8)
	data_tmp_s8 = (int8_t *)p_page->param[Num];  
  else
    data_tmp_float = (float *)p_page->param[Num];
  
  OLED_Clear(); 
  Menustatus = 1;
  uintForNow = 1;                  
  OLED_ShowString(0, 0, Object, 0);
  while (1)
  {
    Key_Event();
    if (NowPage == LastPage)
    {
      Menustatus = 0; 
      OLED_Clear();   
      break;          
    }
	
    if (uintArray[uintForNow] != 0)
    {
      if (p_page->param_type[Num] == UINT32)
        *data_tmp_u32 += DataInterval * uintArray[uintForNow];
      else if(p_page->param_type[Num] == INT32)
		*data_tmp_s32 += DataInterval * uintArray[uintForNow];
      else if(p_page->param_type[Num] == UINT16)
		*data_tmp_u16 += DataInterval * uintArray[uintForNow];	  
      else if(p_page->param_type[Num] == INT16)
		*data_tmp_s16 += DataInterval * uintArray[uintForNow];
      else if(p_page->param_type[Num] == UINT8)
		*data_tmp_u8 += DataInterval * uintArray[uintForNow];	  
      else if(p_page->param_type[Num] == INT8)
		*data_tmp_s8 += DataInterval * uintArray[uintForNow];	  
	  else
        *data_tmp_float += DataInterval * uintArray[uintForNow];
    }
    else if (uintArray[uintForNow] == 0 && DataInterval != 0)
    {
      if (p_page->param_type[Num] == UINT32)
        *data_tmp_u32 = 0;
      else if(p_page->param_type[Num] == INT32)
	    *data_tmp_s32 = 0;
      else if(p_page->param_type[Num] == UINT16)
	    *data_tmp_u16 = 0;
      else if(p_page->param_type[Num] == INT16)
	    *data_tmp_s16 = 0;	  
      else if(p_page->param_type[Num] == UINT8)
	    *data_tmp_u8 = 0;
      else if(p_page->param_type[Num] == INT8)
	    *data_tmp_s8 = 0;
	  else
        *data_tmp_float = 0;
    }

    DataInterval = 0; 

    if (p_page->param_type[Num] == UINT32)
      OLED_PrintVar(0, 1, *data_tmp_u32, 14, 4);
    else if(p_page->param_type[Num] == INT32)
	  OLED_PrintVar(0, 1, *data_tmp_s32, 14, 4);
    else if(p_page->param_type[Num] == UINT16)
	  OLED_PrintVar(0, 1, *data_tmp_u16, 14, 4);
    else if(p_page->param_type[Num] == INT16)
	  OLED_PrintVar(0, 1, *data_tmp_s16, 14, 4);
    else if(p_page->param_type[Num] == UINT8)
	  OLED_PrintVar(0, 1, *data_tmp_u8, 14, 4);	
    else if(p_page->param_type[Num] == INT8)
	  OLED_PrintVar(0, 1, *data_tmp_s8, 14, 4);
	else
      OLED_PrintVar(0, 1, *data_tmp_float, 14, 4);

    if (uintArray[uintForNow] == 0)
    {
      OLED_ShowString(0, 2, "Clear To  0", 1); 
    }
    else
    {
      OLED_PrintVar(0, 2, uintArray[uintForNow], 14, 5); 
    }

	#ifdef USE_OS
		task_delay_ms(20);
	#endif	
  }
}

void show_content(u8 cursor, P_Page_TypeDef p_page)
{
  u8 y;
  u8 **page_coment = p_page->Page_content;
  for (y = 0; y < 8; y++)
  {
    if (y == p_page->Page_H)
      break; 
    if (Cursor < 8)
    {
      if (y == Cursor)
      {
        OLED_ShowString(0, y, *(page_coment + y), 0); 
      }
      else
		  OLED_ShowString(0 , y , *(page_coment + y) , 1);       
      if (p_page->page_type == DIG)
      {
        if (p_page->param_type[y] == FLOAT)
          OLED_PrintVar(90, y, *((float *)(p_page->param[y])), 6, 1);
        else if(p_page->param_type[y] == INT16)
		  OLED_PrintVar(90, y, *((int16_t *)(p_page->param[y])), 6, 1);
        else if(p_page->param_type[y] == UINT16)
		  OLED_PrintVar(90, y, *((uint16_t *)(p_page->param[y])), 6, 1);
        else if(p_page->param_type[y] == INT32)
		  OLED_PrintVar(90, y, *((int32_t *)(p_page->param[y])), 6, 1);
        else if(p_page->param_type[y] == UINT32)
		  OLED_PrintVar(90, y, *((uint32_t *)(p_page->param[y])), 6, 1);		
        else if(p_page->param_type[y] == INT8)
		  OLED_PrintVar(90, y, *((int8_t *)(p_page->param[y])), 6, 1);				
		else
          OLED_PrintVar(90, y, *((uint8_t *)(p_page->param[y])), 6, 1);
      }
    }
    else
    {
      if (y == 7)
      {
        OLED_ShowString(0, y, *(page_coment + Cursor - 7 + y), 0);
      }
      else
        OLED_ShowString(0, y, *(page_coment + Cursor - 7 + y), 1);
      if (p_page->page_type == DIG)
      {
        if (p_page->param_type[Cursor - 7 + y] == FLOAT)
          OLED_PrintVar(90, y, *((float *)(p_page->param[Cursor - 7 + y])), 6, 1);
        else
          OLED_PrintVar(90, y, *((uint32_t *)(p_page->param[Cursor - 7 + y])), 6, 1);
      }
    }
  }
}

void  (*mainpage_callback)(void); //声明指向上面函数原型的指针
/*主页面回调函数设置*/
void set_mainpage_callback(void  (*user_callback)(void))
{
	mainpage_callback = user_callback;    //向函数指针赋值
}

void UI_show(P_Page_TypeDef p_page)
{
  NowPage = 2; 
  Cursor = 0; 
  Menustatus = 0;
  OLED_Clear(); 

  while (1)
  {
    Key_Event(); 
    if (Cursor == p_page->Page_H)
      Cursor = 0; 
    if (Cursor < 0)
      Cursor = p_page->Page_H - 1;
		
    if (NowPage == 1)
    {
      if (p_page->is_mainmenu != 1)
      {
        OLED_Clear();
        break;
      }else{
		NowPage = 2;
	  }
    }
    else if (NowPage == 3)
    {
      if (p_page->page_type == DIG)
        ChangeData(p_page, Cursor, 2, p_page->param);
      else
      {
        NowPage = 2;
        Cursor_last = Cursor;
        OLED_Clear();
        p_page->func[Cursor]();
        Cursor = Cursor_last; 
      }
    }

    if (p_page->page_type != SHOW)
      show_content(Cursor, p_page);
    else
      p_page->show_func();
#ifdef USE_OS
    task_delay_ms(20);
#endif
  }
}
//**************************************************************************************************


//*****************************************数据保存部分**********************************************
dnode_t	dhead_flash;

void	flash_list_init()
{
	dnode_head_init(&dhead_flash);
}

void	flash_list_add(datatype_TypeDef datatype, void * p_data)
{
	p_flash_list	pr;
	
	pr		= (p_flash_list)MEMORY_GET(SRAMIN,sizeof(flash_list_t));
	pr->datatype = datatype;
	if(pr->datatype == FLOAT)
		pr->p_float = (float *)p_data;
	else
		pr->p_uint32_t = (uint32_t *)p_data;
	dnode_add_last(&dhead_flash, &pr->dnd);
	
}

static uint32_t Float2U32(float data)    
{
  u8 * point;
  point = (u8*)(&data);
  return (((uint32_t)point[3] << 24) | ((uint32_t)point[2] << 16) | ((uint32_t)point[1] << 8) | ((uint32_t)point[0]));
}


static float U322Float(uint32_t * data)   
{
  return (*((float *)data));
}

void data_save()
{
	p_dnode curr;
	p_flash_list pr;
	curr	= dhead_flash.next;
	uint32_t data_save_buff[FLASH_BUFF_LENTH];
	uint32_t data_num = 0;
	
	while(curr != &dhead_flash)//遍历所有resolve
	{
		pr	= CONTAINER_OF(curr, flash_list_t, dnd);
		if(pr->datatype == FLOAT)
			*(data_save_buff + data_num) = Float2U32(*pr->p_float);
		else
			*(data_save_buff + data_num) = *pr->p_uint32_t;
		data_num++;
		curr	= curr->next;
	}
	//eeprom_write(OFFSET_START,data_save_buff,data_num);
	Flash_EraseSector(FLASH_Sector_10);
	Flash_Write32BitDatas(ADDR_FLASH_SECTOR_10,data_num,(s32*) data_save_buff);	

}

void data_read()
{
	p_dnode curr;
	p_flash_list pr;
	curr	= dhead_flash.next;
	uint32_t data_read_buff[FLASH_BUFF_LENTH];
	uint32_t data_num = 0;

	//eeprom_read(OFFSET_START,data_read_buff,FLASH_BUFF_LENTH);
	Flash_Read32BitDatas(ADDR_FLASH_SECTOR_10,FLASH_BUFF_LENTH,(s32*)data_read_buff);
	
	while(curr != &dhead_flash)//遍历所有resolve
	{
		pr	= CONTAINER_OF(curr, flash_list_t, dnd);
		
		if(pr->datatype == FLOAT)
			*pr->p_float = U322Float((data_read_buff + data_num));
		else
			*pr->p_uint32_t = data_read_buff[data_num];
		
		data_num++;
		
		curr	= curr->next;
	}	
}
//*************************************************************************************************
