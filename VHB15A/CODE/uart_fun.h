/**
  EUSART Generated Driver API Header File 
 */

#ifndef __UART_FUN_H
#define __UART_FUN_H

#define EUSART_DataReady  (eusartRxCount)

////#ifndef uint8_t
////typedef unsigned char uint8_t;
////#endif
//#ifndef uint8_t
//typedef unsigned char uint8_t;
//#endif

extern volatile unsigned char eusartTxBufferRemaining;
extern volatile unsigned char eusartRxCount;

void EUSART_Initialize(void);

unsigned char EUSART_Read(void);
 
void EUSART_Write(unsigned char txData);

void EUSART_Transmit_ISR(void);

void EUSART_Receive_ISR(void);

void EUSART_Write_Str(const unsigned char *stra,unsigned char cnt);//发送字符串或数组到串口
     
#endif  // _EUSART_H

/**
 End of File
*/