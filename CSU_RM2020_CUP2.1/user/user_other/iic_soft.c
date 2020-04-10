/**
  * @file      iic.c
  * @brief     模拟IIC
  * @auther    zhiyu
  * @date      2019.02.07
  * @opyright  CSU RM.All rights reserved.
  */

#include "iic_soft.h"
//#include "delay.h"

/**
  * @brief      配置IIC总线GPIO
  * @param      无
  * @note       无
  * @retval     无
  */
void IIC1_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
    //PB6: I2C1_SCL  PB7: I2C1_SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_7 | GPIO_Pin_6);
}
GPIO_InitTypeDef GPIO_InitStructure;
void IIC1_SDA_IN(void)
{
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC1_SDA_OUT(void)
{
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief      发起IIC1总线启动信号
  * @param      无
  * @note       无
  * @retval     无
  */
void IIC1_Start(void)
{
	IIC1_SDA_OUT();
	IIC1_SDA_1;
	IIC1_SCL_1;  //当SCL为高时，SDA出现下降沿表示IIC总线启动
	IIC_Delay;
	IIC1_SDA_0;
	IIC_Delay;
	IIC1_SCL_0;
}

/**
  * @brief      发起IIC总线停止信号
  * @param      无
  * @note       无
  * @retval     无
  */
void IIC1_Stop(void)
{
	IIC1_SDA_OUT();
	IIC1_SCL_0;
	IIC1_SDA_0;
	IIC_Delay;
	IIC1_SCL_1;
	IIC_Delay;
	IIC1_SDA_1;  //当SCL为高时，SDA出项上升沿表示IIC总线停止
}

//发送8位数据
/**
  * @brief      发送8位数据
  * @param      ucByte:等待发送的字节
  * @note       无
  * @retval     无
  */
void IIC1_SendByte(u8 ucByte)
{
	u8 i;
	
	IIC1_SDA_OUT();
	
	IIC1_SCL_0;  //拉低时钟开始数据传输
	
	delay_us(3);
	
	for(i = 0; i < 8; i++)
	{
		if(ucByte & 0x80)
		{
			IIC1_SDA_1;
		}
		else
		{
			IIC1_SDA_0;
		}
		IIC_Delay;
		IIC1_SCL_1;
		IIC_Delay;
		IIC1_SCL_0;
		if(i == 7)
		{
			IIC1_SDA_IN();
			IIC1_SDA_1;  //释放总线
		}
		ucByte <<= 1;  //左移一位
		IIC_Delay;
	}
}

//void IIC1_SendByte(u8 ucByte)
//{                        
//    u8 t;   
//	IIC1_SDA_OUT();    
//	IIC1_SCL_0; //拉低时钟开始数据传输
//    for(t=0;t<8;t++)
//    {              
// 		if(ucByte & 0x80)
//		{
//			IIC1_SDA_1;
//		}
//		else
//		{
//			IIC1_SDA_0;
//		}
//        ucByte<<=1; 	  
//		IIC_Delay;   //对TEA5767这三个延时都是必须的
//		IIC1_SCL_1;
//		IIC_Delay;
//		IIC1_SCL_0;	
//		IIC_Delay;
//    }	 
//} 	
/**
  * @brief      读取8位数据
  * @param      无
  * @note       无
  * @retval     读到的数据
  */
u8 IIC1_ReadByte(void)
{
	u8 i;
	u8 value = 0;
	
	IIC1_SDA_IN();

	for(i = 0; i < 8; i++)
	{
		IIC1_SCL_0;
		IIC_Delay;
		IIC1_SCL_1;
		value <<= 1;
		if(IIC1_SDA_READ)
		{
			value++;
		}
		
		IIC_Delay;
	}
	return value;
}

/**
  * @brief      读取ACK应答信号
  * @param      无
  * @note       无
  * @retval     返回0表示正确应答，返回1表示无响应
  */
u8 IIC1_WaitAck(void)
{
	u8 i = 0;
	
	IIC1_SDA_IN();
	IIC1_SDA_1;
	delay_us(1);
	IIC1_SCL_1;
	delay_us(1);
	while(IIC1_SDA_READ)
	{
		i++;
		if(i > 250)
		{
			IIC1_Stop();
			return 1;
		}
	}
	IIC1_SCL_0;
	return 0;
}

/**
  * @brief      产生一个Ack信号
  * @param      无
  * @note       无
  * @retval     无
  */
void IIC1_Ack(void)
{
	IIC1_SCL_0;
	IIC1_SDA_OUT();
	IIC1_SDA_0;
	IIC_Delay;
	IIC1_SCL_1;
	IIC_Delay;
	IIC1_SCL_0;
}

/**
  * @brief      产生一个NAck信号
  * @param      无
  * @note       无
  * @retval     无
  */
void IIC1_NAck(void)
{
	IIC1_SCL_0;
	IIC1_SDA_OUT();
	IIC1_SDA_1;
	IIC_Delay;
	IIC1_SCL_1;
	IIC_Delay;
	IIC1_SCL_0;
}


/**
  * @brief      发送地址，检测总线设备，读取设备应答判断设备是否存在，并初始化GPIO
  * @param      Address:设备总线地址
  * @note       系统使用模拟IIC总线前必须调用
  * @retval     返回0表示正确，返回1表示未检测到
  */
u8 IIC_CheckDevice(u8 Address)
{
	u8 ucAck;
	
	IIC1_Start();
	
	IIC1_SendByte(Address);
	ucAck = IIC1_WaitAck();
	
	IIC1_Stop();
	
	return ucAck;
}











