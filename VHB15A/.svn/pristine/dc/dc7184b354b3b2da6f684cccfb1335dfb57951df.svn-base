#ifndef __NORMAL_TASK__
#define __NORMAL_TASK__

#include "key_datas.h"

#define ManageTIFG                      0//Task Manager  20mS executes once
#define KeyScanTIFG                     1//Key scanning 20mS executes once
#define HmiServiceModeTIFG      			  2//HMI Service mode 100mS executes once
#define HmiPostTIFG      			  				3//HMI power on self test 100mS
#define HmiFactoryDefaultSelTIFG      	4//HMI previous or Default select 100mS
#define HmiNon_InvasiveSelTIFG      	  5//HMI Invasiver or Noninvasive mode select 100mS
#define HmiRunningTIFG      			      6//HMI Running status 100mS
#define HmiSetTempPatientTIFG      			7//HMI Patinet side temperature setting 100mS
#define HmiSetTempChamberTIFG      			8//HMI Chamber outlet temperature setting 100mS
#define HmiSetInExpTIFG      			      9//HMI InExp setting 100mS
#define HmiSetTimeTIFG      			      10//HMI Real time clock setting 100mS
#define HmiScreenSaverModeTIFG      	  11//HMI Screen saver 100mS
#define GetTemp_HpChamberTIFG           12//Get the heating plate and chamber outlet temperature 20mS
#define GetTempHumidity_PatientTIFG     13//Get the patient side temperature 20mS
#define RefreshTempHumidyTIFG      			14//Update the patient side temperature and humidity displayed on the screen 240mS
#define RefreshRunTimeTIFG     					15//Update running time displayed on the screen 100mS
#define RefreshRTCTimeTIFG      			  16//Update real time clock displayed on the screen 100mS
#define HeaterPlateWireControlTIFG     	17//Control of heating plate and heating wire Circuit 1S
#define LowPowerModeTIFG     						18//Low power mode 1S
#define HeaterWireModeDetTIFG     			19//Heating wire status Detection 1S
#define AlarmErrorTIFG      			      20//Alarm and error event handling 100mS
#define LowTempDet1STIFG      	        21//Low Temperature and Low Humidity Detection 1S
#define SaveDateToFlashTIFG      			  22//Save data to FLASH 1S
#define UartRecTIFG                     23//Serial Port Receiving Task 20mS


//******************************************************************************
//UI状态枚举参数 用UI状态来决定加载哪些任务
//******************************************************************************
typedef enum
{
  UI_STATE_POWER_OFF_MODE=0,    				//关机模式   1
	UI_STATE_SERVICE_MODE=1,   				    //服务模式   1
	UI_STATE_POST_MODE,    				        //POST模式   1/12/13
  UI_STATE_FACTORY_DEFAULT_SEL_MODE,    //开机时默认和上一次选择界面 1/3
  UI_STATE_NON_INVASIVE_MODE,           //开机时的有创无创选择界面 1/5
  UI_STATE_RUNNING_NORMAL_MODE, 				//运行模式 1/6/11/12/13/14/15/16/17/18/19/20/21/22/23
	UI_STATE_SCREENSAVER_MODE,  				  //屏保模式 1/6/11/12/13/17/18/19/20/21/22/23            
	UI_STATE_SetTempPatient_MODE, 				//设定患者温度模式 1/7/11/12/15/16/17/18/19/20/21/22/23  
	UI_STATE_SetTempChamber_MODE, 				//设定出气口温度模式 1/8/11/12/15/16/17/18/19/20/21/22/23
	UI_STATE_SetInExp_MODE, 							//设定InExp模式 1/9/11/12/13/14/15/16/17/18/19/20/21/22/23
	UI_STATE_SetTime_MODE, 								//设定RTC时间模式 1/10/11/12/13/14/15/16/17/18/19/20/21/22/23
	UI_STATE_DATAREADER_MODE,             //数据传输界面   
}WORK_STATUS;

extern WORK_STATUS Work_State;//工作状态定义

void ManageTask(void);//0
void KeyScanTask(void);//1
void HmiServiceModeTask(void);//2
void HmiPostTask(void);//3
void HmiFactoryDefaultSelTask(void);//4
void HmiNon_InvasiveSelTask(void);//5
void HmiRunningTask(void);//6
void HmiSetTempPatientTask(void);//7
void HmiSetTempChamberTask(void);//8
void HmiSetInExpTask(void);//9
void HmiSetTimeTask(void);//10
void HmiScreenSaverModeTask(void);//11
void GetTemp_HpChamberTask(void);//12
void GetTempHumidity_PatientTask(void);//13
void RefreshTempHumidyTask(void);//14
void RefreshRunTimeTask(void);//15
void RefreshRTCTimeTask(void);//16
void HeaterWireControlTask(void);//17
void LowPowerModeTask(void);//18
void HeaterWireModeDetTask(void);//19
void AlarmErrorTask(void);//20
void LowTempDet1STask(void);//21
void SaveDateToFlashTask(void);//22
void UartRecTask(void);//23


//extern KEY_STATUS KeyToFuncVal;



#endif