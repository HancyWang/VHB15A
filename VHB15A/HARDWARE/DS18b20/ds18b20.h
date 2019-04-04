#ifndef __DS18B20_H
#define __DS18B20_H 

#define DS18b20_ERROR    65535 //加热盘温度

extern uint8_t   Read_18B20_Value;
extern uint8_t   Read_18B20_Value2; 

//void get_temp_ds18b20(uint8_t *Temp_18B20_data);//数据高,数据低,应答位 约5ms
//void DS18B20_Reset(uint8_t *Temp_18B20_data);
void DS18B20_ReadByte(void);
void DS18B20_WriteByte(uint8_t dat);
u8t DS18B20_CheckCrc(const u8t Cdata[], u8t nbrOfBytes, u8t checksum);

#endif