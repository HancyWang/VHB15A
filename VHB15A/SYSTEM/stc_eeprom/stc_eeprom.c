//#include <reg51.H>
#include <intrins.H>
#include "stc_eeprom.h"

sfr IAP_DATA    = 0xC2;
sfr IAP_ADDRH   = 0xC3;
sfr IAP_ADDRL   = 0xC4;
sfr IAP_CMD     = 0xC5;
sfr IAP_TRIG    = 0xC6;
sfr IAP_CONTR   = 0xC7;

//Define the waiting time for Flash operations and the constants that allow IAP/ISP/EEPROM operations
#define ENABLE_ISP 0x82 //

//#define DATA_FLASH_START_ADDRESS 0x00  //STC5Axx Series EEPROM Starting Address

static union 
{
    INT16U un_temp16;
    INT8U  un_temp8[2];
}my_unTemp16;


static void IAP_Disable(void);  //Disable IAP function 



INT8U STC_IAP_Byte_Read(INT16U add)
{
    IAP_DATA = 0x00;
    IAP_CONTR = ENABLE_ISP;         //Enable IAP
    IAP_CMD = 0x01;                 //IAP/ISP/EEPROM read byte command

    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];   //Setting target address high 8bit
    IAP_ADDRL = my_unTemp16.un_temp8[1];   //Setting target address low 8bit

    IAP_TRIG = 0x5A;   //command 0X5A
    IAP_TRIG = 0xA5;   //command 0XA5
    _nop_();

    IAP_Disable();  //Disable IAP

    return (IAP_DATA);
}

void STC_IAP_Byte_Program(INT16U add, INT8U ch)
{
    IAP_CONTR = ENABLE_ISP;         //Enable IAP
    IAP_CMD = 0x02;                 //IAP/ISP/EEPROM write byte command

    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //Setting target address high 8bit
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //Setting target address low 8bit

    IAP_DATA = ch;    

    IAP_TRIG = 0x5A;   //
    IAP_TRIG = 0xA5;   //
    _nop_();

    IAP_Disable();  //Disable IAP
}

void STC_IAP_Sector_Erase(INT16U add)
{
    IAP_CONTR = ENABLE_ISP;         //Enable IAP
    IAP_CMD = 0x03;                 //IAP/ISP/EEPROM erase sector command

    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //Setting target address high 8bit
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //Setting target address low 8bit

    IAP_TRIG = 0x5A;   //
    IAP_TRIG = 0xA5;   //
    _nop_();

    IAP_Disable();  //Disable IAP
}

static void IAP_Disable(void) //Disable IAP
{ 
    IAP_CONTR = 0;      //
    IAP_CMD   = 0;      //
    IAP_TRIG  = 0;      //
    IAP_ADDRH = 0;
    IAP_ADDRL = 0;
}





