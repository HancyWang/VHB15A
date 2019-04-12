// #include "STC12C32AD.h"
 #include "all.h"
// #include "INTRINS.H" 
// #include "delay.h"
// #include "ds18b20.h"
 
uint8_t   Read_18B20_Value;
uint8_t   Read_18B20_Value2; 

//2019.03.30
//sbit P10 = (u8)0x90^(u8)0;
sbit P10 = (u8)0x90;
sbit P37 = (u8)0xB0^(u8)7;

//DS18B20程序=================================================
void DS18B20_ReadByte(void)
{
    BYTE i;
    u8 Wei;
    u8 Wei2;
    Read_18B20_Value=0;
    Read_18B20_Value2=0;
    //EA=0;
    for (i=0; i<(u8)8; i++)             //8位计数器
    {
        Read_18B20_Value >>= 1;
        Read_18B20_Value2 >>= 1;
        HeatingPlateSensor_Port = (bit)0;            //开始时间片
        ChamberOutletSensor_Port = (bit)0;            //开始时间片
        delay_us(2); 
        HeatingPlateSensor_Port = (bit)1;             //准备接收
        ChamberOutletSensor_Port =(bit)1;
        delay_us(2);              //接收延时
        Wei=HeatingPlateSensor_Port;
        Wei2=ChamberOutletSensor_Port;
        if (Wei==(u8)1)
				{
					Read_18B20_Value |= (uint8_t)0x80;        //读取数据
				}					
        if (Wei2==(u8)1) 
				{
					Read_18B20_Value2 |= (uint8_t)0x80;        //读取数据
				}
        delay_us(90);                  //等待60us时间片结束
    }
    //EA=1; 
}

void DS18B20_WriteByte(uint8_t dat)
{
    BYTE  i;
    //EA=0;
    for (i=0; i<(u8)8; i++)               //8位计数器
    {
        HeatingPlateSensor_Port = (bit)0;                        //开始时间片
        ChamberOutletSensor_Port=(bit)0;
        delay_us(2);                         //延时等待
        if((dat & (uint8_t)0x01)!=(u8)0)
        {
           HeatingPlateSensor_Port = (bit)1;
           ChamberOutletSensor_Port=(bit)1;
        }
        else
        {
       	   HeatingPlateSensor_Port=(bit)0;
       	   ChamberOutletSensor_Port=(bit)0;
       	}
        delay_us(110);                         //等待时间片结束
        HeatingPlateSensor_Port = (bit)1;                     //恢复数据线
        ChamberOutletSensor_Port=(bit)1;
        delay_us(2);                         //延时等待
        dat >>= 1;                           //送出数据
    }
    //EA=1;
}

u8t DS18B20_CheckCrc(const u8t Cdata[], u8t nbrOfBytes, u8t checksum)
//==============================================================================
{
	u8t crc = 0;	
	u8t byteCtr;
	u8t bit8;
	u8t crc_1byte = 0;
	u8t ret=0;
	//calculates 8-Bit checksum with given polynomial
//	crc = 0;
	for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
	{ 
		crc ^= (Cdata[byteCtr]);
//		crc_1byte ^= (u8t)0;
		for (bit8 = 0; bit8 < (u8)8; bit8++)
		{
			if (((crc_1byte^crc) & (u8t)0x01)!=(u8t)0)
			{
				crc_1byte ^= (u8t)0x18;
				crc_1byte >>= 1;
				crc_1byte |= (u8t)0x80;
			}
			else
			{
				crc_1byte >>= 1;
			}				
			crc >>= 1;  
		}
		//crc = crc ^ crc_1byte;
	}
	if(crc_1byte != checksum) 
	{
		ret= CHECKSUM_ERROR;
	}
	else 
	{
		ret=(u8t)0;
	}
	return ret;
} 
