//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : OLED 4�ӿ���ʾ����(51ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��PD6��SCL��
//              D1   ��PD7��SDA��
//              RES  ��PD4
//              DC   ��PD5
//              CS   ��PD3               
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2014/3/16
//All rights reserved
//******************************************************************************/
#ifndef __OLED_H
#define __OLED_H			  	 
#include "stdlib.h"	  
#include "headfile.h"

#define USE_OS

#ifdef USE_OS 
#define delay_ms(x)	task_delay_ms(x)
#endif

//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ
//#define OLED_MODE 0
#define SIZE 8
//#define XLevelL		0x00
//#define XLevelH		0x10
#define Max_Column	128
//#define Max_Row		64
//#define	Brightness	0xFF 
//#define X_WIDTH 	128
//#define Y_WIDTH 	64	    						  
//-----------------OLED�˿ڶ���----------------

#define OLED_CS_Clr()  {}
#define OLED_CS_Set()  {}

//#define OLED_RST_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_13)  //OLED4
//#define OLED_RST_Set() GPIO_SetBits(GPIOD,GPIO_Pin_13)

//#define OLED_DC_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_10) //OLED1
//#define OLED_DC_Set() GPIO_SetBits(GPIOD,GPIO_Pin_10)

//#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_12)//OLED3
//#define OLED_SCLK_Set() GPIO_SetBits(GPIOD,GPIO_Pin_12)

//#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_11)//OLED2
//#define OLED_SDIN_Set() GPIO_SetBits(GPIOD,GPIO_Pin_11)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_13)  //OLED4
#define OLED_SDIN_Set() GPIO_SetBits(GPIOD,GPIO_Pin_13)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_10) //OLED1
#define OLED_RST_Set() GPIO_SetBits(GPIOD,GPIO_Pin_10)

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_12)//OLED3
#define OLED_SCLK_Set() GPIO_SetBits(GPIOD,GPIO_Pin_12)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_11)//OLED2
#define OLED_DC_Set() GPIO_SetBits(GPIOD,GPIO_Pin_11)	
 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd,u8 type);   
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 type);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 type);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 type);
void OLED_PrintVar(uint8_t x, uint8_t y, double data, uint8_t Length, uint8_t num);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no,u8 typr);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[],u8 type);
void OLED_PrintAD(uint8_t x, uint8_t y, int16_t data, int16_t DataMax);
signed int OLED_printf(uint8_t x,uint8_t y,const char *pFormat, ...);
#endif  

