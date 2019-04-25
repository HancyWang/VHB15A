#include "all.h"

////2019.03.30
////sbit P20 = (uint8_t)0xA0^(uint8_t)0;
//sbit P20 = (uint8_t)0xA0;
//sbit P21 = (uint8_t)0xA0^(uint8_t)1;
//sbit P22 = (uint8_t)0xA0^(uint8_t)2;
//sbit P23 = (uint8_t)0xA0^(uint8_t)3;

WORK_STATUS Work_State = UI_STATE_POWER_OFF_MODE;//工作状态定义

void ManageTask(void)//Task0:Task Manager ----20mS
{
    static INT Manage_Tik_100mS_Cnt = 0;
	  static INT Manage_Tik_250mS_Cnt = 0;
	  static INT Manage_Tik_1000mS_Cnt = 0;		     
             
    WS_TaskState = ManageTIFG;
    if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)   
    { 
        WS_DelTaskIfg(WS_TaskState); 
        { 				
					//20mS任务
					WS_SetTaskIfg(KeyScanTIFG);//按键扫描任务 20mS					
					WS_SetTaskIfg(UartRecTIFG);//串口接收进程
					
					if(Work_State != UI_STATE_DATAREADER_MODE)//数据传输界面屏蔽以下任务
					{					
						if(Work_State != UI_STATE_POWER_OFF_MODE)
						{					
							WS_SetTaskIfg(GetTemp_HpChamberTIFG);      //读取加热盘盘和出气口的温度 20mS
							WS_SetTaskIfg(GetTempHumidity_PatientTIFG); //读取患者端温度和温度 20mS
						}					
						//100mS任务
						Manage_Tik_100mS_Cnt++;
						if(Manage_Tik_100mS_Cnt >= 5)//
						{
							Manage_Tik_100mS_Cnt = 0;
	//						time_test_lcd_backlight();
							if(Work_State == UI_STATE_SERVICE_MODE)
							{					
								WS_SetTaskIfg(HmiServiceModeTIFG);//服务模式
							}
							else if(Work_State == UI_STATE_POST_MODE)
							{
								WS_SetTaskIfg(HmiPostTIFG);//开机自检
							}					
							else if(Work_State == UI_STATE_FACTORY_DEFAULT_SEL_MODE)
							{
								WS_SetTaskIfg(HmiFactoryDefaultSelTIFG);//开机选择功能
							}
							else if(Work_State == UI_STATE_NON_INVASIVE_MODE)
							{
								WS_SetTaskIfg(HmiNon_InvasiveSelTIFG);//开机有创无创选择
							}
							else if(Work_State == UI_STATE_RUNNING_NORMAL_MODE)
							{
								WS_SetTaskIfg(HmiRunningTIFG);//正常运行状态
							}
							else if(Work_State == UI_STATE_SetTempPatient_MODE)
							{
								WS_SetTaskIfg(HmiSetTempPatientTIFG);//患者端温度设定
							}
							else if(Work_State == UI_STATE_SetTempChamber_MODE)
							{
								WS_SetTaskIfg(HmiSetTempChamberTIFG);//出气口温度设定
							}
							else if(Work_State == UI_STATE_SetInExp_MODE)
							{
								WS_SetTaskIfg(HmiSetInExpTIFG);//InExp设定
							}
							else if(Work_State == UI_STATE_SetTime_MODE)
							{
								WS_SetTaskIfg(HmiSetTimeTIFG);//时间设定
							}
							else
							{
								//do nothing
							}							
							
							if((Work_State != UI_STATE_POWER_OFF_MODE)
								&&(Work_State != UI_STATE_POST_MODE)
								&&(Work_State != UI_STATE_FACTORY_DEFAULT_SEL_MODE)
								&&(Work_State != UI_STATE_NON_INVASIVE_MODE)	
								&&(Work_State != UI_STATE_SERVICE_MODE))//除以上模式外					
							{
								WS_SetTaskIfg(HmiScreenSaverModeTIFG);//屏保模式 
							}

							
							if((Work_State == UI_STATE_RUNNING_NORMAL_MODE)
								||(Work_State == UI_STATE_SetTempPatient_MODE)
								||(Work_State == UI_STATE_SetTempChamber_MODE)
								||(Work_State == UI_STATE_SetInExp_MODE)
								||(Work_State == UI_STATE_SetTime_MODE))
							{	
								if((Work_State != UI_STATE_SetTime_MODE)//设定时间时不刷新实时RTC时间 
									&&(Work_State != UI_STATE_SCREENSAVER_MODE))
								{
									WS_SetTaskIfg(RefreshRTCTimeTIFG);//刷新RTC时间			
								}								
							}
						}
						
						//240mS任务
						Manage_Tik_250mS_Cnt++;
						if(Manage_Tik_250mS_Cnt >= 12)//
						{
							Manage_Tik_250mS_Cnt = 0;
							if((Work_State == UI_STATE_RUNNING_NORMAL_MODE)
								||(Work_State == UI_STATE_SetInExp_MODE)
								||(Work_State == UI_STATE_SetTime_MODE))
							{
								if((Work_State != UI_STATE_SetTempPatient_MODE)
									&&(Work_State != UI_STATE_SetTempChamber_MODE))
								{		
									WS_SetTaskIfg(RefreshTempHumidyTIFG);//刷新实时温度和湿度
								}
							}
							
							if((Work_State != UI_STATE_POWER_OFF_MODE)
								&&(Work_State != UI_STATE_POST_MODE)
								&&(Work_State != UI_STATE_FACTORY_DEFAULT_SEL_MODE)
								&&(Work_State != UI_STATE_NON_INVASIVE_MODE)
								&&(Work_State != UI_STATE_SERVICE_MODE))//除以上模式外									
							{
								WS_SetTaskIfg(AlarmErrorTIFG);//报警和错误事件处理,错误事件刷新 
							}
						}
											
						//1000mS任务
						Manage_Tik_1000mS_Cnt++;
						if(Manage_Tik_1000mS_Cnt >= 50)//
						{
							Manage_Tik_1000mS_Cnt = 0;
							if((Work_State == UI_STATE_RUNNING_NORMAL_MODE)
								||(Work_State == UI_STATE_SCREENSAVER_MODE)
								||(Work_State == UI_STATE_SetTempPatient_MODE)
								||(Work_State == UI_STATE_SetTempChamber_MODE)
								||(Work_State == UI_STATE_SetInExp_MODE)
								||(Work_State == UI_STATE_SetTime_MODE))
							{							
								WS_SetTaskIfg(HeaterWireModeDetTIFG); //加热线状态检测 1S
								WS_SetTaskIfg(HeaterPlateWireControlTIFG);//回路加热线温度的控制 1S(仅运算，驱动在定时中断)
								WS_SetTaskIfg(LowPowerModeTIFG);//低功率模式 1S
								WS_SetTaskIfg(SaveDateToFlashTIFG);//保存数据到FLASH 1S	
								WS_SetTaskIfg(LowTempDet1STIFG);//低温低湿检测1S
								WS_SetTaskIfg(RefreshRunTimeTIFG);//刷新运行时间 1S
							}							
						}					
					} 
				}          
    }
}


void KeyScanTask(void) //1
{
	static KEY_STATUS KeyToFuncVal = KEY_STATE_ReleaseValue; 
	static const struct Key_Multifun_Element Key_UP_Element = 
	{
		KEY_STATE_PressValue,       	//PressValue
		KEY_STATE_ReleaseValue,     	//ReleaseValue
		10,                         	//TimOf_Low_High 按下弹起的最大响应时间
		20,                         	//TimOfSe 初次长按响应时间
		5,                          	//TimOfLg 后续长连续按下状态时间
		KeyUpPressShortAction,      	//void (*)(void)短按键由低到高弹起
		KeyUpPressLongFirstAction,    //按键长键后的响应函数
		KeyUpPressLongRepeatAction,  //连续按下持续,连+连-
	};
	static struct Multifun_Key Key_UP =
	{
		&KeyToFuncVal,		        //按键码值
		&Key_UP_Tik,	//按键时标
		1,			      //处理状态
	};
	static const struct Key_Multifun_Element Key_Down_Element = 
	{
		KEY_STATE_PressValue,       	//PressValue
		KEY_STATE_ReleaseValue,     	//ReleaseValue
		10,                         	//TimOf_Low_High 按下弹起的最大响应时间
		20,                         	//TimOfSe 初次长按响应时间
		5,                          	//TimOfLg 后续长连续按下状态时间
		KeyDownPressShortAction,      	//void (*)(void)短按键由低到高弹起
		KeyDownPressLongFirstAction,    //按键长键后的响应函数
		KeyDownPressLongRepeatAction,  //连续按下持续,连+连-
	};
	static struct Multifun_Key Key_Down =
	{
		&KeyToFuncVal,		        //按键码值
		&Key_Down_Tik,	//按键时标
		1,			      //处理状态
	};
	static const struct Key_Multifun_Element Key_OK_Element = 
	{
		KEY_STATE_PressValue,       	//PressValue
		KEY_STATE_ReleaseValue,     	//ReleaseValue
		10,                         	//TimOf_Low_High 按下弹起的最大响应时间
		20,                         	//TimOfSe 初次长按响应时间
		5,                          	//TimOfLg 后续长连续按下状态时间
		KeyOKPressShortAction,      	//void (*)(void)短按键由低到高弹起
		KeyOKPressLongFirstAction,    //按键长键后的响应函数
		KeyOKPressLongRepeatAction,  //连续按下持续,连+连-
	};
	static struct Multifun_Key Key_OK =
	{
		&KeyToFuncVal,		        //按键码值
		&Key_OK_Tik,	//按键时标
		1,			      //处理状态
	};
	static const struct Key_Multifun_Element Key_Mute_Element = 
	{
		KEY_STATE_PressValue,       	//PressValue
		KEY_STATE_ReleaseValue,     	//ReleaseValue
		10,                         	//TimOf_Low_High 按下弹起的最大响应时间
		20,                         	//TimOfSe 初次长按响应时间
		5,                          	//TimOfLg 后续长连续按下状态时间
		KeyMutePressShortAction,      	//void (*)(void)短按键由低到高弹起
		KeyMutePressLongFirstAction,    //按键长键后的响应函数
		KeyMutePressLongRepeatAction,  //连续按下持续,连+连-
	};
	static struct Multifun_Key Key_Mute =
	{
		&KeyToFuncVal,		        //按键码值
		&Key_Mute_Tik,	//按键时标
		1,			      //处理状态
	};
//	BitStatus bit_status;
	bit bit_status;
	WS_TaskState = KeyScanTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			bit_status = KEY_LEFT_UP_IN;//UP

//			if((INT)bit_status == (INT)RESET)
			if(!bit_status)
			{
				KeyToFuncVal = KEY_STATE_PressValue;     
			}
			else
			{
				KeyToFuncVal = KEY_STATE_ReleaseValue;
			}
			Key_Multifun(Key_UP_Element,&Key_UP);
			
			bit_status = KEY_LEFT_DOWN_IN;//DOWN
			if(!bit_status)
			{
				KeyToFuncVal = KEY_STATE_PressValue;     
			}
			else
			{
				KeyToFuncVal = KEY_STATE_ReleaseValue; 
			}

			Key_Multifun(Key_Down_Element,&Key_Down);
			
			bit_status = KEY_RIGHT_UP_IN;//OK

			if(!bit_status)
			{
				KeyToFuncVal = KEY_STATE_PressValue;     
			}
			else
			{
				KeyToFuncVal = KEY_STATE_ReleaseValue; 
			}
				
			Key_Multifun(Key_OK_Element,&Key_OK);
			
			bit_status = KEY_RIGHT_DOWN_IN;//MUTE

			if(!bit_status)
			{
				KeyToFuncVal = KEY_STATE_PressValue;				
			}
			else
			{
				KeyToFuncVal = KEY_STATE_ReleaseValue; 
			}
				
			Key_Multifun(Key_Mute_Element,&Key_Mute);					
		}
	}
}

void HmiServiceModeTask(void)//2
{
	WS_TaskState = HmiServiceModeTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiServiceModeFunc();
		}
	}
}

void HmiPostTask(void)//3
{
	WS_TaskState = HmiPostTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiPostFunc();
		}
	}
}

void HmiFactoryDefaultSelTask(void)//4
{
	WS_TaskState = HmiFactoryDefaultSelTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiFactoryDefaultFunc();
//			time_test_lcd_backlight();
		}
	}
}

void HmiNon_InvasiveSelTask(void)//5
{
	WS_TaskState = HmiNon_InvasiveSelTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiNon_InvasiveSelFunc();
		}
	}	
}

void HmiRunningTask(void)//6
{
	WS_TaskState = HmiRunningTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiRunningFunc();
		}
	}		
}

void HmiSetTempPatientTask(void)//7
{
	WS_TaskState = HmiSetTempPatientTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiSetTempPatientFunc();
		}
	}	
}

void HmiSetTempChamberTask(void)//8
{
	WS_TaskState = HmiSetTempChamberTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiSetTempChamberFunc();
		}
	}	
}	

void HmiSetInExpTask(void)//9
{
	WS_TaskState = HmiSetInExpTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiSetInExpFunc();
		}
	}
}

void HmiSetTimeTask(void)//10
{
	WS_TaskState = HmiSetTimeTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiSetTimeFunc();
		}
	}
}

void HmiScreenSaverModeTask(void)//11
{
	WS_TaskState = HmiScreenSaverModeTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HmiScreenSaverModeFunc();
		}
	}
}

void GetTemp_HpChamberTask(void)//12
{
	WS_TaskState = GetTemp_HpChamberTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			GetTemp_HpChamberFunc();
		}
	}		
}

void GetTempHumidity_PatientTask(void)//13
{
	WS_TaskState = GetTempHumidity_PatientTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			GetTempHumidity_PatientFunc();
		}
	}		
}

void RefreshTempHumidyTask(void)//14
{
	WS_TaskState = RefreshTempHumidyTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			RefreshTempHumidyFunc(0);
		}
	}		
}

void RefreshRunTimeTask(void)//15
{
	WS_TaskState = RefreshRunTimeTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			RefreshRunTimeFunc(0);
		}
	}			
}

void RefreshRTCTimeTask(void)//16
{
	WS_TaskState = RefreshRTCTimeTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			if((Work_State != UI_STATE_SetTime_MODE)//设定时间时不刷新实时RTC时间 
					&&(Work_State != UI_STATE_SCREENSAVER_MODE))
			{
				RefreshRTCTimeFunc();	
			}	
			
		}
	}		
}

void HeaterWireControlTask(void)//17
{
	WS_TaskState = HeaterPlateWireControlTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HeaterPlateWireControlFunc();
		}
	}
}

void LowPowerModeTask(void)//18
{
	WS_TaskState = LowPowerModeTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			LowPowerModeFunc();
		}
	}
}

void HeaterWireModeDetTask(void)//19
{
	WS_TaskState = HeaterWireModeDetTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			HeaterWireModeDetFunc();
		}
	}
}

void AlarmErrorTask(void)//20
{
	WS_TaskState = AlarmErrorTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			AlarmErrorFunc();
		}
	}
}

void LowTempDet1STask(void)//21
{
	WS_TaskState = LowTempDet1STIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			LowTempDet1SFunc();
		}
	}
}

void SaveDateToFlashTask(void)//22
{
	WS_TaskState = SaveDateToFlashTIFG;
	if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
	{   
		WS_DelTaskIfg(WS_TaskState);
		{
			SaveDateToFlashFunc();
		}
	}
}

void UartRecTask(void)//23
{
    WS_TaskState = UartRecTIFG;
    if(WS_TaskJudge(WS_TaskState)!=(uint8_t)0)
    {   
        WS_DelTaskIfg(WS_TaskState);         
        UART_RecData_Func();   
    }
}
















