 //#include "STC12C32AD.h"
 #include "all.h" 
 //#include "delay.h"
 //#include "RX8010.h"

 
#define SCL          P25
#define SDA          P26
#define ASK            0
#define NO_ASK         1
#define OUT_LOW        0
#define INPUT_HIGH     1

#define SCL_DIR_OUTPUT()   P2M1 |= 0x10;P2M0 |= 0x10  //11 ��©���
#define SDA_DIR_INPUT()    P2M1 |= 0x40;P2M0 &= 0xBF  //10 ��Ϊ����
#define SDA_DIR_OUTPUT()   P2M1 |= 0x40;P2M0 |= 0x40  //11 ��©���

#define	RX8010_WRITE  0x64 //I2C������ַ
#define RX8010_READ		0x65

//2019.03.29
sfr WDT_CONTR = 0xC1;
sbit EA       = 0xA8^7;
sbit P25 = 0xA0^5;
sbit P26 = 0xA0^6;
sfr P2M0 = 0x96;
sfr P2M1 = 0x95;

///************************************
//** ����ԭ��: void IC_start(void); **
//** �� ��: IC ������ʼλ. **
//************************************/
static void IC_start(void)
{	
		SCL_DIR_OUTPUT();
		SDA_DIR_OUTPUT();	  
		SCL=1;
		SDA=1;
		delay_us(2);
		SDA=0;
		delay_us(2);//
		SCL=0;
}

////**********************************
////** ����ԭ��: void IC_stop(void); **
////** �� ��: IC ����ֹͣλ. **
////**********************************/
static void IC_stop(void)
{
		SCL=0;
	  SDA_DIR_OUTPUT();
		SDA=0;
		delay_us(2);
		SCL=1;
		delay_us(2); //
		SDA=1;
		delay_us(2);
}

/////***************************************************
////** ����ԭ��: bit IC_writebyte(uchar wdata); **
////** �� ��: ��IC ���߷���8 λ����,������һ��Ӧ���� **
////** ��ACK.����յ�ACK Ӧ���򷵻�1(TRUE), **
////** ���򷵻�0(FALSE). **
////***************************************************/
static unsigned char IC_WriteByte( unsigned char wdata)
{
	unsigned char i;
	SDA_DIR_OUTPUT();
	for(i=0;i<8;i++)
	{
		SCL=0;
		delay_us(2);
		if(wdata&0x80) SDA=1;
		else SDA=0;
		delay_us(2);
		SCL=1;
		delay_us(4);
		wdata<<=1;
	}
	SCL=0;
	delay_us(4);
	SDA_DIR_INPUT();
	SCL=1;
	delay_us(4);
	i=0;
	while(SDA)
	{
		if(++i>12){SCL=0;return(0);}
	}
	SCL=0;
	return(1);
}
/////***********************************************
////** ����ԭ��: uchar IC_readbyte(void); **
////** �� ��: ��IC �����Ͻ���8 λ����,�������ܵ� **
////** 8 λ������Ϊһ���ֽڷ���,������Ӧ **
////** ���ź�ACK. **
////***********************************************/
static unsigned char IC_ReadByte(void)
{ 
	unsigned char i;
	unsigned char IC_data=0;
	SCL=0;
	SDA_DIR_OUTPUT();
	SDA=1;
	SDA_DIR_INPUT();	
	for(i=0;i<8;i++)
	{
		SCL=1;
		delay_us(2);
		IC_data<<=1;
		IC_data|=SDA;
		//delay_us(4);
		SCL=1;
		delay_us(2);
		SCL=0;
		delay_us(4);
	}
	delay_us(4);
	SCL=0;
	SDA_DIR_OUTPUT();
	SDA=0;
	return(IC_data);
}
/////***************************************************************
////** ����ԭ��: bit readEEone(uchar instr,uchar addr,uchar num); **
////** �� ��: ��8010 �ж�ȡnum ���ֽڵ�����,�������ж������� **
////** ʽ��Ƭ��Address ��ַ��ʼ������ȡ����.8010 ���� **
////** ��ָ���ĵ�ַ�򷵻�0(FALSE). **
////***************************************************************/
unsigned char readEEone(unsigned char addr,unsigned char num,unsigned char *Data)
{
	unsigned char i;
	IC_start();
	if(IC_WriteByte(RX8010_WRITE)==0)
	{
		IC_stop(); return(0);
	}
	if(IC_WriteByte(addr)==0)
	{
		IC_stop(); return(0);
	}
		IC_start();
	if(IC_WriteByte(RX8010_READ)==0)
	{
		IC_stop(); return(0);
	}
	for(i=0;i<num-1;i++)
	{
		Data[i]=IC_ReadByte();
		SDA_DIR_OUTPUT();				
		SDA=0; /** Send ACK **/		
		SCL=1;
	}
	Data[num-1]=IC_ReadByte();
	SDA_DIR_OUTPUT();	
	SDA=1; /** Send Read End **/
	delay_us(4);
	SCL=1;
	IC_stop();
	return(1);
}

///****************************************************************
//** ����ԭ��: bit writeEEone(uchar instr,uchar addr,uchar num); **
//** �� ��: ��EEbuf[]��Ԫ�е�����д��8010 ��num ���ֽ�. **
//** ����ҳд������ʽ,ÿ��д��ʱ����Ҫָ��Ƭ�ڵ�ַ. **
//** ���8010 ������ָ���ĵ�ַ��ĳ�����͵��ֽ�δ�յ� **
//** Ӧ���ź�ACK,�򷵻�0(FALSE). **
//****************************************************************/
static unsigned char writeEEone(unsigned char addr,unsigned char num,unsigned char *Data)
{
	unsigned char i;
	//gie=0;
	IC_start();
	if(IC_WriteByte(RX8010_WRITE)==0)
	{
		IC_stop(); return(0);
	}
	if(IC_WriteByte(addr)==0)
	{
		IC_stop(); return(0);
	}
	for(i=0;i<num;i++)
	{
		if(IC_WriteByte(Data[i])==0)
		{
			IC_stop(); return(0);
		}
	}
	IC_stop();
	delay_us(4);
	SDA=0;
	SCL=0;
	//gie=1;
	return(1);
} 


//���VLFƵ��ֹͣλ����Ϊ1���ʼ��RX8010
unsigned char RX8010_Initialize(void) 
{
	unsigned char Rdata[7];
	unsigned char i;
	unsigned char Init_State;
	
	EA = 0;	
	Init_State = RX8010_INIT_NONE;//δ��ʼ��
	
	Rdata[0] = 0x00;
	for(i = 0;i < 10;i++)
	{
		if(readEEone(0x1E,1,Rdata) == 1)
		{
			Init_State = RX8010_INIT_NONE;
			break;//��VLF״̬
		}
		delay_ms(100);
		WDT_CONTR = 0x3F; 
		Init_State = RX8010_INIT_FAIL;//��ʼ��ʧ��
	}
	
	if(Init_State == RX8010_INIT_NONE)
	{		
		if(Bit_is_one(Rdata[0],1))//�ж��Ƿ�ֹͣ,VLF=1
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
	EA = 1;
	return Init_State;
}
 
//�趨ʱ��
//ע�⣬ԭ����DS1302����ΪRX8010��˳��ͬ�����⣬������8421BCD���Ϊλ
void RX8010_SetTime(BYTE *p) 
{
	unsigned char Rdata[7];
	
	Rdata[0] = 0x40;//STOPλΪ1
	writeEEone(0x1F,1,Rdata);
	
	Rdata[0] = *p;//SEC
	Rdata[1] = *(p+1);//MIN
	Rdata[2] = *(p+2);//HOUR	
	Rdata[3] = *(p+5);//WEEK
/*
//	data[3] = 1 << *(p+5); //WEEK��DS1302ת��ΪRX8010 
//	if(data[3]==0x80)data[3]=1;//DS1302������Ϊ7����RX8010�ǵ�λΪ1
*/
	Rdata[4] = *(p+3);//DAY
	Rdata[5] = *(p+4);//MONTH
	Rdata[6] = *(p+6);//YEAR 2015-01-01-00:00:00-Tursday
	writeEEone(0x10,7,Rdata);
	
	Rdata[0] = 0x00;//STOPλΪ0
	writeEEone(0x1F,1,Rdata);	
}
	
//��ȡʱ��
//ע�⣬ԭ����DS1302����ΪRX8010��˳��ͬ�����⣬������8421BCD���Ϊλ
void RX8010_GetTime(BYTE *p) 
{
	unsigned char Rdata[7];
	
	readEEone(0x10,7,Rdata);
	*p = Rdata[0];//SEC	
	*(p+1) = Rdata[1];//MIN
	*(p+2) = Rdata[2];//HOUR
	*(p+5) = Rdata[3];//WEEK
	/* ע�⣺δʹ������
	for(i = 1;i > 7; i++)
	{
		if((*(p+5) >> i)==0)break;
	}
	*(p+5) = i;
	if(*(p+5) == 1)//������
	{
		*(p+5) = 7;
	}
	else
	{
		*(p+5) -=1;
	}
	*/
	
	*(p+3) = Rdata[4];//DAY
	*(p+4) = Rdata[5];//MONTH
	*(p+6) = Rdata[6];//YEAR 2015-01-01-00:00:00-Tursday
}
