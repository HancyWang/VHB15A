/**
  EUSART Generated Driver API Header File 
 */

#ifndef __UART_FUN_H
#define __UART_FUN_H
#include "data_type.h"
#define EUSART_DataReady  (eusartRxCount)

////#ifndef uint8_t
////typedef uint8_t uint8_t;
////#endif
//#ifndef uint8_t
//typedef uint8_t uint8_t;
//#endif

extern volatile uint8_t eusartTxBufferRemaining;
extern volatile uint8_t eusartRxCount;

void EUSART_Initialize(void);

uint8_t EUSART_Read(void);
 
void EUSART_Write(uint8_t txData);

void EUSART_Transmit_ISR(void);

void EUSART_Receive_ISR(void);

void EUSART_Write_Str(const uint8_t *stra,uint8_t cnt);//发送字符串或数组到串口
     
#endif  // _EUSART_H

/**
 End of File
*/