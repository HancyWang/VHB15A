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
//#define SCL_INPUT_HIGH()   P1M1 |= 0x04;P1M0 &= 0xFB 				//10 仅为输入
//#define SDA_INPUT_HIGH()   P1M1 |= 0x02;P1M0 &= 0xFD 				//10 仅为输入
//#define SCL_OUTPUT_LOW()   P1M1 |= 0x04;P1M0 |= 0x04;SCL=OUT_LOW    //11 开漏 Set port as output for configuration
//#define SDA_OUTPUT_LOW()   P1M1 |= 0x02;P1M0 |= 0x02;SDA=OUT_LOW    //11 开漏 Set port as output for configuration
//=============================================================




static void SHT21_Start(void);
static void SHT21_Stop(void);
static u8t SHT2x_CheckCrc(const u8t Cdata[], u8t nbrOfBytes, u8t checksum);
static uint8_t SHT21_Write8Bit(uint8_t i_input);
static uint8_t SHT21_Read8Bit(uint8_t ask);

//extern uint8_t  Test_Mode_Dis_Data_Flag; //测试模式显示数据DATA的标志	外部变量


//2019.03.29
sfr P1M0 = 0x92; // 
sfr P1M1 = 0x91;
sbit P11 = (u8)0x90^(u8)1;
sbit P12 = (u8)0x90^(u8)2;


static void SCL_INPUT_HIGH(void)
{
	P1M1 |= (u8)0x04;
	P1M0 &= (u8)0xFB;				//10 仅为输入
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



//// 打开或关闭SHT21内部加热功能
//void	SHT21_Heater_On_Off(uint8_t Heater_State)
//{
//	uint8_t  Read_SHT21_Err;
//	uint8_t UserReg;
//	SHT21_Start();
//	Read_SHT21_Err = SHT21_Write8Bit(WriteCmd);	
//	Read_SHT21_Err = SHT21_Write8Bit(0xE7);	 *读用户寄存器指令
//	SHT21_Start();
//	Read_SHT21_Err = SHT21_Write8Bit(0x81);	
//	UserReg =  SHT21_Read8Bit(NO_ASK);//读取用户寄存器
//	UserReg &= 0x38;//保留3-4-5预留位
//	UserReg |= 0x02;//不能启动OTP加载，RH 12bit T 14bit
//	if(Heater_State)
//	{
//		UserReg |= 0x04;//启动片上加热器	
//	} 

//	SHT21_Start();
//	Read_SHT21_Err = SHT21_Write8Bit(WriteCmd);
//	Read_SHT21_Err = SHT21_Write8Bit(0xE6);	 //写用户寄存器指令
//	Read_SHT21_Err = SHT21_Write8Bit(UserReg);	 //写用户寄存器指令
//	SHT21_Stop();
//}



/***************************************************************************/
//返回SHT21_ERROR --- 未收到正确的数据
//返回温度数据
//返回湿度数据
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
	checksum=SHT21_Read8Bit(NO_ASK); //读到CRC8数据
	//checksum+=1;	 //测试CRC
	SHT21_Stop();

  Cdata[0] =  TempH;
	Cdata[1] =  TempL;
	CRC8_error = SHT2x_CheckCrc(Cdata,2,checksum);
	if(CRC8_error == (u8)CHECKSUM_ERROR)//
	{
//		return SHT21_ERROR;//CRC校验失败
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

		////	 LCD_ShowxNum(150,220,24,3,TempH,0x80,BLACK18); //8表示高位为0也显示,0表示非叠加显示
		////	 LCD_ShowxNum(150,268,24,3,TempL,0x80,BLACK18); //8表示高位为0也显示,0表示非叠加显示
		}
		
		//if(TempL& 0x01==0x01) return SHT21_ERROR;
		if(TempLong==(UINT32)0x0)
		{
//			return SHT21_ERROR;//数据为0
			ret=(uint16_t)SHT21_ERROR;
		}
		else
		{
			if((TempL&(uint8_t)0x02)==(u8)0x0)  //温度
			{
				TempLong=(((UINT32)1757*TempLong)>>16)-(UINT32)468; 
				//if(TempLong > 1250)	return SHT21_ERROR;	//温度不能超过125
				//else 
				//				return  TempLong;
				ret=(uint16_t)TempLong;

			}
			else	//湿度
			{
				TempLong=(((UINT32)1250*TempLong)>>16)-(UINT32)60;
				//if(TempLong > 1000)	return SHT21_ERROR;	 //湿度不能超过100
				//else//20141228发现错误
				
				TempLong |= (UINT32)0x8000;//区别温度和湿度数据,温度最高位为1
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
