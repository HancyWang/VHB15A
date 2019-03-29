#ifndef __GUI__
#define __GUI__

		#define Lan_English 				0 //英语
		#define Lan_Portuguese 			1 //葡萄牙语
		#define Lan_Spanish  			  2 //西班牙语
		#define Lan_French   			  3 //法语
		
		
typedef enum
{
  DISPLAY_Temperature_Patient = 0,
	DISPLAY_Temperature_Chamber,
}DISPLAY_Temp_Kind;

extern DISPLAY_Temp_Kind Display_Temp_Kind;//显示温度定义



void	HmiEnterToWorkStateFunc(WORK_STATUS State);//进入某个工作模式界面
void  HmiServiceModeFunc(void);//ServiceMode界面
void	HmiPostFunc(void);//POST界面 
void  HmiPostFunc_Tik_Cnt(void);
void	HmiFactoryDefaultFunc(void);//开机选择界面
void  HmiFactoryDefault_Tik_Cnt(void);
void	HmiLanguageFunc(void);//语言选择界面
void	HmiNon_InvasiveSelFunc(void);//有创无创选择界面
void  HmiNon_InvasiveSel_Tik_Cnt(void);
void	HmiRunningFunc(void);//有创无创选择界面
void	HmiSetTempPatientFunc(void);//患者端温度设置界面
void	HmiSetTempChamberFunc(void);//出气口端温度设置界面
void	HmiSetInExpFunc(void);//InExp设置界面
void	HmiSetTimeFunc(void);//时间设定
void	HmiScreenSaverModeFunc(void);//屏保模式
void  HmiScreenSaverMode_Tik_Cnt(void);
//void  HmiScreenSaverModeFunc_TimeRestFunc(void);//注意进运行模式时要清一次
void  DisPlayTime(unsigned char TempTime[7],unsigned char TimeKind);//显示时间




#endif