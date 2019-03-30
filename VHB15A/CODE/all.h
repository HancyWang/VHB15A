/// @file all.h
/// @brief define IO port,Load header file
/// @author Hancy
/// @date 2019.03.26
/// @version 2.0.3a
/// @note 

#ifndef __ALL_H
#define __ALL_H

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef u8t
typedef unsigned char u8t;
#endif
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif
#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef UINT8
typedef unsigned char UINT8;
#endif

#ifndef uint
typedef unsigned int  uint;
#endif
#ifndef u16t
typedef unsigned int  u16t;
#endif
#ifndef uint16
typedef unsigned int uint16;
#endif
#ifndef uint16_t
typedef unsigned int uint16_t;
#endif

#ifndef int16_t
typedef int int16_t;
#endif


#ifndef uint32
typedef unsigned long uint32;
#endif
#ifndef UINT32
typedef unsigned long UINT32;
#endif


typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus, BitStatus, BitAction;

#include "STC12C32AD.h"
#include "stc_eeprom.h"
#include "INTRINS.H" 
#include "MATH.h"
#include "STRING.H"
#include "delay.h"
#include "w25x.h" 
#include "sht21.h"
#include "lcd.h"
#include "ds18b20.h"
#include "RX8010.h"
#include "Main_Tik_Tok.h"//1
#include "Main_Tik_Function.h"//2
#include "Sub_Tik_Function.h"//3
#include "Normal_Task.h"//4
#include "WS_Task_Manage.h"//5
#include "ActivityFunctions.h"//6
#include "Key_Multifun.h"
#include "key_datas.h"
#include "GUI.h"
#include "uart_fun.h"
#include "Sound_Type.h"
#include "absacc.h"
#include "StcRomCrc16.h"

#define VHB15A_VER "2.0.3A"//software version
#define DATE_VER "190318"	//

#define  Clear_bit(Value,bit_num)    (Value) =(Value)& ( ~ ( 1 << (bit_num) ) )
#define  Set_bit(Value,bit_num)     (Value) =(Value)| ( 1 << (bit_num) )
#define  Cpl_bit(Value,bit_num)      (Value) =(Value)^ ( 1 << (bit_num) )
#define  Bit_is_one(Value,bit_num)   ( (Value) & (1<< (bit_num) ) )
#define  Bit_is_zero(Value,bit_num)  ( !( (Value) & ( 1<< (bit_num) ) ) ) 


//端口定义区===============================================
#define  HeatingPlateSensor_Port  		P37 //Heating plate DS18B20 sensor port
#define  ChamberOutletSensor_Port  		P10 //Chabmer outlet DS18B20 sensor port
#define	 Buzzer_Port  								P43 //
#define  KEY_RIGHT_UP_IN     					P23 //OK   PIN21
#define  KEY_LEFT_UP_IN     					P20 //UP   PIN20
#define  KEY_LEFT_DOWN_IN   					P21	//DOWN PIN19
#define  KEY_RIGHT_DOWN_IN    				P22	//VOL  PIN18
#define  Heat_JRP_Port            		P35
#define  Heat_WireIn_Port         		P36
#define  Heat_WireOut_Port       		 	P27
#define  JRP_Exist_Port          		 	P33
#define  WireIn_Exist_Port        		P34
#define  WireOut_Exist_Port       		P13	 
#define	 Heat_WIRE_EN_OUT			      	P32

#define Const_KEY_UP      13  //UP KEY
#define Const_KEY_OK      7   //OK KEY
#define Const_KEY_DOWN    14  //DOWN KEY
#define Const_KEY_VOL     11  //VOL KEY


//typedef unsigned char BYTE;
//typedef unsigned char uchar;
//typedef unsigned char u8t;
//typedef unsigned char uint8_t;
//typedef unsigned char uint8;
//typedef unsigned char UINT8;

//typedef unsigned int  uint;
//typedef unsigned int  u16t;
//typedef unsigned int uint16;

//typedef unsigned long uint32;
//typedef unsigned long UINT32;

//typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus, BitStatus, BitAction;



#endif