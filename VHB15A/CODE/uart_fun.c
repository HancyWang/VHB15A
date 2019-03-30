/**
  ******************************************************************************

  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "uart_fun.h"
#include "all.h"

//2019.03.29
sfr TMOD = 0x89;
sfr TH1  = 0x8D;
sbit TI  = 0x98^1;
sbit TR1 = 0x88^6;

/**
  Section: Macro Declarations
*/
//#define EUSART_TX_BUFFER_SIZE 64
#define EUSART_RX_BUFFER_SIZE 128

static uint8_t eusartRxHead = 0; //从头计数
static uint8_t eusartRxTail = 0;
static uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;

static uint8_t Send_uart_busy = 0;// 发送忙标志

/**
  Section: EUSART APIs
*/

//2019.03.29
sfr ACC  = 0xE0; 
sfr PCON   = 0x87;
sfr SBUF = 0x99;
sfr IE = 0xA8;
sfr SCON = 0x98;

//串口初始化
void EUSART_Initialize(void)
{ 

    SCON  = 0x50;       //SCON: serail mode 3, 9-bit UART, enable ucvr
    TMOD |= 0x20;       //TMOD: timer 1, mode 2, 8-bit reload
    PCON |= 0x24;        //SMOD=1; 24
    TH1  = 0xFD;       //fosc=11.0592MHz //9600:FD 22.1184 19200*2
    IE   |= 0x90;       //Enable Serial Interrupt
    TR1   = 1;          // timer 1 run

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;  
}

//从串口接收缓冲区读一个字节
uint8_t EUSART_Read(void)
{
    uint8_t readValue  = 0; 
    readValue = eusartRxBuffer[eusartRxTail++];//读出计数
    if(sizeof(eusartRxBuffer) <= eusartRxTail) //读出从头计数
    {
        eusartRxTail = 0;
    }
    eusartRxCount--;
    return readValue;
}

//发送一个字节到串口
static void send_char_com(unsigned char  Send_ch)
{
    ACC=Send_ch;
    SBUF=ACC;	
		Send_uart_busy = 1;
}

//写一个字节到串口发送缓冲区
void EUSART_Write(uint8_t txData)
{
	while(Send_uart_busy);
	send_char_com(txData);
}

//发送cnt个字符到发送缓冲区
void EUSART_Write_Str(const unsigned char *stra,unsigned char cnt)//发送字符串或数组到串口
{
    unsigned char i;    
    for(i=0;i<cnt;i++)
    {
      EUSART_Write(*(stra+i));  
    }		
}

void EUSART_Transmit_ISR(void)
{   
		TI=0;	
		Send_uart_busy = 0;
}

//串口中断接收服务程序
void EUSART_Receive_ISR(void)
{      

    eusartRxBuffer[eusartRxHead++] = SBUF;

    if(sizeof(eusartRxBuffer) <= eusartRxHead)//从头计数
    {
        eusartRxHead = 0;
    }
    eusartRxCount++;
}
 

/************************ (C) COPYRIGHT *****END OF FILE****/
