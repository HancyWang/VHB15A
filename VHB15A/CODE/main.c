#include "all.h"

//2019.03.29
sfr WDT_CONTR = 0xC1;

void main(void)
{
	Main_Tik_Tok_Initial_Fun();	//Main timescale initialization 
	Init_port();//IO,LCD,FLASH initialization
	Main_Init();//Initial variables
  HmiEnterToWorkStateFunc(Work_State);//Enter POST or service mode 

	while(1>0)
	{
		WDT_CONTR = 0x3F; //Clear WatchDog,4.55S	
		ManageTask(); //0
		KeyScanTask();//1	
		HmiServiceModeTask();//2
		HmiPostTask();//3
		HmiFactoryDefaultSelTask();//4
		HmiNon_InvasiveSelTask();//5
		HmiRunningTask();//6
		HmiSetTempPatientTask();//7
		HmiSetTempChamberTask();//8
		HmiSetInExpTask();//9
		HmiSetTimeTask();//10
		HmiScreenSaverModeTask();//11
		GetTemp_HpChamberTask();//12
		GetTempHumidity_PatientTask();//13	
		RefreshTempHumidyTask();//14
		RefreshRTCTimeTask();//15
		RefreshRunTimeTask();//16				
		HeaterWireControlTask();//17
		LowPowerModeTask();//18
		HeaterWireModeDetTask();//19
		AlarmErrorTask();//20
		LowTempDet1STask();//21
		SaveDateToFlashTask();//22
		UartRecTask();//23
	}//end while=============================
}

