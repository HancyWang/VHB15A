/**
  EUSART Generated Driver API Header File 
 */

#ifndef _EUSART_H
#define _EUSART_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
  Section: Macro Declarations
*/

#define EUSART_DataReady  (eusartRxCount)

/**
  Section: Data Type Definitions
*/

/**
 Section: Global variables
 */
extern volatile uint8_t eusartTxBufferRemaining;
extern volatile uint8_t eusartRxCount;

void EUSART_Initialize(void);

uint8_t EUSART_Read(void);
 
void EUSART_Write(uint8_t txData);

void EUSART_Transmit_ISR(void);

void EUSART_Receive_ISR(void);

/**
  @Summary
    Read a byte of data from the EUSART.

  @Description
    This routine reads a byte of data from the EUSART.

  @Preconditions
    EUSART_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if the receiver is not empty before calling this function.
	
	EUSART_DataReady is a macro which checks if any byte is received.
	Call this macro before using this function.

  @Param
    None

  @Returns
    A data byte received by the driver.
	
  @Example
	<code>
            void main(void) {
								// initialize the device
								SYSTEM_Initialize();
								uint8_t data;
								
								// Enable the Global Interrupts
								INTERRUPT_GlobalInterruptEnable();
								
								// Enable the Peripheral Interrupts
								INTERRUPT_PeripheralInterruptEnable();
								
								printf("\t\tTEST CODE\n\r");		//Enable redirect STDIO to USART before using printf statements
								printf("\t\t---- ----\n\r");
								printf("\t\tECHO TEST\n\r");
								printf("\t\t---- ----\n\n\r");
								printf("Enter any string: ");
								do{
								data = EUSART1_Read();		// Read data received
								EUSART_Write(data);			// Echo back the data received
								}while(!EUSART1_DataReady);		//check if any data is received
								
							}
    </code>
*/


#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif  // _EUSART_H
/**
 End of File
*/
