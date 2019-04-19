 #include "STC12C32AD.h"
 #include "all.h"
// #include "w25x.h"
// #include "lcd.h"
// #include "INTRINS.H"	

//#define 	W25X_WP	   P1^3;
#define	  W25X_CS	  P45
#define	  W25X_DI   P44
#define	  W25X_DO	  P42
#define	  W25X_CLK  P41

#define W25X_WriteEnable		  0x06
#define W25X_WriteDisable		  0x04
//#define W25X_ReadStatusReg		0x05
//#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			    0x03
//#define W25X_FastReadData		  0x0B
//#define W25X_FastReadDual		  0x3B
#define W25X_PageProgram		  0x02
//#define W25X_BlockErase			  0xD8
#define W25X_SectorErase		  0x20
//#define W25X_ChipErase			  0xC7
//#define W25X_PowerDown			  0xB9
//#define W25X_ReleasePowerDown	0xAB
//#define W25X_DeviceID			    0xAB
//#define W25X_ManufactDeviceID	0x90
//#define W25X_JedecDeviceID		0x9F

//#define nop() _nop_()


////2019.03.30
//sbit P17 = (u8)0x90^(u8)7;
//sbit P41 = (u8)0xC0^(u8)1;
//sbit P42 = (u8)0xC0^(u8)2;
//sbit P44 = (u8)0xC0^(u8)4;
//sbit P45 = (u8)0xC0^(u8)5;
					  
//unsigned char Read_Data; 

void    SPI_Write_Enable() //写使能
{   W25X_CS = (bit)0;                            //  enable device
	 SPI_Send_Byte(W25X_WriteEnable);        //  send W25X_Write_Enable command
	 W25X_CS = (bit)1;                            //  disable device
}
void    SPI_Write_Disable() //写禁止
{
	 W25X_CS = (bit)0;                            //  enable device
	 SPI_Send_Byte(W25X_WriteDisable);       //  send W25X_WriteW25X_DIsable command
	 W25X_CS = (bit)1;                            //  disable device
}

//void nop(void)
//{
//	_nop_();
//}
// 
//读N个字节
//起始地址
//读字节的个数,最多128
//存放的地址
void   SPI_Read_nBytes(uint32 Dst_Addr, uchar nBytes_128,uchar * Read_Adr)
{
	uchar i = 0;
	LCD_1DIR_H;
	W25X_CS = (bit)0;                                        //  enable device
	SPI_Send_Byte(W25X_ReadData);                       //  read command
	SPI_Send_Byte((u8)((Dst_Addr & (uint32)0xFFFFFF) >> 16));       //  send 3 address bytes
	SPI_Send_Byte((u8)((Dst_Addr & (uint32)0xFFFF) >> 8));
	SPI_Send_Byte((u8)(Dst_Addr & (uint32)0xFF));
	// SPI_Send_Byte(0xFF);                                //  dummy byte
	for (; i < nBytes_128; i++)                     //  read until no_bytes is reached
	{
		Read_Adr[i] = SPI_Get_Byte();                  //  receive byte and store at address 80H - FFH
	}
		 
	W25X_CS = (bit)1;                                         //  disable device
	LCD_1DIR_L;
}

//写N个字节
//起始地址
//写字节的个数,最多128
//存放的地址
 void    SPI_Write_nBytes(uint32 Dst_Addr, uchar nBytes_128,const uchar * Data_Adr)
{
	uchar i, byte;
	W25X_CS = (bit)0;                    /* enable device */
	SPI_Write_Enable();             /* set WEL */
	W25X_CS = (bit)0;
	SPI_Send_Byte(W25X_PageProgram);        /* send Byte Program command */
	SPI_Send_Byte((u8)((Dst_Addr & (uint32)0xFFFFFF) >> 16)); /* send 3 address bytes */
	SPI_Send_Byte((u8)((Dst_Addr & (uint32)0xFFFF) >> 8));
	SPI_Send_Byte((u8)(Dst_Addr & (uint32)0xFF));
	for (i = 0; i < nBytes_128; i++)
	{
		//         byte = *(Data_Adr+i);
		byte = Data_Adr[i];
		SPI_Send_Byte(byte);        /* send byte to be programmed */
	}
	W25X_CS = (bit)1;                /* disable device */
}

 //初始化
 void    SPI_init()
 {
 	 W25X_CLK = (bit)0;                         //  set clock to low initial state for SPI operation mode 0
     W25X_CS = (bit)1;
     _nop_();_nop_(); _nop_();_nop_();
     SPI_Write_Disable();
 }

 //发送一个字节
 void    SPI_Send_Byte(uchar out)
{
	uchar i = 0;
	uchar OUT=out;
	for (; i < (u8)8; i++)
	{
		if ((OUT & (uchar)0x80) == (uchar)0x80)           //  check if MSB is high
		{
			W25X_DI = (bit)1;
		}
		else
		{
			W25X_DI = (bit)0;                    //  if not, set to low
		}
			

		W25X_CLK = (bit)1;                       //  toggle clock high
		OUT = (uint8_t)(OUT << 1);                   //  shift 1 place for next bit
		_nop_();_nop_(); _nop_();_nop_();
		W25X_CLK = (bit)0;                       //  toggle clock low
	}
}

//读取一个字节
uchar   SPI_Get_Byte(void)
{
	uchar i = 0, in = 0;
	uchar temp;
	for (; i < (u8)8; i++)
	{
		in = (uint8_t)(in << 1);                      //  shift 1 place to the left or shift in 0
//		temp = W25X_DO;                      //  save input
		if(W25X_DO)
		{
			temp=(uchar)1;
		}
		else
		{
			temp=(uchar)0;
		}
		W25X_CLK = (bit)1; 
		_nop_();                              //  toggle clock high
		if (temp==(uchar)1)                        //  check to see if bit is high
		{
			in |= (uchar)0x01;                     //  if high, make bit high
		}
			 
		_nop_();
		_nop_();
		_nop_(); 
		_nop_(); 
		W25X_CLK = (bit)0;                       //  toggle clock low
	}
	return in;
}

 //擦除指定的扇区
 void    SPI_Erase_Sector(uint32 Dst_Addr)
 {
 	 W25X_CS = (bit)0;                                        //  enable device
     SPI_Write_Enable();                                 //  set WEL
     W25X_CS = (bit)0;
     SPI_Send_Byte(W25X_SectorErase);                    //  send Sector Erase command
     SPI_Send_Byte((uchar)((Dst_Addr & (uint32)0xFFFFFF) >> 16));//    send 3 address bytes
     SPI_Send_Byte((uchar)((Dst_Addr & (uint32)0xFFFF) >> 8));
     SPI_Send_Byte((uchar)(Dst_Addr & (uint32)0xFF));
     W25X_CS = (bit)1;                                        //  disable device
 }

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//uchar SPI_Flash_ReadSR(void)   
//{  
//	uchar byte=0;   
//	W25X_CS=0;                            //使能器件 
//	SPI_Send_Byte(W25X_ReadStatusReg);    //发送读取状态寄存器命令 
//	LCD_1DIR_H;
//	byte=SPI_Get_Byte();             //读取一个字节  
//	LCD_1DIR_L;
//	W25X_CS=1;                            //取消片选     
//	return byte;   
//}


////等待空闲
//void SPI_Flash_Wait_Busy(void)   
//{   
//	while((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
//}  

////擦除整个芯片		  
////等待时间超长...
//void SPI_Flash_Erase_Chip(void)   
//{                                   
//    SPI_Write_Enable(); 	                  //SET WEL 
//    SPI_Flash_Wait_Busy();   
//  	W25X_CS=0;                            //使能器件   
//    SPI_Send_Byte(W25X_ChipErase);        //发送片擦除命令  
//	W25X_CS=1;                            //取消片选     	      
//	SPI_Flash_Wait_Busy();   				   //等待芯片擦除结束
//}   

