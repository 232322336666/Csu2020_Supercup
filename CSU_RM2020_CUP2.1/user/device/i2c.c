#include "iic_soft.h"
//#include "ina226.h"
#include "usart.h"
#include "i2c.h"
/**
  * @brief      Ð´INA226µÄÅäÖÃ¼Ä´æÆ÷
  * @param      reg_Address:¼Ä´æÆ÷µØÖ·£»reg_Data : Ð´¼Ä´æÆ÷Êý¾Ý
  * @note       ÅäÖÃ¼Ä´æÆ÷µØÖ·Îª ¡°00h¡±£¬16Î»£¬ÅäÖÃÐÅÏ¢·ÖÁ½´Î·¢ËÍ
  * @retval     ÎÞ
  */
 void INA226_Reg_Write(u8 Device_Address, u8 reg_Address, u16 reg_Data)
{
	u8 data_temp[2];
	data_temp[0] = (u8) ((reg_Data & 0xff00) >> 8);  //È¡³ö¸ß8Î»
	data_temp[1] = (u8) (reg_Data & 0xff);           //È¡³öµÍ8Î»
	
	IIC1_Start();
	IIC1_SendByte(Device_Address | 0);  //·¢ËÍINA226Æ÷¼þµØÖ·£¬¸ß7Î»ÊÇµØÖ·£¬µÚ0Î»ÊÇ¶ÁÐ´¿ØÖÆÎ»£¬0±íÊ¾Ð´£¬1±íÊ¾¶Á
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("reg erro\n\n");
		return;
	}
	
	IIC1_SendByte(reg_Address);
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("reg erro\n\n");
		return;
	}
	
	IIC1_SendByte(data_temp[0]);  //·¢ËÍ¸ß8Î»
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("reg erro\n\n");
		return;
	}
	
	IIC1_SendByte(data_temp[1]);  //·¢ËÍµÍ8Î»
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("reg erro\n\n");
		return;
	}
	
	printf("reg ok\n\n");
	IIC1_Stop();  //ÅäÖÃ¼Ä´æÆ÷Ð´Íê±Ï£¬Í£Ö¹IIC×ÜÏß
}

/**
  * @brief      ´ÓINA226¶ÁÈ¡Êý¾Ý
  * @param      reg_Address:Êý¾Ý¼Ä´æÆ÷µØÖ·£»
  * @note       Êý¾Ý¼Ä´æÆ÷ÊÇ16Î»£¬·ÖÁ½´Î¶ÁÈ¡
  * @retval     ¶ÁÈ¡µÄÊý¾Ý
  */
int16_t INA226_ReadData(u8 Device_Address, u8 reg_Address)
{
u8 data_temp[2];
	int16_t temp;
	
	IIC1_Start();
	
	IIC1_SendByte(Device_Address);  //·¢ËÍINA226Æ÷¼þµØÖ·
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("num:erro\n\n");
		return 0;
	}

	IIC1_SendByte(reg_Address);  //·¢ËÍÊý¾Ý¼Ä´æÆ÷µØÖ·
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("num:erro\n\n");
		return 0;
	}
	
	IIC1_Start();

	IIC1_SendByte(Device_Address | 1);  //·¢ËÍINA226Æ÷¼þµØÖ·£¬¸ß7Î»ÊÇµØÖ·£¬µÚ0Î»ÊÇ¶ÁÐ´¿ØÖÆÎ»£¬0±íÊ¾Ð´£¬1±íÊ¾¶Á
	
	if(IIC1_WaitAck() != 0)
	{
		IIC1_Stop();  //Æ÷¼þÎÞÓ¦´ð£¬Í£Ö¹IIC×ÜÏß
		printf("num:erro\n\n");
		return 0;
	}
	
	data_temp[1] = IIC1_ReadByte();  //¸ß8Î»
	
	IIC1_Ack();
	
	data_temp[0] = IIC1_ReadByte();  //µÍ8Î»
	
	IIC1_NAck();
	
	IIC1_Stop();
	
	temp = (int16_t) (data_temp[0] + (data_temp[1] << 8));
	//printf("Êý¾ÝÎª£º%x\n", temp);
	return  temp;
	
}

//**********³äµçµçÁ÷£¬µçÈÝµçÑ¹£¬³äµç¹¦ÂÊ¼à¿Ø**********
/**
  * @brief      ÅÐ¶Ïina226_1ÊÇ·ñÕý³£¹¤×÷
  * @param      ÎÞ
  * @note       ÎÞ
  * @retval     1±íÊ¾Õý³££¬0±íÊ¾²»Õý³£
  */
void INA226_1_Init(void)
{
	IIC1_GPIO_Init();  //ÅäÖÃGPIO
	if(IIC_CheckDevice(INA226_1_ADDR) == 0)
	{
		printf("INA226_1 found\n\n");
		INA226_Reg_Write(INA226_1_ADDR, Config_Reg_ADDR, 0x8000);  //INA226¸´Î»£¬ËùÓÐ¼Ä´æÆ÷»Ö¸´³ÉÄ¬ÈÏÉèÖÃ
		INA226_Reg_Write(INA226_1_ADDR, Config_Reg_ADDR, 0x4007);  //ÉèÖÃ¹¤×÷Ä£Ê½£ºÆ½¾ùÑù±¾Êý£º1£»×ÜÏßµçÑ¹×ª»»Ê±¼ä£º1.1ms£»·ÖÁ÷µçÑ¹×ª»»Ê±¼ä£º1.1ms£»Ä£Ê½£ºµçÁ÷µçÑ¹Á¬ÐøÄ£Ê½
		INA226_Reg_Write(INA226_1_ADDR, Calibration_Reg_ADDR, 0x0800);  //ÉèÖÃÐ£×¼¼Ä´æÆ÷£¬500uA/bit£¬×î´ó¼ì²âµçÁ÷10A
	}
	else
	{
		IIC1_Stop();  //Èô¼ì²âÊ§°Ü£¬·¢ËÍÍ£Ö¹ÐÅºÅ
		printf("nofound ina226_1\n\n");
	}
}

//**********µç³ØÊä³öµçÑ¹£¬µçÁ÷£¬¹¦ÂÊ¼à¿Ø**********
/**
  * @brief      ÅÐ¶Ïina226_2ÊÇ·ñÕý³£¹¤×÷
  * @param      ÎÞ
  * @note       ÎÞ
  * @retval     1±íÊ¾Õý³££¬0±íÊ¾²»Õý³£
  */
void INA226_2_Init(void)
{
	IIC1_GPIO_Init();  //ÅäÖÃGPIO
	if(IIC_CheckDevice(INA226_2_ADDR) == 0)
	{
		printf("INA226_2 found¦\n\n");
		INA226_Reg_Write(INA226_2_ADDR, Config_Reg_ADDR, 0x8000);  //INA226¸´Î»£¬ËùÓÐ¼Ä´æÆ÷»Ö¸´³ÉÄ¬ÈÏÉèÖÃ
		INA226_Reg_Write(INA226_2_ADDR, Config_Reg_ADDR, 0x4007);  //ÉèÖÃ¹¤×÷Ä£Ê½£ºÆ½¾ùÑù±¾Êý£º1£»×ÜÏßµçÑ¹×ª»»Ê±¼ä£º1.1ms£»·ÖÁ÷µçÑ¹×ª»»Ê±¼ä£º1.1ms£»Ä£Ê½£ºµçÁ÷µçÑ¹Á¬ÐøÄ£Ê½
		INA226_Reg_Write(INA226_2_ADDR, Calibration_Reg_ADDR, 0x0800);  //ÉèÖÃÐ£×¼¼Ä´æÆ÷£¬500uA/bit£¬×î´ó¼ì²âµçÁ÷10A
	}
	else
	{
		IIC1_Stop();  //Èô¼ì²âÊ§°Ü£¬·¢ËÍÍ£Ö¹ÐÅºÅ
		printf("nofound ina226_2\n\n");
	}
}

//**********µç³ØÊä³öµçÑ¹£¬µçÁ÷£¬¹¦ÂÊ¼à¿Ø**********
/**
  * @brief      ÅÐ¶Ïina226_3ÊÇ·ñÕý³£¹¤×÷
  * @param      ÎÞ
  * @note       ÎÞ
  * @retval     1±íÊ¾Õý³££¬0±íÊ¾²»Õý³£
  */
void INA226_3_Init(void)
{
	IIC1_GPIO_Init();  //ÅäÖÃGPIO
	if(IIC_CheckDevice(INA226_3_ADDR) == 0)
	{
		printf("INA226_3 found¦\n\n");
		INA226_Reg_Write(INA226_3_ADDR, Config_Reg_ADDR, 0x8000);  //INA226¸´Î»£¬ËùÓÐ¼Ä´æÆ÷»Ö¸´³ÉÄ¬ÈÏÉèÖÃ
		INA226_Reg_Write(INA226_3_ADDR, Config_Reg_ADDR, 0x4007);  //ÉèÖÃ¹¤×÷Ä£Ê½£ºÆ½¾ùÑù±¾Êý£º1£»×ÜÏßµçÑ¹×ª»»Ê±¼ä£º1.1ms£»·ÖÁ÷µçÑ¹×ª»»Ê±¼ä£º1.1ms£»Ä£Ê½£ºµçÁ÷µçÑ¹Á¬ÐøÄ£Ê½
		INA226_Reg_Write(INA226_3_ADDR, Calibration_Reg_ADDR, 0x0800);  //ÉèÖÃÐ£×¼¼Ä´æÆ÷£¬Current¼Ä´æÆ÷500uA/bit£¬×î´ó¼ì²âµçÁ÷10A£¬×ÜÏßµçÑ¹¼Ä´æÆ÷1.25mV/Bit£¬Power¼Ä´æÆ÷0.0125mW/Bit
	}
	else
	{
		IIC1_Stop();  //Èô¼ì²âÊ§°Ü£¬·¢ËÍÍ£Ö¹ÐÅºÅ
		printf("nofound ina226\n\n");
	}
}




