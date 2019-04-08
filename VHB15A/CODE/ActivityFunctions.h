#ifndef __ACTIVITYFUNCTIONS__
#define __ACTIVITYFUNCTIONS__   


//开机选择使用上一次用户的设定或默认设定
#define  Load_User_Pre_MODE      0 //上一次用户的设定
#define  Load_Fac_MODE           1 // 出厂默认设定

//有创模式,无创模式定义
#define  Noninvasive_Mode      0 //无创模式
#define  Invasive_Mode         1 //有创模式

#define Const_InvasPatientTemp_Min       350    //Patient side temperature Minimum limit Invasive mode 
#define Const_InvasPatientTemp_Max       400	  //Patient side temperature Maximum limit Invasive mode 
#define Const_NoninvasPatientTemp_Min    300    //Patient side temperature Minimum limit Noninvasive mode 
#define Const_NoninvasPatientTemp_Max    370    //Patient side temperature Maximum limit Noninvasive mode 

#define Const_InvasChamberTemp_Min       340    //
#define Const_InvasChamberTemp_Max       430	   //
#define Const_NoninvasChamberTemp_Min    300
#define Const_NoninvasChamberTemp_Max    320    //Patient side temperature Maximum limit Noninvasive mode 

#define Alarm_Const_Sensor      		0   //传感器错误 
#define Alarm_Const_HiTemp      		1   //高温报警
#define Alarm_Const_LoTemp     			2   //低温报警
#define Alarm_Const_LoHumity   			3   //低湿度报警
#define Alarm_Const_NoWater     		4   //无水报警
#define Alarm_Const_RTD_Sensor      5   //人体端传感器错误
#define Alarm_Const_CQK_Sensor      6   //出气口传感器错误
#define Alarm_Const_JRP_Sensor      7   //加热盘传感器错误

#define HeatingTemperature_MAX      1050   //发热盘最高限制温度

typedef enum
{
	UART_REC_STATE_READY = 0, //准备接收,初始化计数,  1
	UART_REC_STATE_HEADER_DET,//检测命令头 "NEUNIT=1," 共9个字符 
	UART_REC_STATE_DATA_TAIL_DET,//接收命令(最多9个字符)，检测命令尾"\CR"共3个字符 
	UART_REC_STATE_DONE //接收正确完成
}UART_RECSTATE;


typedef enum  //heating wire mode
{	
	Wire_Sel_Double = 0,  //double heating wire
	Wire_Sel_In_Only,	    //INSP Only heating wire
	Wire_Sel_None		    //Whithout heating wire
}WIRE_MODE_SEL;	


//typedef struct 
//{
//	INT Uk; //总的控制量
////	int Uk1;//上次的总控制量
//	INT Sum_error;//误差总量
//	INT Ek;//当前的误差量
//	INT Ek1;//前一次的误差量
//	INT Ek2;//前二次的误差量       
//}CONTROLLER_TEMP;//定义一个温度Controller结构


void 	Fun_Null(void);
void	Init_port(void); //初始化端口状态--------------------------------
void 	Main_Init(void);//初始化
void  RefreshTempHumidyFunc(uint8_t Refresh_En);//
void  RefreshRunTimeFunc(uint8_t Refresh_En);//
void  RefreshRTCTimeFunc(void);
void  GetTemp_HpChamberFunc(void);
void  GetTempHumidity_PatientFunc(void);
void  HeaterPlateWireControlFunc(void);
void  LowPowerModeFunc(void);
void  HeaterWireModeDetFunc(void);
void  SaveDateToFlashFunc(void);
void  AlarmErrorFunc(void);
void  LowTempDet1SFunc(void);
void  UART_RecData_Func(void);
void  Setting_write_to_flash(void);// 将设定的参数写入到STC EEPROM
void  Load_Settings_Before_Choice(void);//在选择方框出现之前，要处理的数据，以同时兼容有无选择记忆界面程序
void  ServiceMode_TempHumidy_Disp(void);
void  Mem_Flash_Recall(void);//读取上次的记忆
void  WireInOut_State_Confirm(void);//正常运行界面,按下确认键确定回路的模式
void  Err_Base_HeaterWire_DISP_Enable(void);//屏保时置相关标志,以便亮屏时显示主机和线的图形
void  HeaterPlateWireDriveFbTask(void);//

uint8_t  Bit_is_one(uint8_t Value,INT bit_num);


extern struct stc_data_flash data_flash;
extern uint8_t  Work_Mode;		 
extern uint8_t defalut_mode;	//预设模式选择
extern uint8_t show_str[23]; 
extern uint8_t  In_Exp_Ratio;	 //In和Exp加热方式的比例  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
extern uint8_t Micro_Temp_Val;   //最大为201;最小为0,1表示10ms加热时间
extern uint16_t Micro_Temp_In;
extern uint16_t Micro_Temp_Out;
		 
//实时参数
extern INT    		RT_Temp;            //人体端温度
extern INT    		JEP_Temp;           //加热盘温度
extern LONG    	CQK_Temp;           //出气口温度
		 
extern INT  Set_RT_Temp;         //人体端温度设定值
extern uint16_t  Set_RT_WCTemp;       //人体无创温度
extern uint16_t  Set_RT_YCTemp;       //人体有创温度
extern uint16_t  Set_CQK_Temp;        //出气口温度
extern uint16_t  Set_CQK_WCTemp;      //人体无创温度
extern uint16_t  Set_CQK_YCTemp;      //人体有创温度
extern uint8_t RT_Temp_Reach_Set_Cnt;//人体温度达到设定温度-1.5
extern uint8_t CQK_Temp_Reach_Set_Cnt;//出气口温度达到30
extern uint8_t In_Exp_Ratio;	 //In和Exp加热方式的比例  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
extern uint16_t   Diplay_RTtemp;//显示温度
extern uint8_t WireIn_State;      //加热线In状态 1-有 0-无
extern uint8_t WireOut_State;     //加热线Out状态	  1-有 0-无
extern uint8_t HeaterPlate_State; //加热盘状态 1-有 0-无
extern uint16_t  HP_CNT_Int;				//加热盘中断检测计数

extern uint8_t  ERR_Kind;
extern uint8_t Wire_Mode_Mismatch; //回路发热丝模式和实际不匹配		

extern uint16_t  	HP_CNT_Int_End_Rem;//8S结束时加热盘中断检测计数

extern uint8_t    SaveData[16];
extern uint8_t     Test_Mode_Dis_Jrp_Ctl; //从测试模式进入正常模式即显示加热盘控制的数据	
extern uint8_t AlarmSoundPauseStatus;//报警声音暂停 0-正常报警 1-报警声音暂停(仅报警时有效)
extern uint8_t AlarmInfoIndex;//
extern uint8_t No_HeatSensor_Times;//加热盘温度传感器无响应次数
extern uint16_t      Working_Normal;//正常工作读数，每秒加1，故障重置，改变设置也重置

#define DISPLAY_TEST_DATA_EN_DIS_DEF 0//显示数据
		 
 
#endif