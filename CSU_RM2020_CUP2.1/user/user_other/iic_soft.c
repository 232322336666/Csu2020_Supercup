/**
  * @file      iic.c
  * @brief     ģ��IIC
  * @auther    zhiyu
  * @date      2019.02.07
  * @opyright  CSU RM.All rights reserved.
  */

#include "iic_soft.h"
//#include "delay.h"

/**
  * @brief      ����IIC����GPIO
  * @param      ��
  * @note       ��
  * @retval     ��
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
  * @brief      ����IIC1���������ź�
  * @param      ��
  * @note       ��
  * @retval     ��
  */
void IIC1_Start(void)
{
	IIC1_SDA_OUT();
	IIC1_SDA_1;
	IIC1_SCL_1;  //��SCLΪ��ʱ��SDA�����½��ر�ʾIIC��������
	IIC_Delay;
	IIC1_SDA_0;
	IIC_Delay;
	IIC1_SCL_0;
}

/**
  * @brief      ����IIC����ֹͣ�ź�
  * @param      ��
  * @note       ��
  * @retval     ��
  */
void IIC1_Stop(void)
{
	IIC1_SDA_OUT();
	IIC1_SCL_0;
	IIC1_SDA_0;
	IIC_Delay;
	IIC1_SCL_1;
	IIC_Delay;
	IIC1_SDA_1;  //��SCLΪ��ʱ��SDA���������ر�ʾIIC����ֹͣ
}

//����8λ����
/**
  * @brief      ����8λ����
  * @param      ucByte:�ȴ����͵��ֽ�
  * @note       ��
  * @retval     ��
  */
void IIC1_SendByte(u8 ucByte)
{
	u8 i;
	
	IIC1_SDA_OUT();
	
	IIC1_SCL_0;  //����ʱ�ӿ�ʼ���ݴ���
	
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
			IIC1_SDA_1;  //�ͷ�����
		}
		ucByte <<= 1;  //����һλ
		IIC_Delay;
	}
}

//void IIC1_SendByte(u8 ucByte)
//{                        
//    u8 t;   
//	IIC1_SDA_OUT();    
//	IIC1_SCL_0; //����ʱ�ӿ�ʼ���ݴ���
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
//		IIC_Delay;   //��TEA5767��������ʱ���Ǳ����
//		IIC1_SCL_1;
//		IIC_Delay;
//		IIC1_SCL_0;	
//		IIC_Delay;
//    }	 
//} 	
/**
  * @brief      ��ȡ8λ����
  * @param      ��
  * @note       ��
  * @retval     ����������
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
  * @brief      ��ȡACKӦ���ź�
  * @param      ��
  * @note       ��
  * @retval     ����0��ʾ��ȷӦ�𣬷���1��ʾ����Ӧ
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
  * @brief      ����һ��Ack�ź�
  * @param      ��
  * @note       ��
  * @retval     ��
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
  * @brief      ����һ��NAck�ź�
  * @param      ��
  * @note       ��
  * @retval     ��
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
  * @brief      ���͵�ַ����������豸����ȡ�豸Ӧ���ж��豸�Ƿ���ڣ�����ʼ��GPIO
  * @param      Address:�豸���ߵ�ַ
  * @note       ϵͳʹ��ģ��IIC����ǰ�������
  * @retval     ����0��ʾ��ȷ������1��ʾδ��⵽
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











