 #include "STC12C32AD.h"
 #include "all.h"
 #include "INTRINS.H" 
 #include "delay.h"
 #include "ds18b20.h"
 
unsigned char   Read_18B20_Value;
unsigned char   Read_18B20_Value2; 

//2019.03.30
sbit P10 = 0x90^0;
sbit P37 = 0xB0^7;

//DS18B20����=================================================
void DS18B20_ReadByte(void)
{
    BYTE i;
    bit Wei;
    bit Wei2;
    Read_18B20_Value=0;
    Read_18B20_Value2=0;
    //EA=0;
    for (i=0; i<8; i++)             //8λ������
    {
        Read_18B20_Value >>= 1;
        Read_18B20_Value2 >>= 1;
        HeatingPlateSensor_Port = 0;            //��ʼʱ��Ƭ
        ChamberOutletSensor_Port = 0;            //��ʼʱ��Ƭ
        delay_us(2); 
        HeatingPlateSensor_Port = 1;             //׼������
        ChamberOutletSensor_Port =1;
        delay_us(2);              //������ʱ
        Wei=HeatingPlateSensor_Port;
        Wei2=ChamberOutletSensor_Port;
        if (Wei==1) Read_18B20_Value |= 0x80;        //��ȡ����
        if (Wei2==1) Read_18B20_Value2 |= 0x80;        //��ȡ����
        delay_us(90);                  //�ȴ�60usʱ��Ƭ����
    }
    //EA=1; 
}

void DS18B20_WriteByte(unsigned char dat)
{
    BYTE  i;
    //EA=0;
    for (i=0; i<8; i++)               //8λ������
    {
        HeatingPlateSensor_Port = 0;                        //��ʼʱ��Ƭ
        ChamberOutletSensor_Port=0;
        delay_us(2);                         //��ʱ�ȴ�
        if((dat & 0x01)!=0)
        {
           HeatingPlateSensor_Port = 1;
           ChamberOutletSensor_Port=1;
        }
        else
        {
       	   HeatingPlateSensor_Port=0;
       	   ChamberOutletSensor_Port=0;
       	}
        delay_us(110);                         //�ȴ�ʱ��Ƭ����
        HeatingPlateSensor_Port = 1;                     //�ָ�������
        ChamberOutletSensor_Port=1;
        delay_us(2);                         //��ʱ�ȴ�
        dat >>= 1;                           //�ͳ�����
    }
    //EA=1;
}

u8t DS18B20_CheckCrc(u8t Cdata[], u8t nbrOfBytes, u8t checksum)
//==============================================================================
{
  u8t crc = 0;	
  u8t byteCtr;
  u8t bit8;
	u8t crc_1byte = 0;
  //calculates 8-Bit checksum with given polynomial
	crc = 0;
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  { 
    crc ^= (Cdata[byteCtr]);
		crc_1byte ^= 0;
    for (bit8 = 0; bit8 < 8; bit8++)
    {
			if ((crc_1byte^crc) & 0x01)
			{
				crc_1byte ^= 0x18;
				crc_1byte >>= 1;
				crc_1byte |= 0x80;
			}
			else crc_1byte >>= 1;
			crc >>= 1;  
		}
		//crc = crc ^ crc_1byte;
	}
  if(crc_1byte != checksum) 
  {
  	return CHECKSUM_ERROR;
  }
  else return 0;
} 
