#ifndef __RX8010_H
#define __RX8010_H 

#define RX8010_INIT_NONE		0
#define RX8010_INIT_OK			1
#define RX8010_INIT_FAIL		2

uint8_t RX8010_Initialize(void);
void RX8010_SetTime(uint8_t *p); 
void RX8010_GetTime(uint8_t *p);

uint8_t readEEone(uint8_t addr,uint8_t num,uint8_t *Data);


#endif