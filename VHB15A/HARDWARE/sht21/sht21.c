// #include "STC12C32AD.h"
 #include "all.h"
// #include "sht21.h"
// #include "delay.h"	
 //#include "lcd.h" 
 
#define	WriteCmd  0x80
#define	ReadCmd   0x81
#define TempCmd   0xE3
#define RHCmd     0xE5

 /***************************************************************************/

#define SDA_CONF     P11
#define SCL          P12
#define SDA          P11
#define ASK            0
#define NO_ASK         1
#define OUT_LOW        0
//#define INPUT_HIGH     1
//#define SCL_INPUT_HIGH()   P1M1 |= 0x04;P1M0 &= 0xFB 				//10 ��Ϊ����
//#define SDA_INPUT_HIGH()   P1M1 |= 0x02;P1M0 &= 0xFD 				//10 ��Ϊ����
//#define SCL_OUTPUT_LOW()   P1M1 |= 0x04;P1M0 |= 0x04;SCL=OUT_LOW    //11 ��© Set port as output for configuration
//#define SDA_OUTPUT_LOW()   P1M1 |= 0x02;P1M0 |= 0x02;SDA=OUT_LOW    //11 ��© Set port as output for configuration
//=============================================================




static void SHT21_Start(void);
static void SHT21_Stop(void);
static u8t SHT2x_CheckCrc(const u8t Cdata[], u8t nbrOfBytes, u8t checksum);
static uint8_t SHT21_Write8Bit(uint8_t i_input);
static uint8_t SHT21_Read8Bit(uint8_t ask);

//extern uint8_t  Test_Mode_Dis_Data_Flag; //����ģʽ��ʾ����DATA�ı�־	�ⲿ����


//2019.03.29
sfr P1M0 = 0x92; // 
sfr P1M1 = 0x91;
sbit P11 = (u8)0x90^(u8)1;
sbit P12 = (u8)0x90^(u8)2;


static void SCL_INPUT_HIGH(void)
{
	P1M1 |= (u8)0x04;
	P1M0 &= (u8)0xFB;				//10 ��Ϊ����
}

static void SDA_INPUT_HIGH(void)
{
	P1M1 |= (u8)0x02;
	P1M0 &= (u8)0xFD; 
}	
static void SCL_OUTPUT_LOW(void)
{
	P1M1 |= (u8)0x04;
	P1M0 |= (u8)0x04;
	SCL=(bit)OUT_LOW;
}
static void SDA_OUTPUT_LOW(void)
{
	P1M1 |= (u8)0x02;
	P1M0 |= (u8)0x02;
	SDA=(bit)OUT_LOW;
}

static void SHT21_Start(void)
{
		SCL_INPUT_HIGH();	
		SDA_INPUT_HIGH();
		delay_us(10);		
		SDA_OUTPUT_LOW();
		delay_us(10); //
		SCL_OUTPUT_LOW();		
}


static void SHT21_Stop(void)
 {
		SCL_OUTPUT_LOW();
	  SDA_OUTPUT_LOW();		
		SCL_INPUT_HIGH();
		delay_us(10); //
		SDA_INPUT_HIGH();
		delay_us(10); //
}


/***************************************************************************/
/***************************************************************************/
static uint8_t SHT21_Write8Bit(uint8_t i_input)
{
		uint16_t  i;
		uint8_t ret=0;

		for(i=(uint16_t)0x80; i>(u16)0; i>>=1)           //shift bit for masking (8 times)
		{
			SCL_OUTPUT_LOW();
			if((i &(uint8_t)i_input)!=(uint8_t)0)
			{	
				SDA_INPUT_HIGH();
			}			
			else
			{	
				SDA_OUTPUT_LOW(); //masking txByte, write bit to SDA-Line
			}              

			delay_us(10);                        //data set-up time (t_SU;DAT)
			SCL_INPUT_HIGH();                                //generate clock pulse on SCL
			delay_us(10);                        //SCL input_high time (t_HIGH)
		}
		SCL_OUTPUT_LOW();
		delay_us(10); 
		SDA_INPUT_HIGH();                       //release SDA-line
	
		SCL_INPUT_HIGH();                       //clk #9 for ack
		delay_us(10);                         //data set-up time (t_SU;DAT)
		
		i = 0;
		while(SDA_CONF)
		{
			if(++i>(u16)2400){SCL_OUTPUT_LOW();ret=(uint8_t)1;break;}
		}
//		SCL_OUTPUT_LOW();
//		return (uint8_t)0;
		if(ret==(uint8_t)0)
		{
			SCL_OUTPUT_LOW();
		}
		
		return ret;
}

/***************************************************************************/
static uint8_t SHT21_Read8Bit(uint8_t ask)
{
		uint8_t  mask,rxByte=0; 		

		SDA_INPUT_HIGH(); //release SDA-line

		for (mask=(u8)0x80; mask>(u8)0; mask>>=1) //shift bit for masking (8 times)
		{
			SCL_INPUT_HIGH();                   //sht21_start clock on SCL-line
		  delay_us(10);                     //SCL input_high time (t_HIGH)
		  if (SDA_CONF==1) 
			{
				rxByte=(rxByte | mask); //read bit
			}
		  SCL_OUTPUT_LOW();
		  delay_us(10);                     //data hold time(t_HD;DAT)
		}
		if(ask!=(uint8_t)0)
		{
		  SDA_INPUT_HIGH();
		}
		else
		{
		  SDA_OUTPUT_LOW();
		}                      //send acknowledge if necessary
		delay_us(10);                       //data set-up time (t_SU;DAT)
		SCL_INPUT_HIGH();                     //clk #9 for ack
		delay_us(10);                       //SCL input_high time (t_HIGH)
		SCL_OUTPUT_LOW();
		delay_us(10);
		SDA_INPUT_HIGH();                      //release SDA-line
		SDA_OUTPUT_LOW();
		delay_us(10);		
		return  rxByte;
}



/***************************************************************************/
uint8_t SHT21_WriteTemp(void)
{
	uint8_t  Read_SHT21_Err;
	SHT21_Start();
	Read_SHT21_Err=SHT21_Write8Bit(WriteCmd);
	Read_SHT21_Err=SHT21_Write8Bit(TempCmd);
	SHT21_Start();
	Read_SHT21_Err=SHT21_Write8Bit(ReadCmd);
	SCL_INPUT_HIGH();    // set SCL I/O port as input	

	return Read_SHT21_Err;
}

uint8_t SHT21_WriteRH(void)
{
	uint8_t  Read_SHT21_Err;
	SHT21_Start();
	Read_SHT21_Err=SHT21_Write8Bit(WriteCmd);
	Read_SHT21_Err=SHT21_Write8Bit(RHCmd);
	SHT21_Start();
	Read_SHT21_Err=SHT21_Write8Bit(ReadCmd);
  SCL_INPUT_HIGH(); // set SCL I/O port as input
	

  	return Read_SHT21_Err;
}



//// �򿪻�ر�SHT21�ڲ����ȹ���
//void	SHT21_Heater_On_Off(uint8_t Heater_State)
//{
//	uint8_t  Read_SHT21_Err;
//	uint8_t UserReg;
//	SHT21_Start();
//	Read_SHT21_Err = SHT21_Write8Bit(WriteCmd);	
//	Read_SHT21_Err = SHT21_Write8Bit(0xE7);	 *���û��Ĵ���ָ��
//	SHT21_Start();
//	Read_SHT21_Err = SHT21_Write8Bit(0x81);	
//	UserReg =  SHT21_Read8Bit(NO_ASK);//��ȡ�û��Ĵ���
//	UserReg &= 0x38;//����3-4-5Ԥ��λ
//	UserReg |= 0x02;//��������OTP���أ�RH 12bit T 14bit
//	if(Heater_State)
//	{
//		UserReg |= 0x04;//����Ƭ�ϼ�����	
//	} 

//	SHT21_Start();
//	Read_SHT21_Err = SHT21_Write8Bit(WriteCmd);
//	Read_SHT21_Err = SHT21_Write8Bit(0xE6);	 //д�û��Ĵ���ָ��
//	Read_SHT21_Err = SHT21_Write8Bit(UserReg);	 //д�û��Ĵ���ָ��
//	SHT21_Stop();
//}



/***************************************************************************/
//����SHT21_ERROR --- δ�յ���ȷ������
//�����¶�����
//����ʪ������
uint16_t SHT21_ReadData(void)
{
	uint8_t TempH,TempL;
	UINT32  TempLong; 
	u8t  checksum;   //checksum
    u8t  Cdata[2];    //data array for checksum verification
	u8t  CRC8_error;   //checksum
	uint8_t tmp=0;
	uint16_t ret=0;

	TempH=SHT21_Read8Bit(ASK);
	//delay_us(50);
  	TempL=SHT21_Read8Bit(ASK);
	//delay_us(50);
	checksum=SHT21_Read8Bit(NO_ASK); //����CRC8����
	//checksum+=1;	 //����CRC
	SHT21_Stop();

  Cdata[0] =  TempH;
	Cdata[1] =  TempL;
	CRC8_error = SHT2x_CheckCrc(Cdata,2,checksum);
	if(CRC8_error == (u8)CHECKSUM_ERROR)//
	{
//		return SHT21_ERROR;//CRCУ��ʧ��
		ret=(uint16_t)SHT21_ERROR;
	}
	else
	{
		TempLong=TempH;
	//	TempLong=(TempLong<<8)+(UINT32)(TempL & (uint8_t)0xFC);
		TempLong<<=(UINT32)8;
		tmp=(TempL & (uint8_t)0xFC);
		TempLong+=(UINT32)tmp;

	//	if(Work_State == UI_STATE_SERVICE_MODE)
		{

		////	 LCD_ShowxNum(150,220,24,3,TempH,0x80,BLACK18); //8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ
		////	 LCD_ShowxNum(150,268,24,3,TempL,0x80,BLACK18); //8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ
		}
		
		//if(TempL& 0x01==0x01) return SHT21_ERROR;
		if(TempLong==(UINT32)0x0)
		{
//			return SHT21_ERROR;//����Ϊ0
			ret=(uint16_t)SHT21_ERROR;
		}
		else
		{
			if((TempL&(uint8_t)0x02)==(u8)0x0)  //�¶�
			{
				TempLong=(((UINT32)1757*TempLong)>>16)-(UINT32)468; 
				//if(TempLong > 1250)	return SHT21_ERROR;	//�¶Ȳ��ܳ���125
				//else 
				//				return  TempLong;
				ret=(uint16_t)TempLong;

			}
			else	//ʪ��
			{
				TempLong=(((UINT32)1250*TempLong)>>16)-(UINT32)60;
				//if(TempLong > 1000)	return SHT21_ERROR;	 //ʪ�Ȳ��ܳ���100
				//else//20141228���ִ���
				
				TempLong |= (UINT32)0x8000;//�����¶Ⱥ�ʪ������,�¶����λΪ1
//				return  TempLong;
				ret=(uint16_t)TempLong;				
			}
		}
	}
	return ret;
} 

static u8t SHT2x_CheckCrc(const u8t Cdata[], u8t nbrOfBytes, u8t checksum)
//==============================================================================
{
  u8t crc = 0;	
  u8t byteCtr;
  u8t bit8;
	uint16_t tmp=0;
	uint8_t res=0;
  //calculates 8-Bit checksum with given polynomial
	for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
	{ 
		crc ^= (Cdata[byteCtr]);
		for (bit8 = 8; bit8 > (u8)0; --bit8)
		{
	//	  if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
	//			else crc = (crc << 1);
			if ((crc & (u8t)0x80)!=(u8t)0)
			{
	//				crc = (u8t)((uint16_t)(crc << 1) ^ (uint16_t)POLYNOMIAL);
				crc<<=(uint16_t)1;
				tmp=(crc ^ (uint16_t)POLYNOMIAL);
				crc = (u8t)tmp;
			}				
			else
			{
				crc = (uint8_t)((crc << 1));
			}				
		}
	}
	if(crc != checksum) 
	{
		res=(uint8_t)CHECKSUM_ERROR;
	}
	else
	{
		res=(uint8_t)0;
	}	
	return res;
} 
