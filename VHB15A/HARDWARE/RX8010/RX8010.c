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

//#define SCL_DIR_OUTPUT()   P2M1 |= 0x10;P2M0 |= 0x10  //11 ��©���
//#define SDA_DIR_INPUT()    P2M1 |= 0x40;P2M0 &= 0xBF  //10 ��Ϊ����
//#define SDA_DIR_OUTPUT()   P2M1 |= 0x40;P2M0 |= 0x40  //11 ��©���

#define	RX8010_WRITE  0x64 //I2C������ַ
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
//** ����ԭ��: void IC_start(void); **
//** �� ��: IC ������ʼλ. **
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
//** ����ԭ��: void IC_stop(void); **
//** �� ��: IC ����ֹͣλ. **
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
//** ����ԭ��: bit IC_writebyte(uchar wdata); **
//** �� ��: ��IC ���߷���8 λ����,������һ��Ӧ���� **
//** ��ACK.����յ�ACK Ӧ���򷵻�1(TRUE), **
//** ���򷵻�0(FALSE). **
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
//** ����ԭ��: uchar IC_readbyte(void); **
//** �� ��: ��IC �����Ͻ���8 λ����,�������ܵ� **
//** 8 λ������Ϊһ���ֽڷ���,������Ӧ **
//** ���ź�ACK. **
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
//** ����ԭ��: bit readEEone(uchar instr,uchar addr,uchar num); **
//** �� ��: ��8010 �ж�ȡnum ���ֽڵ�����,�������ж������� **
//** ʽ��Ƭ��Address ��ַ��ʼ������ȡ����.8010 ���� **
//** ��ָ���ĵ�ַ�򷵻�0(FALSE). **
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
//** ����ԭ��: bit writeEEone(uchar instr,uchar addr,uchar num); **
//** �� ��: ��EEbuf[]��Ԫ�е�����д��8010 ��num ���ֽ�. **
//** ����ҳд������ʽ,ÿ��д��ʱ����Ҫָ��Ƭ�ڵ�ַ. **
//** ���8010 ������ָ���ĵ�ַ��ĳ�����͵��ֽ�δ�յ� **
//** Ӧ���ź�ACK,�򷵻�0(FALSE). **
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


//���VLFƵ��ֹͣλ����Ϊ1���ʼ��RX8010
uint8_t RX8010_Initialize(void) 
{
	uint8_t Rdata[7];
	uint8_t i;
	uint8_t Init_State;
	
	EA = (bit)0;	
	Init_State = RX8010_INIT_NONE;//δ��ʼ��
	
	Rdata[0] = 0x00;
	for(i = 0;i < (u8)10;i++)
	{
		if(readEEone(0x1E,1,Rdata) == (u8)1)
		{
			Init_State = RX8010_INIT_NONE;
			break;//��VLF״̬
		}
		delay_ms(100);
		WDT_CONTR = 0x3F; 
		Init_State = RX8010_INIT_FAIL;//��ʼ��ʧ��
	}
	
	if(Init_State == (u8)RX8010_INIT_NONE)
	{		
		if(Bit_is_one(Rdata[0],1)!=(uint8_t)0)//�ж��Ƿ�ֹͣ,VLF=1
		{
			Init_State = RX8010_INIT_OK;//��ʼ���ɹ�
			Rdata[0] = 0x58;
			writeEEone(0x17,1,Rdata);
			Rdata[0] = 0x00;
			Rdata[1] = 0x18;
			Rdata[2] = 0x00;
			writeEEone(0x30,3,Rdata);	
			Rdata[0] = 0x04;//���ж����
			Rdata[1] = 0x00;//��AF��VLF
			Rdata[2] = 0x40;		
			writeEEone(0x1D,3,Rdata);	
			
			Rdata[0] = 0x00;//SEC
			Rdata[1] = 0x00;//MIN
			Rdata[2] = 0x00;//HOUR
			Rdata[3] = 0x10;//WEEK
			Rdata[4] = 0x01;//DAY
			Rdata[5] = 0x01;//MONTH
			Rdata[6] = 0x15;//YEAR 2015-01-01-00:00:00-Tursday
			writeEEone(0x10,7,Rdata);//�趨ʱ��
			
			Rdata[0] = 0x00;//MIN
			Rdata[1] = 0x00;//HOUR
			Rdata[2] = 0x00;//WEEK
			writeEEone(0x18,3,Rdata);//�趨����
			
			Rdata[0] = 0x00;//MIN
			Rdata[1] = 0x00;//HOUR
			writeEEone(0x1B,2,Rdata);//�趨��ʱ
			
			Rdata[0] = 0x00;//STOPλΪ0
			writeEEone(0x1F,1,Rdata);			
		}	
	}
	EA = (bit)1;
	return Init_State;
}
 
//�趨ʱ��
//ע�⣬ԭ����DS1302����ΪRX8010��˳��ͬ�����⣬������8421BCD���Ϊλ
void RX8010_SetTime(const BYTE *p) 
{
	uint8_t Rdata[7];
	
	Rdata[0] = 0x40;//STOPλΪ1
	writeEEone(0x1F,1,Rdata);
	
	Rdata[0] = *p;//SEC
//	Rdata[1] = *(p+1);//MIN
//	Rdata[2] = *(p+2);//HOUR	
//	Rdata[3] = *(p+5);//WEEK
	Rdata[1] = p[1];//MIN
	Rdata[2] = p[2];//HOUR	
	Rdata[3] = p[5];//WEEK

//	data[3] = 1 << *(p+5); //WEEK��DS1302ת��ΪRX8010 
//	if(data[3]==0x80)data[3]=1;//DS1302������Ϊ7����RX8010�ǵ�λΪ1

//	Rdata[4] = *(p+3);//DAY
//	Rdata[5] = *(p+4);//MONTH
//	Rdata[6] = *(p+6);//YEAR 2015-01-01-00:00:00-Tursday
	Rdata[4] = p[3];//DAY
	Rdata[5] = p[4];//MONTH
	Rdata[6] = p[6];//YEAR 2015-01-01-00:00:00-Tursday
	writeEEone(0x10,7,Rdata);
	
	Rdata[0] = 0x00;//STOPλΪ0
	writeEEone(0x1F,1,Rdata);	
}
	
//��ȡʱ��
//ע�⣬ԭ����DS1302����ΪRX8010��˳��ͬ�����⣬������8421BCD���Ϊλ
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
		//	// ע�⣺δʹ������
		//	for(i = 1;i > 7; i++)
		//	{
		//		if((*(p+5) >> i)==0)break;
		//	}
		//	*(p+5) = i;
		//	if(*(p+5) == 1)//������
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
