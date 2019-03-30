 #include "STC12C32AD.h"
 #include "all.h"
 #include "w25x.h"
 #include "lcd.h"
 #include "INTRINS.H"	

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

#define nop() _nop_()

//2019.03.30
sbit P17 = 0x90^7;
sbit P41 = 0xC0^1;
sbit P42 = 0xC0^2;
sbit P44 = 0xC0^4;
sbit P45 = 0xC0^5;
					  
//unsigned char Read_Data; 

 void    SPI_Write_Enable() //дʹ��
 {   W25X_CS = 0;                            //  enable device
     SPI_Send_Byte(W25X_WriteEnable);        //  send W25X_Write_Enable command
     W25X_CS = 1;                            //  disable device
 }
 void    SPI_Write_Disable() //д��ֹ
 {
     W25X_CS = 0;                            //  enable device
     SPI_Send_Byte(W25X_WriteDisable);       //  send W25X_WriteW25X_DIsable command
     W25X_CS = 1;                            //  disable device
 }

//��N���ֽ�
//��ʼ��ַ
//���ֽڵĸ���,���128
//��ŵĵ�ַ
 void   SPI_Read_nBytes(uint32 Dst_Addr, uchar nBytes_128,uchar * Read_Adr)
 {
 	   uchar i = 0;
 	   LCD_1DIR_H;
     W25X_CS = 0;                                        //  enable device
     SPI_Send_Byte(W25X_ReadData);                       //  read command
     SPI_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16));       //  send 3 address bytes
     SPI_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
     SPI_Send_Byte(Dst_Addr & 0xFF);
    // SPI_Send_Byte(0xFF);                                //  dummy byte
     for (i = 0; i < nBytes_128; i++)                     //  read until no_bytes is reached
         *(Read_Adr+i) = SPI_Get_Byte();                  //  receive byte and store at address 80H - FFH
     W25X_CS = 1;                                         //  disable device
     LCD_1DIR_L;
 }

//дN���ֽ�
//��ʼ��ַ
//д�ֽڵĸ���,���128
//��ŵĵ�ַ
 void    SPI_Write_nBytes(uint32 Dst_Addr, uchar nBytes_128,uchar * Data_Adr)
 {
     uchar i, byte;
     W25X_CS = 0;                    /* enable device */
     SPI_Write_Enable();             /* set WEL */
     W25X_CS = 0;
     SPI_Send_Byte(W25X_PageProgram);        /* send Byte Program command */
     SPI_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16)); /* send 3 address bytes */
     SPI_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
     SPI_Send_Byte(Dst_Addr & 0xFF);
     for (i = 0; i < nBytes_128; i++)
     {
         byte = *(Data_Adr+i);
         SPI_Send_Byte(byte);        /* send byte to be programmed */
     }
     W25X_CS = 1;                /* disable device */
 }

 //��ʼ��
 void    SPI_init()
 {
 	 W25X_CLK = 0;                         //  set clock to low initial state for SPI operation mode 0
     W25X_CS = 1;
     nop();nop(); nop();nop();
     SPI_Write_Disable();
 }

 //����һ���ֽ�
 void    SPI_Send_Byte(uchar out)
 {
 	   uchar i = 0;
     for (i = 0; i < 8; i++)
     {
     	  if ((out & 0x80) == 0x80)           //  check if MSB is high
             W25X_DI = 1;
         else
             W25X_DI = 0;                    //  if not, set to low
         W25X_CLK = 1;                       //  toggle clock high
         out = (out << 1);                   //  shift 1 place for next bit
         nop();nop(); nop();nop();
         W25X_CLK = 0;                       //  toggle clock low
     }
 }

//��ȡһ���ֽ�
 uchar   SPI_Get_Byte(void)
 {
 	   uchar i = 0, in = 0;
 	   bit temp;
     for (i = 0; i < 8; i++)
     {
     	   in = (in << 1);                      //  shift 1 place to the left or shift in 0
         temp = W25X_DO;                      //  save input
         W25X_CLK = 1; 
         nop();                              //  toggle clock high
         if (temp==1)                        //  check to see if bit is high
             in |= 0x01;                     //  if high, make bit high
         nop();
         nop();
         nop(); 
         nop(); 
         W25X_CLK = 0;                       //  toggle clock low
     }
     return in;
 }

 //����ָ��������
 void    SPI_Erase_Sector(uint32 Dst_Addr)
 {
 	 W25X_CS = 0;                                        //  enable device
     SPI_Write_Enable();                                 //  set WEL
     W25X_CS = 0;
     SPI_Send_Byte(W25X_SectorErase);                    //  send Sector Erase command
     SPI_Send_Byte((uchar)((Dst_Addr & 0xFFFFFF) >> 16));//    send 3 address bytes
     SPI_Send_Byte((uchar)((Dst_Addr & 0xFFFF) >> 8));
     SPI_Send_Byte((uchar)Dst_Addr & 0xFF);
     W25X_CS = 1;                                        //  disable device
 }

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//uchar SPI_Flash_ReadSR(void)   
//{  
//	uchar byte=0;   
//	W25X_CS=0;                            //ʹ������ 
//	SPI_Send_Byte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ������� 
//	LCD_1DIR_H;
//	byte=SPI_Get_Byte();             //��ȡһ���ֽ�  
//	LCD_1DIR_L;
//	W25X_CS=1;                            //ȡ��Ƭѡ     
//	return byte;   
//}


////�ȴ�����
//void SPI_Flash_Wait_Busy(void)   
//{   
//	while((SPI_Flash_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
//}  

////��������оƬ		  
////�ȴ�ʱ�䳬��...
//void SPI_Flash_Erase_Chip(void)   
//{                                   
//    SPI_Write_Enable(); 	                  //SET WEL 
//    SPI_Flash_Wait_Busy();   
//  	W25X_CS=0;                            //ʹ������   
//    SPI_Send_Byte(W25X_ChipErase);        //����Ƭ��������  
//	W25X_CS=1;                            //ȡ��Ƭѡ     	      
//	SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
//}   

