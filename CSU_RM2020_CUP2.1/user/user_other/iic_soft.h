#ifndef __IIC_SOFT_H
#define __IIC_SOFT_H

//#include <inttypes.h>
//#include "delay.h"
#include "stm32f4xx.h"
#include "headfile.h"
//IO方向设置
//#define IIC1_SDA_IN()  {GPIOB->MODER&=~(((uint32_t)3)<<(GPIO_Pin_7*2));GPIOB->MODER|= (((uint32_t)0)<<(GPIO_Pin_7 * 2));}  //上下拉输入模式
//#define IIC1_SDA_OUT() {GPIOB->MODER&=~(((uint32_t)3)<<(GPIO_Pin_7*2));GPIOB->MODER|= (((uint32_t)1)<<(GPIO_Pin_7 * 2));}  //推挽输出模式

//#define IIC1_SDA_IN()  {GPIOA->MODER&= 0xffff3fff ;} //上下拉输入模式
//#define IIC1_SDA_OUT() {GPIOA->MODER|= 0x00007000 ;}  //推挽输出模式

//#define IIC1_SDA_IN()  {GPIOA->MODER&=~(3<<(2*7));GPIOA->MODER|=0<<2*7;}  //上下拉输入模式
//#define IIC1_SDA_OUT() {GPIOA->MODER&=~(3<<(2*7));GPIOA->MODER|=1<<2*7;}  //推挽输出模式

//IO输出
#define IIC1_SCL_1 GPIO_SetBits(GPIOB, GPIO_Pin_6)    //SCL = 1
#define IIC1_SCL_0 GPIO_ResetBits(GPIOB, GPIO_Pin_6)  //SCL = 0
#define IIC1_SDA_1 GPIO_SetBits(GPIOB, GPIO_Pin_7)    //SDA = 1
#define IIC1_SDA_0 GPIO_ResetBits(GPIOB, GPIO_Pin_7)  //SDA = 0

//IO输入
#define IIC1_SDA_READ GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)  //读取SDA线状态

#define IIC_Delay   delay_us(2)

void IIC1_GPIO_Init(void);
void IIC1_Start(void);
void IIC1_Stop(void);
void IIC1_SendByte(u8 ucByte);
u8 IIC1_ReadByte(void);
u8 IIC1_WaitAck(void);
void IIC1_Ack(void);
void IIC1_NAck(void);
u8 IIC_CheckDevice(u8 Address);

#endif





