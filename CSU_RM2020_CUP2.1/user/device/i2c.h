/* #include "stm32f4_i2c.h" */

#ifndef __STM32F4_I2C_H
#define __STM32F4_I2C_H
/*=====================================================================================================*/
#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
//器件地址
#define INA226_1_ADDR  0x80  //充电监控
#define INA226_2_ADDR  0x82  //电池输出监控
#define INA226_3_ADDR  0x88  //底盘实际功率监控

//寄存器地址
#define Config_Reg_ADDR        0x00
#define ShuntVoltage_Reg_ADDR  0x01
#define BusVoltage_Reg_ADDR    0x02
#define Power_Reg_ADDR         0x03
#define Current_Reg_ADDR       0x04
#define Calibration_Reg_ADDR   0x05

void INA226_1_Init(void);
void INA226_2_Init(void);
void INA226_3_Init(void);
	
void INA226_Reg_Write(u8 Device_Address, u8 reg_Address, u16 reg_Data);
int16_t  INA226_ReadData(u8 Device_Address, u8 reg_Address);
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
