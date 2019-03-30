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

static uint8_t eusartRxHead = 0; //��ͷ����
static uint8_t eusartRxTail = 0;
static uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;

static uint8_t Send_uart_busy = 0;// ����æ��־

/**
  Section: EUSART APIs
*/

//2019.03.29
sfr ACC  = 0xE0; 
sfr PCON   = 0x87;
sfr SBUF = 0x99;
sfr IE = 0xA8;
sfr SCON = 0x98;

//���ڳ�ʼ��
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

//�Ӵ��ڽ��ջ�������һ���ֽ�
uint8_t EUSART_Read(void)
{
    uint8_t readValue  = 0; 
    readValue = eusartRxBuffer[eusartRxTail++];//��������
    if(sizeof(eusartRxBuffer) <= eusartRxTail) //������ͷ����
    {
        eusartRxTail = 0;
    }
    eusartRxCount--;
    return readValue;
}

//����һ���ֽڵ�����
static void send_char_com(unsigned char  Send_ch)
{
    ACC=Send_ch;
    SBUF=ACC;	
		Send_uart_busy = 1;
}

//дһ���ֽڵ����ڷ��ͻ�����
void EUSART_Write(uint8_t txData)
{
	while(Send_uart_busy);
	send_char_com(txData);
}

//����cnt���ַ������ͻ�����
void EUSART_Write_Str(const unsigned char *stra,unsigned char cnt)//�����ַ��������鵽����
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

//�����жϽ��շ������
void EUSART_Receive_ISR(void)
{      

    eusartRxBuffer[eusartRxHead++] = SBUF;

    if(sizeof(eusartRxBuffer) <= eusartRxHead)//��ͷ����
    {
        eusartRxHead = 0;
    }
    eusartRxCount++;
}
 

/************************ (C) COPYRIGHT *****END OF FILE****/
