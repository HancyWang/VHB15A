#include "all.h"
//******************************************************************************
//foreground Assist task, functions placed on timer interrupts
//******************************************************************************
//******************************************************************************


void Sub_Tik_FunctionS(void) //Execute tasks every 100mS
{ 
	Key_UP_TikFunc();//Key Detection Time Scale Execution
	Key_Down_TikFunc();
	Key_OK_TikFunc();
	Key_Mute_TikFunc();
	
	HmiPostFunc_Tik_Cnt();//UI Time Scale Execution
	HmiFactoryDefault_Tik_Cnt();
	HmiNon_InvasiveSel_Tik_Cnt();
	HmiScreenSaverMode_Tik_Cnt();
}												  
