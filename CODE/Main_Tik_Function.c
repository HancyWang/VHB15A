#include "all.h"
//******************************************************************************
//foreground task, functions placed on timer interrupts
//******************************************************************************

void Main_Tik_FunctionS(void)//Execute tasks every 20mS
{
  WS_SetTaskIfg(ManageTIFG); //Execute task manager once
	HeaterPlateWireDriveFbTask();//Driving and Existence Detection of Heating Plate and Heating Wire
}