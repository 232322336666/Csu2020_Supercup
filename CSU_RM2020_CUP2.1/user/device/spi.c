#include "spi.h"
#include "headfile.h"
uint16_t SPI2_ReadWriteByte(uint16_t TxData);

void SPI2_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //使能SPI2时钟
	
	GPIO_InitStructure.GPIO_Pin = spi2_NSS_PIN;    //PB12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         //上拉
	GPIO_Init(spi2_GPIO, &GPIO_InitStructure);     //初始化
 
	GPIO_InitStructure.GPIO_Pin = spi2_SCK_PIN|spi2_MISO_PIN|spi2_MOSI_PIN;//PB13~15复用功能输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                             //复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                           //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                       //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                             //上拉
	GPIO_Init(spi2_GPIO, &GPIO_InitStructure);                                         //初始化   
	
	GPIO_PinAFConfig(spi2_SCK_GPIO,GPIO_PinSource13,GPIO_AF_SPI2);   //PB13复用为 SPI2
	GPIO_PinAFConfig(spi2_MISO_GPIO,GPIO_PinSource14,GPIO_AF_SPI2);  //PB14复用为 SPI2
	GPIO_PinAFConfig(spi2_MOSI_GPIO,GPIO_PinSource15,GPIO_AF_SPI2);  //PB15复用为 SPI2
 
	//这里只针对SPI口初始化
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);  //复位SPI1
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE); //停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                    //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		                //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                      //串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                        //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//42M/128=328.125KHZ定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                          //CRC值计算的多项式(未启用)
	SPI_Init(SPI2, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE);   //使能SPI外设	 	 
}   
/*******************************************************************************
	SPI2 读写一个字节
	函数名称：SPI1_ReadWriteByte
	输入参数：TxData
	输出参数：读取到的字节
*******************************************************************************/
uint16_t SPI2_ReadWriteByte(uint16_t TxData)
{		 			 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个byte  数据	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	    
}

int16_t mems_gyro,mems_yaw;
float mems_gyro_f,mems_yaw_f;
void mems_read_gyro_and_yaw()
{
	mems_ENABLE;
	SPI2_ReadWriteByte(0x0400);//写入读命令和角速度寄存器地址
	delay_us(9);
	mems_gyro=SPI2_ReadWriteByte(0x0e00);//写入读命令和角度寄存器地址并读取角速度
	delay_us(9);
	mems_yaw=SPI2_ReadWriteByte(0x0400);//读取角度
	mems_DISENABLE;
	mems_gyro&=0X3FFF;
	mems_yaw&=0X3FFF;
	if(mems_gyro&0X2000)
	{
		mems_gyro|=0XC000;
	}
	mems_gyro_f=mems_gyro*0.07326;
	mems_yaw_f=mems_yaw*0.03663;
}
int16_t mems_read_and_write(int16_t data)
{
	int16_t res;
	mems_ENABLE;
	res=SPI2_ReadWriteByte(data);
	mems_DISENABLE;
	delay_us(9);
	return res;
}
void mems_offset(s16 data)
{
	int16_t temp_data;
	data&=0xfff;
	mems_ENABLE;
	temp_data=0x94<<8|(data&0xff);//低八位
	SPI2_ReadWriteByte(temp_data);
	delay_us(9);
	temp_data=0x95<<8|(data>>8);//高八位
	SPI2_ReadWriteByte(temp_data);
	mems_DISENABLE;
	delay_us(9);
}

