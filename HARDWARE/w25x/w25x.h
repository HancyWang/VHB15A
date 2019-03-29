#ifndef __W25X_H
#define __W25X_H 

void	SPI_Write_Enable();
void	SPI_Write_Disable();
void	SPI_init();
void	SPI_Send_Byte(uint8_t);
uint8_t	SPI_Get_Byte(void);
void   	SPI_Read_nBytes(unsigned long Dst_Addr, uint8_t nBytes_128,uint8_t * Read_Adr);
void    SPI_Write_nBytes(unsigned long Dst_Addr, uint8_t nBytes_128,uint8_t * Data_Adr);
void    SPI_Erase_Sector(unsigned long Dst_Addr);
//uint8_t	SPI_Flash_ReadSR(void);
//void    SPI_Flash_Wait_Busy(void);
//void    SPI_Flash_Erase_Chip(void);


#endif