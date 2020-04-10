#include "spi.h"
#include "headfile.h"
uint16_t SPI2_ReadWriteByte(uint16_t TxData);

void SPI2_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //ʹ��SPI2ʱ��
	
	GPIO_InitStructure.GPIO_Pin = spi2_NSS_PIN;    //PB12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         //����
	GPIO_Init(spi2_GPIO, &GPIO_InitStructure);     //��ʼ��
 
	GPIO_InitStructure.GPIO_Pin = spi2_SCK_PIN|spi2_MISO_PIN|spi2_MOSI_PIN;//PB13~15���ù������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                             //���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                           //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                       //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                             //����
	GPIO_Init(spi2_GPIO, &GPIO_InitStructure);                                         //��ʼ��   
	
	GPIO_PinAFConfig(spi2_SCK_GPIO,GPIO_PinSource13,GPIO_AF_SPI2);   //PB13����Ϊ SPI2
	GPIO_PinAFConfig(spi2_MISO_GPIO,GPIO_PinSource14,GPIO_AF_SPI2);  //PB14����Ϊ SPI2
	GPIO_PinAFConfig(spi2_MOSI_GPIO,GPIO_PinSource15,GPIO_AF_SPI2);  //PB15����Ϊ SPI2
 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);  //��λSPI1
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE); //ֹͣ��λSPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                    //����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		                //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                      //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                        //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//42M/128=328.125KHZ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                          //CRCֵ����Ķ���ʽ(δ����)
	SPI_Init(SPI2, &SPI_InitStructure);                                 //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE);   //ʹ��SPI����	 	 
}   
/*******************************************************************************
	SPI2 ��дһ���ֽ�
	�������ƣ�SPI1_ReadWriteByte
	���������TxData
	�����������ȡ�����ֽ�
*******************************************************************************/
uint16_t SPI2_ReadWriteByte(uint16_t TxData)
{		 			 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	    
}

int16_t mems_gyro,mems_yaw;
float mems_gyro_f,mems_yaw_f;
void mems_read_gyro_and_yaw()
{
	mems_ENABLE;
	SPI2_ReadWriteByte(0x0400);//д�������ͽ��ٶȼĴ�����ַ
	delay_us(9);
	mems_gyro=SPI2_ReadWriteByte(0x0e00);//д�������ͽǶȼĴ�����ַ����ȡ���ٶ�
	delay_us(9);
	mems_yaw=SPI2_ReadWriteByte(0x0400);//��ȡ�Ƕ�
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
	temp_data=0x94<<8|(data&0xff);//�Ͱ�λ
	SPI2_ReadWriteByte(temp_data);
	delay_us(9);
	temp_data=0x95<<8|(data>>8);//�߰�λ
	SPI2_ReadWriteByte(temp_data);
	mems_DISENABLE;
	delay_us(9);
}

