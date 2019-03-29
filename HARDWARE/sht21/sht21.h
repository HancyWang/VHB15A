#ifndef __SHT21_H
#define __SHT21_H 


//#if SENSOR_SEL_SHT21

#define CHECKSUM_ERROR 255
#define POLYNOMIAL 0x131   //P(x)=x^8+x^5+x^4+1 = 100110001


#define SHT21_ERROR    65535 //错误数据
#define SCL_CONF     P12

#define SCL_INPUT_HIGH()   P1M1 |= 0x04;P1M0 &= 0xFB 				//10 仅为输入

//#endif

//---------- Defines -----------------------------------------------------------
//  CRC

uint8_t SHT21_WriteTemp(void);
uint8_t SHT21_WriteRH(void);
uint16_t  SHT21_ReadData(void);
uint8_t SHT2x_CheckCrc(uint8_t Cdata[], uint8_t nbrOfBytes, uint8_t checksum);
//void SHT21_Heater_On_Off(uint8_t);


#endif