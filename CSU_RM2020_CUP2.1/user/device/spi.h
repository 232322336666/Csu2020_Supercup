#ifndef _SPI_H
#define _SPI_H

#include "makos_includes.h"

#define mems_ENABLE 	          GPIO_ResetBits(GPIOB, GPIO_Pin_12)    //片选有效
#define mems_DISENABLE          GPIO_SetBits(GPIOB, GPIO_Pin_12)      //片选无效

/*SPI2相关引脚定义*/
#define spi2_GPIO               GPIOB                   
#define spi2_GPIO_CLK           RCC_AHB1Periph_GPIOB       

#define spi2_NSS_GPIO           GPIOB
#define spi2_NSS_GPIO_CLK       RCC_AHB1Periph_GPIOB
#define spi2_NSS_PIN            GPIO_Pin_12

#define spi2_SCK_GPIO           GPIOB
#define spi2_SCK_GPIO_CLK       RCC_AHB1Periph_GPIOB
#define spi2_SCK_PIN            GPIO_Pin_13

#define spi2_MISO_GPIO          GPIOB
#define spi2_MISO_GPIO_CLK      RCC_AHB1Periph_GPIOB
#define spi2_MISO_PIN           GPIO_Pin_14

#define spi2_MOSI_GPIO          GPIOB
#define spi2_MOSI_GPIO_CLK      RCC_AHB1Periph_GPIOB
#define spi2_MOSI_PIN           GPIO_Pin_15

extern int16_t mems_gyro,mems_yaw;
extern float mems_gyro_f,mems_yaw_f;

void SPI2_Init(void);
void mems_read_gyro_and_yaw(void);
int16_t mems_read_and_write(int16_t data);
void mems_offset(s16 data);

#endif
