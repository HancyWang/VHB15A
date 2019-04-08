#ifndef __W25X_H
#define __W25X_H 

//void nop(void);

void	SPI_Write_Enable(void);
void	SPI_Write_Disable(void);
void	SPI_init(void);
void	SPI_Send_Byte(uchar out);
uchar   SPI_Get_Byte(void);
void   SPI_Read_nBytes(uint32 Dst_Addr, uchar nBytes_128,uchar * Read_Adr);
void    SPI_Write_nBytes(uint32 Dst_Addr, uchar nBytes_128,const uchar * Data_Adr);
void    SPI_Erase_Sector(uint32 Dst_Addr);
//uint8_t	SPI_Flash_ReadSR(void);
//void    SPI_Flash_Wait_Busy(void);
//void    SPI_Flash_Erase_Chip(void);


#endif