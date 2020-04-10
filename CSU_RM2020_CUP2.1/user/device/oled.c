#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "stdarg.h"

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

//��SSD1106д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd,u8 type)
{	
	u8 i;			  
	if(!type)
	{
		dat = ~dat;
	}
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD,1);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD,1);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD,1); 
}   	  

//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD,1);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD,1);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD,1);  //DISPLAY ON
}

//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD,1);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD,1);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD,1);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD,1);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD,1);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD,1);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA,1); 
	} //������ʾ
}


//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 type)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
		{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
				OLED_WR_Byte(F8X16[c*16+i],OLED_DATA,type);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
				OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA,type);
		}
		else 
		{	
			OLED_Set_Pos(x,y+1);
			for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA,type);				
		}
}
//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 type)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',type);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',type); 
	}
} 
//��ʾһ���ַ��Ŵ�
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 type)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{	
		if(!type)
			OLED_ShowChar(x,y,chr[j],0);
		else
			OLED_ShowChar(x,y,chr[j],1);
		x+=6;
		if(x>122){x=0;y+=2;}
			j++;
	}
}

/*
??:?????
??:
*/
void OLED_PrintVar(uint8_t x, uint8_t y, double data, uint8_t Length, uint8_t num)
{
  uint8_t StrTemp[20] = {0};
  uint8_t Site = 0;
  int bit = 0;
  int32_t datatemp = 0;
   
  while(Site < num)
  {
    data *= 10;
    Site++;
  }
  datatemp = (int32_t)data;
  
  Site = 0;
  if(datatemp < 0)
  {
    StrTemp[Site] = '-';
    datatemp = -datatemp;
    Site++;   
  }
  else if(datatemp == 0)
  {
    StrTemp[Site] = '0';
    Site++;
  }
  
  bit = 0;
  int32_t d = datatemp;
  while(d > 0)
  {
    d /= 10;
    bit++;                   /////?????????
  }
  
  uint8_t dotsite = 0; ////?????
  if(bit <= num)
  {
    StrTemp[Site] = '0';
    Site++;
    dotsite = 0;
    bit = num;
  }
  else
  {
    dotsite = bit - num;
   // bit;
  }
  
  StrTemp[Site + bit + 1] = '\0';
  while(bit >= 0)
  {
    if(bit == dotsite)
    {
      StrTemp[Site + bit] = '.';
    }
    else
    {
      StrTemp[Site + bit] = datatemp % 10 + '0';
      datatemp /= 10;
    }
    bit--;
  }
  OLED_ShowString(x, y, StrTemp, 1);
}

//��ʾ����
void OLED_ShowCHinese(u8 x,u8 y,u8 no,u8 type)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA,type);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA,type);
				adder+=1;
      }					
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[],u8 type)
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA,type);	    	
	    }
	}
} 

//��ʾ��״����
#define AD_END   90//��״��ֹλ��
#define AD_MAX  3000 
void OLED_PrintAD(uint8_t x, uint8_t y, int16_t data, int16_t DataMax)
{
   uint8_t i; 
   float DataL = AD_END - x;
   OLED_Set_Pos(x, y);
   DataL *= (float)data /(float) DataMax;
   DataL += x;
   if(DataL >= AD_END)DataL = AD_END;
   for(i = x; i <= (uint8_t)DataL; i++)
     //OLED_WriteData(0x00, 0);
     OLED_WR_Byte(0x00,OLED_DATA,0);
   for(i = (uint8_t)DataL; i <= AD_END; i++)
     //OLED_WriteData(0xff, 0);
     OLED_WR_Byte(0xff,OLED_DATA,0);
  
   //*****����׼��******

   DataL = AD_END - x;//�ܳ���  
   DataL *= (float)AD_MAX / (float)DataMax;//���ݶ�Ӧ�ĳ���
   DataL += x;//���ݶ�Ӧ����ֹλ��
   OLED_Set_Pos(DataL, y);
   //OLED_WriteData(0x55, 0);
   OLED_WR_Byte(0x55,OLED_DATA,0);
}


//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	 //ʹ��PC,D,G�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;	 //PD3,PD6�������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
 	GPIO_SetBits(GPIOD,GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);	//PD3,PD6 �����

	OLED_SCLK_Set();
	OLED_RST_Clr();
	delay_ms(50);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD,1);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD,1);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD,1);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD,1);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD,1);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD,1); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD,1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD,1);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD,1);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD,1);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD,1);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD,1);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD,1);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD,1);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD,1);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD,1);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD,1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD,1);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD,1);
	OLED_WR_Byte(0xDB,OLED_CMD,1);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD,1);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD,1);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD,1);//
	OLED_WR_Byte(0x8D,OLED_CMD,1);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD,1);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD,1);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD,1);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD,1);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD,1); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  

/*OLED�ض���*/
signed int OLED_printf(uint8_t x,uint8_t y,const char *pFormat, ...){
 
    unsigned char pStr[25] = {'\0'}; 
    va_list ap;
    signed int result;
 
    // Forward call to vprintf
    va_start(ap, pFormat);
    result = vsprintf((char *)pStr, pFormat, ap);
    va_end(ap);
 
    OLED_ShowString(x,y,pStr,1);//Ĭ��������ɫ��ʾ�ַ���
 
    return result;
}
