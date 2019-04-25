 #include "STC12C32AD.h"
 #include "all.h" 
 //#include "delay.h"
 //#include "RX8010.h"

 
#define SCL_          P25
#define SDA_        	P26
//#define ASK            0
//#define NO_ASK         1
//#define OUT_LOW        0
//#define INPUT_HIGH     1

//#define SCL_DIR_OUTPUT()   P2M1 |= 0x10;P2M0 |= 0x10  //11 开漏输出
//#define SDA_DIR_INPUT()    P2M1 |= 0x40;P2M0 &= 0xBF  //10 仅为输入
//#define SDA_DIR_OUTPUT()   P2M1 |= 0x40;P2M0 |= 0x40  //11 开漏输出

#define	RX8010_WRITE  0x64 //I2C器件地址
#define RX8010_READ		0x65

////2019.03.29
//sfr WDT_CONTR = 0xC1;
//sbit EA  = (u8)0xA8^(u8)7;
//sbit P25 = (u8)0xA0^(u8)5;
//sbit P26 = (u8)0xA0^(u8)6;
//sfr P2M0 = 0x96;
//sfr P2M1 = 0x95;


static void SCL_DIR_OUTPUT(void)
{
	P2M1 |= (u8)0x10;
	P2M0 |= (u8)0x10;
} 

static void SDA_DIR_INPUT(void)
{
	P2M1 |= (u8)0x40;
	P2M0 &= (u8)0xBF;
}
static void SDA_DIR_OUTPUT(void)
{
	P2M1 |= (u8)0x40;
	P2M0 |= (u8)0x40;
}

//************************************
//** 函数原型: void IC_start(void); **
//** 功 能: IC 总线起始位. **
//************************************
static void IC_start(void)
{	
		SCL_DIR_OUTPUT();
		SDA_DIR_OUTPUT();	  
		SCL_=(bit)1;
		SDA_=(bit)1;
		delay_us(2);
		SDA_=(bit)0;
		delay_us(2);//
		SCL_=(bit)0;
}

//**********************************
//** 函数原型: void IC_stop(void); **
//** 功 能: IC 总线停止位. **
//**********************************
static void IC_stop(void)
{
		SCL_=(bit)0;
	  SDA_DIR_OUTPUT();
		SDA_=(bit)0;
		delay_us(2);
		SCL_=(bit)1;
		delay_us(2); //
		SDA_=(bit)1;
		delay_us(2);
}

//***************************************************
//** 函数原型: bit IC_writebyte(uchar wdata); **
//** 功 能: 向IC 总线发送8 位数据,并请求一个应答信 **
//** 号ACK.如果收到ACK 应答则返回1(TRUE), **
//** 否则返回0(FALSE). **
//***************************************************
static uint8_t IC_WriteByte( uint8_t wdata)
{
	uint8_t i;
	uint8_t ret=1;
	SDA_DIR_OUTPUT();
	for(i=0;i<(u8)8;i++)
	{
		SCL_=(bit)0;
		delay_us(2);
		if((wdata&(uint8_t)0x80)!=(uint8_t)0)
		{
			SDA_=(bit)1;
		}			
		else 
		{
			SDA_=(bit)0;
		}
		delay_us(2);
		SCL_=(bit)1;
		delay_us(4);
		wdata<<=1;
	}
	SCL_=(bit)0;
	delay_us(4);
	SDA_DIR_INPUT();
	SCL_=(bit)1;
	delay_us(4);
	i=0;
	while(SDA_)
	{
		if(++i>(u8)12){SCL_=(bit)0;ret=(uint8_t)0;break;}
	}
	if(ret==(uint8_t)1)
	{
		SCL_=(bit)0;
	}
	return ret;
}
//***********************************************
//** 函数原型: uchar IC_readbyte(void); **
//** 功 能: 从IC 总线上接收8 位数据,并将接受到 **
//** 8 位数据作为一个字节返回,不回送应 **
//** 答信号ACK. **
//***********************************************
static uint8_t IC_ReadByte(void)
{ 
	uint8_t i;
	uint8_t IC_data=0;
	SCL_=(bit)0;
	SDA_DIR_OUTPUT();
	SDA_=(bit)1;
	SDA_DIR_INPUT();	
	for(i=0;i<(uint8_t)8;i++)
	{
		SCL_=(bit)1;
		delay_us(2);
		IC_data<<=1;

		//IC_data|=SDA_;
		if(SDA_)
		{
			IC_data++;
		}
		//delay_us(4);
		SCL_=(bit)1;
		delay_us(2);
		SCL_=(bit)0;
		delay_us(4);
	}
	delay_us(4);
	SCL_=(bit)0;
	SDA_DIR_OUTPUT();
	SDA_=(bit)0;
	return(IC_data);
}
//***************************************************************
//** 函数原型: bit readEEone(uchar instr,uchar addr,uchar num); **
//** 功 能: 从8010 中读取num 个字节的数据,采用序列读操作方 **
//** 式从片内Address 地址开始连续读取数据.8010 不接 **
//** 受指定的地址则返回0(FALSE). **
//***************************************************************
uint8_t readEEone(uint8_t addr,uint8_t num,uint8_t *Data)
{
//	uint8_t i;
//	IC_start();
//	if(IC_WriteByte(RX8010_WRITE)==0)
//	{
//		IC_stop(); return(0);
//	}
//	
//	if(IC_WriteByte(addr)==0)
//	{
//		IC_stop(); return(0);
//	}
//	IC_start();
//	if(IC_WriteByte(RX8010_READ)==0)
//	{
//		IC_stop(); return(0);
//	}
//	for(i=0;i<(num-1);i++)
//	{
//		Data[i]=IC_ReadByte();
//		SDA_DIR_OUTPUT();				
//		SDA_=0; 	
//		SCL_=1;
//	}
//	Data[num-1]=IC_ReadByte();
//	SDA_DIR_OUTPUT();	
//	SDA_=1; 
//	delay_us(4);
//	SCL_=1;
//	IC_stop();
//	return(1);

	uint8_t i;
	uint8_t ret=1;
	IC_start();
	if(IC_WriteByte(RX8010_WRITE)==(u8)0)
	{
		IC_stop(); ret=(uint8_t)0;
	}
	else if(IC_WriteByte(addr)==(u8)0)
	{
		IC_stop(); ret=(uint8_t)0;
	}
	else
	{
		IC_start();
		if(IC_WriteByte(RX8010_READ)==(u8)0)
		{
			
			IC_stop(); ret=(uint8_t)0;
		}
		else
		{
			for(i=0;i<(num-(u8)1);i++)
			{
				Data[i]=IC_ReadByte();
				SDA_DIR_OUTPUT();				
				SDA_=(bit)0; /** Send ACK **/		
				SCL_=(bit)1;
			}
			Data[num-(u8)1]=IC_ReadByte();
			SDA_DIR_OUTPUT();	
			SDA_=(bit)1; /** Send Read End **/
			delay_us(4);
			SCL_=(bit)1;
			IC_stop();
		}
	}
	return ret;
}

//****************************************************************
//** 函数原型: bit writeEEone(uchar instr,uchar addr,uchar num); **
//** 功 能: 将EEbuf[]单元中的数据写入8010 的num 个字节. **
//** 采用页写操作方式,每次写入时都需要指定片内地址. **
//** 如果8010 不接受指定的地址或某个传送的字节未收到 **
//** 应答信号ACK,则返回0(FALSE). **
//****************************************************************
static void writeEEone(uint8_t addr,uint8_t num,const uint8_t *Data)
{
	uint8_t i;
	//gie=0;
	IC_start();
	if(IC_WriteByte(RX8010_WRITE)==(u8)0)
	{
//		IC_stop(); return(0);
		IC_stop();
	}
	if(IC_WriteByte(addr)==(u8)0)
	{
//		IC_stop(); return(0);
		IC_stop();
	}
	for(i=0;i<num;i++)
	{
		if(IC_WriteByte(Data[i])==(u8)0)
		{
//			IC_stop(); return(0);
			IC_stop();
		}
	}
	IC_stop();
	delay_us(4);
	SDA_=(bit)0;
	SCL_=(bit)0;
	//gie=1;
//	return(1);
} 


//检测VLF频率停止位，若为1则初始化RX8010
uint8_t RX8010_Initialize(void) 
{
	uint8_t Rdata[7];
	uint8_t i;
	uint8_t Init_State;
	
	EA = (bit)0;	
	Init_State = RX8010_INIT_NONE;//未初始化
	
	Rdata[0] = 0x00;
	for(i = 0;i < (u8)10;i++)
	{
		if(readEEone(0x1E,1,Rdata) == (u8)1)
		{
			Init_State = RX8010_INIT_NONE;
			break;//读VLF状态
		}
		delay_ms(100);
		WDT_CONTR = 0x3F; 
		Init_State = RX8010_INIT_FAIL;//初始化失败
	}
	
	if(Init_State == (u8)RX8010_INIT_NONE)
	{		
		if(Bit_is_one(Rdata[0],1)!=(uint8_t)0)//判定是否停止,VLF=1
		{
			Init_State = RX8010_INIT_OK;//初始化成功
			Rdata[0] = 0x58;
			writeEEone(0x17,1,Rdata);
			Rdata[0] = 0x00;
			Rdata[1] = 0x18;
			Rdata[2] = 0x00;
			writeEEone(0x30,3,Rdata);	
			Rdata[0] = 0x04;//无中断输出
			Rdata[1] = 0x00;//清AF和VLF
			Rdata[2] = 0x40;		
			writeEEone(0x1D,3,Rdata);	
			
			Rdata[0] = 0x00;//SEC
			Rdata[1] = 0x00;//MIN
			Rdata[2] = 0x00;//HOUR
			Rdata[3] = 0x10;//WEEK
			Rdata[4] = 0x01;//DAY
			Rdata[5] = 0x01;//MONTH
			Rdata[6] = 0x15;//YEAR 2015-01-01-00:00:00-Tursday
			writeEEone(0x10,7,Rdata);//设定时间
			
			Rdata[0] = 0x00;//MIN
			Rdata[1] = 0x00;//HOUR
			Rdata[2] = 0x00;//WEEK
			writeEEone(0x18,3,Rdata);//设定闹钟
			
			Rdata[0] = 0x00;//MIN
			Rdata[1] = 0x00;//HOUR
			writeEEone(0x1B,2,Rdata);//设定定时
			
			Rdata[0] = 0x00;//STOP位为0
			writeEEone(0x1F,1,Rdata);			
		}	
	}
	EA = (bit)1;
	return Init_State;
}
 
//设定时钟
//注意，原来是DS1302，改为RX8010后，顺序不同，另外，星期由8421BCD码改为位
void RX8010_SetTime(const BYTE *p) 
{
	uint8_t Rdata[7];
	
	Rdata[0] = 0x40;//STOP位为1
	writeEEone(0x1F,1,Rdata);
	
	Rdata[0] = *p;//SEC
//	Rdata[1] = *(p+1);//MIN
//	Rdata[2] = *(p+2);//HOUR	
//	Rdata[3] = *(p+5);//WEEK
	Rdata[1] = p[1];//MIN
	Rdata[2] = p[2];//HOUR	
	Rdata[3] = p[5];//WEEK

//	data[3] = 1 << *(p+5); //WEEK由DS1302转换为RX8010 
//	if(data[3]==0x80)data[3]=1;//DS1302星期天为7，而RX8010是低位为1

//	Rdata[4] = *(p+3);//DAY
//	Rdata[5] = *(p+4);//MONTH
//	Rdata[6] = *(p+6);//YEAR 2015-01-01-00:00:00-Tursday
	Rdata[4] = p[3];//DAY
	Rdata[5] = p[4];//MONTH
	Rdata[6] = p[6];//YEAR 2015-01-01-00:00:00-Tursday
	writeEEone(0x10,7,Rdata);
	
	Rdata[0] = 0x00;//STOP位为0
	writeEEone(0x1F,1,Rdata);	
}
	
//读取时钟
//注意，原来是DS1302，改为RX8010后，顺序不同，另外，星期由8421BCD码改为位
void RX8010_GetTime(BYTE *p) 
{
	uint8_t Rdata[7];
	
//	readEEone(0x10,7,Rdata);
	if(readEEone(0x10,7,Rdata)==(uint8_t)1)
	{
		*p = Rdata[0];//SEC	
		//	*(p+1) = Rdata[1];//MIN
		//	*(p+2) = Rdata[2];//HOUR
		//	*(p+5) = Rdata[3];//WEEK
		p[1]= Rdata[1];//MIN
		p[2] = Rdata[2];//HOUR
		p[5] = Rdata[3];//WEEK
		//	// 注意：未使用星期
		//	for(i = 1;i > 7; i++)
		//	{
		//		if((*(p+5) >> i)==0)break;
		//	}
		//	*(p+5) = i;
		//	if(*(p+5) == 1)//星期天
		//	{
		//		*(p+5) = 7;
		//	}
		//	else
		//	{
		//		*(p+5) -=1;
		//	}
		//	

		//	*(p+3) = Rdata[4];//DAY
		//	*(p+4) = Rdata[5];//MONTH
		//	*(p+6) = Rdata[6];//YEAR 2015-01-01-00:00:00-Tursday
		p[3] = Rdata[4];//DAY
		p[4] = Rdata[5];//MONTH
		p[6] = Rdata[6];//YEAR 2015-01-01-00:00:00-Tursday
	}
}
