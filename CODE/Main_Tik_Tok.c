#include "all.h"

//******************************************************************************
//******************************************************************************
static unsigned  int Sub_1_TikCount = 0;  
//******************************************************************************

//2019.03.29
sfr AUXR  = 0x8E;
sfr TMOD = 0x89;
sfr TL0  = 0x8A;
sfr TH0  = 0x8C;
sbit ET0      = 0xA8^1;
sbit TF0 = 0x88^5;
sbit TR0 = 0x88^4;

//******************************************************************************
void Main_Tik_Tok_Initial_Fun(void)//Initialize the timer(20mS)
{
	AUXR &= 0x7F;		//Timer 12T mode
	TMOD &= 0xF0;		//set Timer mode
	TMOD |= 0x01;		//
	TL0 = 0x00;		 	//Timer0 Initial Value
	TH0 = 0x70;			//
	TF0 = 0;		 		//clear TF0 flag
	TR0 = 1;				//Timer0 enable
	ET0=1;					//Timer0 interrupt enable
}

//*****************************************************************************
//*****************************************************************************
void Timer_T0_INT_Main(void) //20mS
{
	TL0 = 0x00;		//Timer0 set value
	TH0 = 0x70;		//
	
  Main_Tik_FunctionS();  //execute foreground task once
	Sound_Type_Drive();		 //Sound drive
  Sub_1_TikCount++; 		
  if(Sub_1_TikCount>=5)  //100mS
	{
		Sub_1_TikCount=0;
		Sub_Tik_FunctionS();  //execute auxiliary foreground task once 
  }
}