/* VHB15A湿化器*/	

#include "all.h"

//2019.03.29
sfr P4M0 = 0xB4; 
sfr P4M1 = 0xB3;
sfr P4SW = 0xBB;
sfr WDT_CONTR = 0xC1;
sbit EA       = 0xA8^7;
sbit EX1      = 0xA8^2;
sbit ET0      = 0xA8^1;
sfr P1M0 = 0x92; 
sfr P1M1 = 0x91;
sbit P10 = 0x90^0;
sbit P12 = 0x90^2;
sbit P13 = 0x90^3;
sbit P17 = 0x90^7;
sbit P20 = 0xA0^0;
sbit P21 = 0xA0^1;
sbit P22 = 0xA0^2;
sbit P27 = 0xA0^7;
sfr P2M0 = 0x96;
sfr P2M1 = 0x95;
sbit P32 = 0xB0^2;
sbit P34 = 0xB0^4;
sbit P35 = 0xB0^5;
sbit P36 = 0xB0^6;
sbit P37 = 0xB0^7;
sfr P3M0 = 0xB2;
sfr P3M1 = 0xB1; 
sbit P40 = 0xC0^0;
sbit P43 = 0xC0^3;
sbit TI  = 0x98^1;
sbit RI  = 0x98^0;
sbit TR0 = 0x88^4;
sbit IT1 = 0x88^2;

static enum	Wire_Mode_Def  //heating wire mode
{	
	Wire_Sel_Double = 0,  //double heating wire
	Wire_Sel_In_Only,	    //INSP Only heating wire
	Wire_Sel_None,		    //Whithout heating wire
}Wire_Mode_Sel;	
		
//错误检测和报警相关变量	
uint8_t  ERR_Kind=0;//错误的种类//0传感器/1高温/2低温/3低湿/4无水/5患者传感器/6出气口传感器/7加热盘传感器   
bit Wire_Mode_Mismatch ; //回路发热丝模式和实际不匹配		
static bit JRP_Sensor_Err;
static bit CQK_Sensor_Err;
static bit RTD_Sensor_Err;
static bit SHIDU_Sensor_Err;		
		
uint8_t AlarmSoundPauseStatus = 0;//报警声音暂停 0-正常报警 1-报警声音暂停(仅报警时有效)
uint8_t AlarmInfoIndex = 0;//
          
static uint8_t  xdata  HiTemp_Count=0;	  //高温度次数
static uint8_t  xdata  LoTemp_Count=0; 	  //低温度次数
static uint8_t  xdata  LoTemp_CQK_Count=0;//出气口低温度次数
static uint8_t  xdata  LoHumity_Count=0;  //低湿度次数

uint8_t No_HeatSensor_Times=0;//加热盘温度传感器无响应次数
static uint16_t  No_CQKSensor_Times=0;//出气口温度传感器无响应次数
static uint16_t  CQKSensor_Hithen800_Times=0;//超过80度
static uint16_t  JRP_Sensor_Temp_No_Change_Times=0;// 加热盘温度完全不变的次数2014-07-03
static uint16_t  CQK_Sensor_Temp_No_Change_Times=0;// 出气口温度完全不变的次数		
static uint8_t ERR_RT_Temp_Times=0;//患者温度超过80度
static uint8_t No_ReadData_SHIDU_Times=0;//患者端温度无响应次数
static uint16_t  RT_Temp_Not_Updated_10mS = 0;//连续未采样到数据的时间
static uint8_t No_ReadData_Temp_Times=0;//出气口温度无响应次数
static uint16_t  Nochange_Times=0;//患者端温湿度完全不变的次数

uint8_t RT_Temp_Reach_Set_Cnt = 0;//人体温度达到设定温度-1.5
uint8_t CQK_Temp_Reach_Set_Cnt = 0;//出气口温度达到30

uint8_t show_str[16]; 

static uint8_t xdata  Work_Day=0;
static uint8_t xdata  Work_Hour=0;
static uint8_t xdata  Work_Min=0;
static uint8_t xdata  Work_Sec=0;

//存储相关
struct stc_data_flash data_flash;//
static uint16_t  Run_Count;
BYTE  xdata  SaveData[16];
																		
//串口相关变量																		
static bit Buff_Full_flag;		
static uint8_t xdata  Rec_Count;
static BYTE  xdata  Rec_Buff[21];          //接收到的指令READ0001[CR]-READ0200[CR];
                                    //一条指令可接收256条数据																		
static uint16_t   Tick_20ms=0;
uint8_t defalut_mode = Load_User_Pre_MODE;	//预设模式选择

//实时参数
int  xdata  					RT_Temp=0;            //人体端温度
int  xdata  					JEP_Temp=280;           //加热盘温度
long  xdata  					CQK_Temp=280;           //出气口温度
static uint16_t  xdata  	RT_SHIDU=0;             //人体端湿度
static uint16_t  xdata  	CONTROL_RT_SHIDU=550;     //控制加热的湿度
uint16_t    xdata Diplay_RTtemp = 0;

//设定参数		
uint8_t  				Work_Mode = Invasive_Mode; //默认1为有创模式 ，0为无创模式，2，简易模式(无发热丝)				
int  									Set_RT_Temp=390;         //人体端温度设定值				
uint16_t  				Set_RT_WCTemp=340;       //人体无创温度
uint16_t  				Set_RT_YCTemp=390;       //人体有创温度
uint16_t  				Set_CQK_Temp=370;        //出气口温度
uint16_t  				Set_CQK_WCTemp=310;      //人体无创温度
uint16_t  				Set_CQK_YCTemp=370;      //人体有创温度
uint8_t 			  In_Exp_Ratio=4;	 //In和Exp加热方式的比例  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5

//加热盘驱动和检测
uint16_t  				HP_CNT_Int = 0;//加热盘中断检测计数
uint16_t  				HP_CNT_Int_End_Rem = 0;//8S结束时加热盘中断检测计数
uint8_t 				WireIn_State=0;      //加热线In状态 1-有 0-无
uint8_t 				WireOut_State=0;     //加热线Out状态	  1-有 0-无
uint8_t 				HeaterPlate_State = 0; //加热盘状态 1-有 0-无

uint8_t 				Micro_Temp_Val=0;   //加热盘最大为201;最小为0,1表示10ms加热时间
uint16_t 					Micro_Temp_In=0;    //进气支
uint16_t 					Micro_Temp_Out=0;   //出气支

//控制参数
static uint16_t   Aim_SHIDU=900;  //目标湿度 
static int  xdata  JEP_Temp_Aim=400;           //加热盘目标温度
static uint16_t Wire_Warm_Up_Sec;    //当<=300时限制发热丝最大加热功率
static uint16_t Plate_Warm_Up_Sec;   //当<=600时限制发热盘最大加热功率
static uint16_t Humidity_No_Change_Sec;//湿度不改变的计数，当大于600时可能探头失效，湿度控制失效
static uint16_t Low_Power_Mode_Flag = 0;//低功率模式使能标志

uint8_t  xdata   Test_Mode_Dis_Jrp_Ctl=0; //从测试模式进入正常模式即显示所有工作数据	
static uint8_t  xdata   Micro_Adj_Mode_Test;  //加热的模式测试
uint16_t xdata     Working_Normal=0;//正常工作读数，每秒加1，故障重置，改变设置也重置

static struct PID
{
	int Uk; //总的控制量
	int Uk1;//上次的总控制量
	int Sum_error;//误差总量
	int Ek;//当前的误差量
	int Ek1;//前一次的误差量
	int Ek2;//前二次的误差量       
}PID_temp,PID_temp2,PID_temp3;//定义一个温度PID结构


 //秒    分    时    日    月  星期    年
static BYTE   NowTime[7]= {0x00, 0x00, 0xB2, 0x01, 0x01, 0x05, 0x12};

//uint8_t Alarm_time_det_10mS_cnt = 0;

//================================================== 

static void Store_Data(void);           //存放数据
static void Get_RunCount(void);         //存放数据
static void Init_Erase_Flash_Data(void);//初始化时擦除外部FLASH数据
static void Init_port(void); //初始化端口状态，显示版本号，读取数据，屏幕测试，W25X清数据
static void Main_Init(void);	//得到存放数据的条数,画工作窗口,使能涓流充电,RTC检测,其它变量初始化
static void Write_Default_Setting_to_flash(void); //把默认出厂设定写入FLASH
static void LCD_Show_Verion(void);//显示版本号
static void PatientTemp_NoneHeatWire_Adj(void);//无回路发热丝时的患者端温度调节
static void PID_Calc(void); //PID算法
static void HeatPlateTemp_Aim_PID_Calc(void); //加热盘目标温度PID算法---
static void HeatPlate_PID_Calc(void);//加热盘的PID算法---



static void  Interrupt_EXT1()  interrupt 2  //外部中断EXT1
{
	HP_CNT_Int++;	
}

static void  Interrupt_Time0()  interrupt 1  using  2  //10ms中断一次 //22.1184M
{	  
	 Timer_T0_INT_Main();
	 Tick_20ms++;
	 
}

static void  Serial (void) interrupt 4 using 1		//串口中断
{
    if(RI)
    {
			EUSART_Receive_ISR();

      RI = 0;
    }
		
		if(TI)
		{			
			EUSART_Transmit_ISR();
			TI = 0;
		}
}



static enum
{
    UART_REC_STATE_READY = 0, //准备接收,初始化计数,  1
    UART_REC_STATE_HEADER_DET,//检测命令头 "NEUNIT=1," 共9个字符 
    UART_REC_STATE_DATA_TAIL_DET,//接收命令(最多9个字符)，检测命令尾"\CR"共3个字符 
    UART_REC_STATE_DONE, //接收正确完成
}UART_RecState = UART_REC_STATE_READY;


 static uint8_t   UART_Rec_Buf[20];//接收缓冲区 
 
static void UART_RecBuf_Clear_Fun(void)  //清空缓冲区
 {
   uint8_t i;
   
   for(i=0;i<=19;i++)
    {
       UART_Rec_Buf[i] = 0;        
    } 
 }

//接收数据
static uint8_t   DataToPc[8];
void UART_RecData_Func(void)
{
	  static uint8_t   UART_Rec_Cnt = 0;//接收数据数量 
//	  static uint8_t UART_Len = 0;//接收的数据长度
    uint8_t   i;
    uint8_t   uart_data;	  
	  unsigned long   Adress;
//	  uint16_t    sum;
	
		while(EUSART_DataReady)//缓冲区有数据
    {
			uart_data = EUSART_Read(); //从缓冲区读一个数据
			
			if(Work_State == UI_STATE_DATAREADER_MODE)
			{
					UART_Rec_Cnt++;//数量增加
					if(UART_RecState == UART_REC_STATE_READY)//开始
					{
						UART_RecState = UART_REC_STATE_HEADER_DET;           
						UART_RecBuf_Clear_Fun();//清空缓冲区
						UART_Rec_Buf[0] = uart_data;//初始接收
						UART_Rec_Cnt = 1; //数量
						//EUSART_Write('A');
					}
					else if(UART_RecState == UART_REC_STATE_HEADER_DET)//检测接收头
					{//注意:最终8个字节存放的顺序是先收到的在最低位，例:收到的是AXXLOGET，存放的是TEGOLXXA
						UART_Rec_Buf[UART_Rec_Cnt-1] = uart_data;//从低字节往高字节存放
						if(UART_Rec_Cnt>=3)//接收到3个数据 RD 13
						{
							if((UART_Rec_Buf[0] == 'R')//头标志判断
								&&(UART_Rec_Buf[1] == 'D'))
							{
	//							EUSART_Write_Str("HEAR_OK",7);
								UART_RecState = UART_REC_STATE_DATA_TAIL_DET;//标志OK，接收数据
								UART_RecBuf_Clear_Fun();//清空缓冲区  
								UART_Rec_Cnt = 0; //数量
							}
							else
							{
								UART_Rec_Cnt--;//往低位移
								for(i=0;i<=18;i++) //数据往低位移一位
								{
									 UART_Rec_Buf[i] = UART_Rec_Buf[i+1];        
								}//注意:最终8个字节存放的顺序是先收到的在最低位，例:收到的是AXXLOGET，存放的是TEGOLXXA               
							}
						} 
					}
					else if(UART_RecState == UART_REC_STATE_DATA_TAIL_DET)//检测数据和结尾
					{
						UART_Rec_Buf[UART_Rec_Cnt-1] = uart_data;//从低字节往高字节存放

						if(UART_Rec_Cnt>=2)//最多存放15个数据
						{	
							UART_RecState = UART_REC_STATE_DONE;//接收完成 				
						}
					}
					
					if(UART_RecState == UART_REC_STATE_DONE)//检测到尾标志
					{
						UART_RecState = UART_REC_STATE_READY; //准备下一次检测
						
						Adress=UART_Rec_Buf[0];
						Adress=(Adress<<8)+UART_Rec_Buf[1]-1;
						Adress=(Adress<<4)+0x2000;
						SPI_Read_nBytes(Adress,16,&SaveData[0]);
						EUSART_Write_Str(&SaveData[0],16);//发送16个字节给PC
					}						
			 }
		}
}

void HeaterWireModeDetFunc(void)
{
			if(Wire_Mode_Sel == Wire_Sel_Double)//双支加热回路但实际未检测到IN或OUT	 			 	 
		{
			if((WireIn_State == 0)||(WireOut_State == 0))	
			{
				Wire_Mode_Mismatch = 1;//回路发热丝模式和实际不匹配标志
			}
			else
			{
				Wire_Mode_Mismatch = 0;//回路发热丝模式和实际不匹配标志
			}
		}	
	  else if(Wire_Mode_Sel == Wire_Sel_In_Only)	//单支加热回路但实际未检测到IN或检测到有OUT  	 
		{
			if((WireIn_State == 0)||(WireOut_State > 0))	
			{
				Wire_Mode_Mismatch = 1;//回路发热丝模式和实际不匹配标志
			}
			else
			{
				Wire_Mode_Mismatch = 0;//回路发热丝模式和实际不匹配标志
			}
		}
	  else if(Wire_Mode_Sel == Wire_Sel_None)	//无加热回路但实际检测到有IN或OUT
	  {
	  	if((WireIn_State > 0)||(WireOut_State > 0))	
			{
				Wire_Mode_Mismatch = 1;//回路发热丝模式和实际不匹配标志
			}
			else
			{
				Wire_Mode_Mismatch = 0;//回路发热丝模式和实际不匹配标志
			}
	  }	
}

void RefreshTempHumidyFunc(uint8_t Refresh_En)//刷新显示温度和湿度
{
	static uint8_t  xdata   Refresh_WenDu_Cnt = 0; 
	static uint8_t  xdata   Alarm_WenDu_Cnt = 0; 

	uint16_t Disp,color;
	uint16_t Temp_Dis_En = 0;//温度显示标志
	bit Disp_Err;

	Refresh_WenDu_Cnt++;
	if(Refresh_WenDu_Cnt > 5)//1.2S
	{
		Refresh_WenDu_Cnt = 0;	
	}	
	
	Alarm_WenDu_Cnt++;
	if(Alarm_WenDu_Cnt > 1)
		Alarm_WenDu_Cnt = 0;
	
		Back_Color=WHITE18;	
		Disp_Err=0;

	  Temp_Dis_En = 0;//温度显示标志
		if(Refresh_En)Temp_Dis_En = 1;//强制刷新一次	
	
	  color=BLACK18;
	  if((Bit_is_one(ERR_Kind,Alarm_Const_HiTemp))   //患者端高温红色
			&&(Display_Temp_Kind==DISPLAY_Temperature_Patient))
	  {
			 Temp_Dis_En = 1;//温度显示标志
			 color=RED18; 
		 	  if(Alarm_WenDu_Cnt == 1)
			  {
			  	color=WHITE18;
			  }
	  }
	  else if(((Bit_is_one(ERR_Kind,Alarm_Const_LoTemp))&&(Display_Temp_Kind==0))  //患者端低温红色
					 ||((LoTemp_CQK_Count>120)&&(Display_Temp_Kind==1)))  //出气口低温红色
	  {
			 Temp_Dis_En = 1;//温度显示标志
			 color=RED18;
			 if(Alarm_WenDu_Cnt == 1)
			 color=WHITE18;
	  }
	  else
	  {
			if(Refresh_WenDu_Cnt == 0)Temp_Dis_En = 1;//温度显示标志
	  }
		
		if(Temp_Dis_En == 1)
		{			
			if(Display_Temp_Kind==DISPLAY_Temperature_Chamber)//出气口温度
			{
			  Disp=CQK_Temp;
			  Disp_Err=CQK_Sensor_Err; 

	    }
			else if(Display_Temp_Kind==DISPLAY_Temperature_Patient) //人体端温度
	    {
	  	  Disp=Diplay_RTtemp;
	  	  Disp_Err=RTD_Sensor_Err;
	    }
			
			if(Disp_Err)
		  {
		     Draw_Rectangle_Real(POS_RT_TEMP_X+4,POS_RT_TEMP_Y,POS_RT_TEMP_X+56,POS_RT_TEMP_Y+103,WHITE18);     
		     Draw_Rectangle_Real(POS_RT_TEMP_X,POS_RT_TEMP_Y,POS_RT_TEMP_X+3,POS_RT_TEMP_Y+29,RED18);
		     Draw_Rectangle_Real(POS_RT_TEMP_X,POS_RT_TEMP_Y+34,POS_RT_TEMP_X+3,POS_RT_TEMP_Y+63,RED18);
		     Draw_Rectangle_Real(POS_RT_TEMP_X,POS_RT_TEMP_Y+73,POS_RT_TEMP_X+3,POS_RT_TEMP_Y+102,RED18);
		     //Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,RED18); //画点	
		  }
			else  if(Disp>=1000)//显示温度
		  {
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34,(Disp%1000)/100,color);   
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+73,(Disp%100)/10,color);    
		    Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,WHITE18); //去掉小数点
		  }
			else
		  {
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y,(Disp/100)%10,color);  
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34,Disp%100/10,color);   
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34*2+5,Disp%10,color);    
		    Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,color); //画点
		  }
	   }		 
		 
	 //显示湿度
		color=BLACK18;
		if(Bit_is_one(ERR_Kind,Alarm_Const_LoHumity))   //低湿度报警
		{
			Temp_Dis_En = 1;//温度显示标志
			color=RED18;
			if(Alarm_WenDu_Cnt == 1)
			{
				color=WHITE18;
			}
		}
		else
		{
			if(Refresh_WenDu_Cnt == 0)Temp_Dis_En = 1;//温度显示标志				
		}
			
		if(Temp_Dis_En == 1)
		{
			if(RTD_Sensor_Err)
			{		 	
				 Draw_Rectangle_Real(POS_RT_RH_X+4 , POS_RT_RH_Y,POS_RT_RH_X+39,POS_RT_RH_Y+35,WHITE18);   
				 Draw_Rectangle_Real(POS_RT_RH_X , POS_RT_RH_Y,POS_RT_RH_X+3,POS_RT_RH_Y+16,RED18);     
				 Draw_Rectangle_Real(POS_RT_RH_X , POS_RT_RH_Y+19,POS_RT_RH_X+3,POS_RT_RH_Y+35,RED18);       
			}
		  else
			{	
				 DISP_RH_17X40(POS_RT_RH_X,POS_RT_RH_Y,RT_SHIDU%1000/100,color);
				 DISP_RH_17X40(POS_RT_RH_X,POS_RT_RH_Y+19,RT_SHIDU%100/10,color);
			}
		}	
}

//---------刷新运行时间
void RefreshRunTimeFunc(uint8_t Refresh_En)
//Refresh_En 1-强制刷新一次，否则每分钟刷新一次
{ 
	uint8_t t;
	static uint8_t Last_Sec = 60;		
	
	RX8010_GetTime(NowTime);	
	t = (NowTime[0]&0xF)%10;
	
	if(t != Last_Sec)//时间秒有变化
	{
		 Last_Sec = t;
		 if(Work_Sec<59)
		 {
				Work_Sec++;
		 }
		 else
		 {
			 Work_Sec = 0;
			 t = 0xff;//刷新标志		
			 if(Work_Min<59)
			 {
					Work_Min++;
			 }else
			 {
					Work_Min=0;
					if(Work_Hour<23)
					{
						 Work_Hour++;
					}else
					{
						 Work_Hour=0;
						 if(Work_Day<99) Work_Day++;
					}
			 }
		 }
	}
	
	if((Refresh_En)||(t == 0xff))
	{
		Refresh_En = 0;		
		if(Work_State != UI_STATE_SCREENSAVER_MODE)
		{
			Back_Color=WHITE18;
			DISP_TIME_10X16(POS_ICO_TIME_X-2,POS_DMH_Y-5,Work_Day/10,BLACK18);
			DISP_TIME_10X16(POS_ICO_TIME_X-2,POS_DMH_Y+6,Work_Day%10,BLACK18);
			DISP_TIME_10X16(POS_ICO_TIME_X-2,POS_DMH_Y+22,Work_Hour/10,BLACK18);
			DISP_TIME_10X16(POS_ICO_TIME_X-2,POS_DMH_Y+33,Work_Hour%10,BLACK18);
			DISP_TIME_10X16(POS_ICO_TIME_X-2,POS_DMH_Y+47,Work_Min/10,BLACK18);
			DISP_TIME_10X16(POS_ICO_TIME_X-2,POS_DMH_Y+58,Work_Min%10,BLACK18);  
		}
	}
}

//---------刷新RTC时钟
void RefreshRTCTimeFunc(void)
{
	static uint8_t Last_Min;
	uint8_t color;
	uint8_t t;
	static uint8_t Last_Sec = 60;

	
//时间转换=================================================
	RX8010_GetTime(NowTime);	
	t = (NowTime[0]&0xF)%10;
	if(t != Last_Sec)//时间秒有变化
	{
		Last_Sec = t;
				 
		if(Last_Min!=NowTime[1])
		{
		 Last_Min=NowTime[1];
		}
		color=BLACK18;		
		DisPlayTime(NowTime,2); //显示时间
		DisPlayTime(NowTime,3); //显示时间
		DisPlayTime(NowTime,4); //显示时间
		DisPlayTime(NowTime,5); //显示时间
		DisPlayTime(NowTime,6); //显示时间
		DisPlayTime(NowTime,7); //显示时间
	}
}

void GetTempHumidity_PatientFunc(void)
{
	static uint8_t Get_time_out = 0;//超时，实测64mS，最长给到100mS
	static BYTE   xdata    Read_Order=0;	
	uint8_t Read_SHT21_Err;

	bit Read_SCL_CONF;
	unsigned long TempLong;
	uint16_t  Temp2;
	
	static uint16_t RT_Temp_Mem_L1 = 0;
	static uint16_t RT_Temp_Mem_L2 = 0;
	static uint16_t RT_Temp_Mem_L3 = 0;
	static uint8_t ReCnt;
	
	ReCnt++;
	if(ReCnt > 50)//1S
	{
		ReCnt = 0;	
		RT_Temp_Mem_L3 = RT_Temp_Mem_L2;
		RT_Temp_Mem_L2 = RT_Temp_Mem_L1;
		RT_Temp_Mem_L1 = RT_Temp;	
		Diplay_RTtemp = (RT_Temp_Mem_L3+RT_Temp_Mem_L2+RT_Temp_Mem_L1) / 3;	
	}
	
	//static int Ix=0;

	//Ix++;
	//LCD_ShowxNum(223,30,16,5,Ix,0x80,BLUE18);//		

//	LCD_ShowxNum(223,10,16,3,RT_Temp_Not_Updated_10mS ,0x80,BLUE18);//	
//	LCD_ShowxNum(223,70,16,3,No_ReadData_Temp_Times,0x80,BLUE18);//		
//	LCD_ShowxNum(223,100,16,3,No_ReadData_SHIDU_Times,0x80,BLUE18);//	
//	LCD_ShowxNum(223,130,16,3,ERR_RT_Temp_Times,0x80,BLUE18);//	
	
//	EA = 0;
	if(RT_Temp_Not_Updated_10mS < 4000)RT_Temp_Not_Updated_10mS++;

	{
	  if(Read_Order==0)		//发温度指令	  	
		{
		 	Read_SHT21_Err = SHT21_WriteTemp(); 
			if(Read_SHT21_Err)
			{
				Read_Order = 0;//接收错误
			}
			else
			{				
				Read_Order = 1;
				Get_time_out = 0;
			}
		}
		else   if(Read_Order == 1)//温度数据接收
	  {
			//SCL_INPUT_HIGH(); // set SCL I/O port as input
			Read_SCL_CONF=SCL_CONF;
			if(Read_SCL_CONF==1)
		  {
				Get_time_out = 0;
				TempLong =	SHT21_ReadData();
				//RT_Temp_Test = TempLong;
					//LCD_ShowxNum(223,160,16,5,TempLong,0x80,BLUE18);//	
					
			  if((TempLong == SHT21_ERROR) || (TempLong & 0x8000)) //读取出错，后面为湿度标志位
				{
					//if(No_ReadData_Temp_Times<250) No_ReadData_Temp_Times++;	
					Read_Order = 0;		
				}
				else
				{					
					No_ReadData_Temp_Times = 0;
					Read_Order = 2;	
										
					if(Work_State != UI_STATE_SERVICE_MODE) //注意,测试模式下显示真实温度
					{

							if(WireIn_State!=0)    //有加热丝线进行温度修正
								TempLong=TempLong*101/100-4*TempLong/388;
						 else
								TempLong=TempLong-4*TempLong/400;	 

							if(TempLong>=350)   //温度修正
								TempLong=TempLong+6*(TempLong-350)/45;
							
							
					   if(TempLong>800) //如果人体端达到80说明有异常，出错报警 //2014-07-07 
					   {
					   	  if(ERR_RT_Temp_Times<100) {ERR_RT_Temp_Times++;}
					   }else 
					   {
					   	  ERR_RT_Temp_Times=0;
								RT_Temp=TempLong;
							  RT_Temp_Not_Updated_10mS = 0;
							 
					   }						 
					}
					else
					{
						RT_Temp=TempLong;
						RT_Temp_Not_Updated_10mS = 0;
					}
				} 
			}
			else
			{
				//CONTROL_RT_SHIDU = 0;				
			}			
			//LCD_ShowxNum(223,150,16,3,RT_Temp,0x80,BLUE18);//	

	  }
		else if(Read_Order==2)  //发湿度指令
		{										 
		  Read_SHT21_Err = SHT21_WriteRH();
			if(Read_SHT21_Err)
			{
				Read_Order = 2;//接收错误
			}
			else
			{				
				Read_Order = 3;
				Get_time_out = 0;
			}
		} 	  
		else  if(Read_Order == 3)//湿度数据接收
		{
			//SCL_INPUT_HIGH(); // set SCL I/O port as input
		Read_SCL_CONF=SCL_CONF;
		if(Read_SCL_CONF==1)
		{ 
				//Read_Order = 0;
				Get_time_out = 0;
				TempLong =	SHT21_ReadData();
				//LCD_ShowxNum(223,210,16,5,TempLong,0x80,BLUE18);//	
			 if((TempLong == SHT21_ERROR)||((TempLong & 0x8000) ==0)) //读取出错
			{
				if(No_ReadData_SHIDU_Times<250) No_ReadData_SHIDU_Times++;
				CONTROL_RT_SHIDU = 0;
				Read_Order = 2;	
				
			}
			else
			{
				Read_Order = 0;	
				TempLong &= 0x7fff;//最高位是1才是湿度
				No_ReadData_SHIDU_Times = 0;
				if(TempLong > 999)TempLong = 999;  
				RT_SHIDU=TempLong;
				CONTROL_RT_SHIDU=TempLong;  //控制湿度
				//75%以上进行校准
				if(Work_State != UI_STATE_SERVICE_MODE) //注意,测试模式下显示真实湿度
				{
						if(RT_Temp>300)
						{							  	 
							 if(TempLong>550)
							 {						 
									Temp2=(TempLong-550)*80/100; //16-05-06
									if(Temp2>249) Temp2=249;		
							 }
							 else Temp2 = 0;
							 RT_SHIDU=TempLong + Temp2;  //2014-07-25
						}
						else
						{									
							 RT_SHIDU=TempLong;	 //2014-07-18
							 //RT_SHIDU=TempLong+120;  		
						}	
						
						if(RT_Temp>350)
						{	
							 RT_SHIDU=RT_SHIDU+180*(RT_Temp-350)/100;  //显示湿度 16-05-06
						}
				}
				else
				{
					
				}	
				RT_SHIDU += 5;	//四舍五入				  
				if(RT_SHIDU>999) RT_SHIDU=999;//;	 
			}
		}
		else
		{
				
		}
		//LCD_ShowxNum(223,130,16,3,RT_SHIDU,0x80,BLUE18);//
		

	}	
	
	 if((Read_SHT21_Err)||(RT_SHIDU == 0))
	 {	
		 if(No_ReadData_SHIDU_Times<100) No_ReadData_SHIDU_Times++;
		 if(No_ReadData_Temp_Times<100) No_ReadData_Temp_Times++;
		 
	 }else 	
	 {
				No_ReadData_SHIDU_Times=0;
		   No_ReadData_Temp_Times=0;
	 } 

		Get_time_out++;
		if(Get_time_out > 100)//超过100mS
		{
			Get_time_out = 0;
			if(Read_Order == 1)Read_Order = 0;
			else if(Read_Order == 3)Read_Order = 2;
			else Read_Order = 0;
		}				 
	}
}


void GetTemp_HpChamberFunc(void)
{
	bit HeaterSensorExist;
	bit CQKSensorExist;
	
//	static uint16_t CQK_Temp_Not_Updated_40mS = 0;//连续未采样到数据的时间S
//  static uint16_t JRP_Temp_Not_Updated_S = 0;//连续未采样到数据的时间S

	uint8_t T18B20_L;
	uint8_t T18B20_H;
	uint8_t CQK_T18B20_L;
	uint8_t CQK_T18B20_H;	
	
	uint8_t T18B20_H_GET;
	uint8_t T18B20_L_GET;
	uint8_t CQK_T18B20_H_GET;
	uint8_t CQK_T18B20_L_GET;
	

	static uint16_t CQK_Temp_Last;
	uint8_t CRC8_error;   //checksum
	uint8_t checksum;   //checksum
	uint8_t DS18B20_Cdata[8];    //data array for checksum verification	

	uint16_t temp;
	static uint16_t JRP_Temp_Last;
	
	static uint8_t Get_Temp_Cnt;
	
	Get_Temp_Cnt++;
	if(Get_Temp_Cnt > 3)Get_Temp_Cnt = 0;
	
	if(No_CQKSensor_Times<2000)No_CQKSensor_Times++;
	if(No_HeatSensor_Times<200)No_HeatSensor_Times++;
	
	if(Get_Temp_Cnt == 0)  //
	 {
	    HeatingPlateSensor_Port=0;                                //主机拉至低电平
	    ChamberOutletSensor_Port=0;
	    delay_us(609);                                //609
	    HeatingPlateSensor_Port=1;                                //释放总线等电阻拉高总线,并保持15~60us
	    ChamberOutletSensor_Port=1;
	    delay_us(86);                           //延时70us                     //延时70us
	    HeaterSensorExist=HeatingPlateSensor_Port;		            //接收应答信号
	    CQKSensorExist=ChamberOutletSensor_Port;
	    delay_us(488);                                //403
	    DS18B20_WriteByte(0xCC);                      //跳过ROM命令
	    DS18B20_WriteByte(0x44);                      //开始转换命令
	 }
   else if(Get_Temp_Cnt == 2) //
	 //进行温度转换
	 {
	    HeatingPlateSensor_Port=0;                                //主机拉至低电平
	    ChamberOutletSensor_Port=0;
	    delay_us(609);                                //503us
	    HeatingPlateSensor_Port=1;                                //释放总线等电阻拉高总线,并保持15~60us
	    ChamberOutletSensor_Port=1;
	    delay_us(86);                           //延时70us
	    HeaterSensorExist=HeatingPlateSensor_Port;		            //接收应答信号
	    CQKSensorExist=ChamberOutletSensor_Port;
	    delay_us(488);
	    DS18B20_WriteByte(0xCC);                      //跳过ROM命令
	    DS18B20_WriteByte(0xBE);                      //读暂存存储器命令
	    delay_us(10);
	    DS18B20_ReadByte();                           //读温度低字节
	    T18B20_L=Read_18B20_Value;
	    CQK_T18B20_L=Read_18B20_Value2;
	    DS18B20_ReadByte();                           //读温度高字节
	    T18B20_H=Read_18B20_Value;
	    CQK_T18B20_H=Read_18B20_Value2;
	
		  if((CQKSensorExist==0))//错误则不进行校验
			{
				DS18B20_Cdata[0] = CQK_T18B20_L;
				DS18B20_Cdata[1] = CQK_T18B20_H;
				DS18B20_ReadByte();  
				DS18B20_Cdata[2] = Read_18B20_Value2;
				DS18B20_ReadByte();  
				DS18B20_Cdata[3] = Read_18B20_Value2;
				DS18B20_ReadByte();  
				DS18B20_Cdata[4] = Read_18B20_Value2;
				DS18B20_ReadByte();  
				DS18B20_Cdata[5] = Read_18B20_Value2;
				DS18B20_ReadByte();  
				DS18B20_Cdata[6] = Read_18B20_Value2;
				DS18B20_ReadByte();  
				DS18B20_Cdata[7] = Read_18B20_Value2;
				DS18B20_ReadByte();  
				checksum = Read_18B20_Value2;			
				

				//Cdata[7] = 0x11; //test CRC

				CRC8_error = DS18B20_CheckCrc(DS18B20_Cdata,8,checksum); 
				
			 if(CRC8_error == CHECKSUM_ERROR)
			 {
				 CQKSensorExist = 1;
			 }					 
			 
			 if((CQKSensorExist==0))//
			 {
						CQK_T18B20_H_GET = CQK_T18B20_H;
						CQK_T18B20_L_GET = CQK_T18B20_L;

					 CQK_T18B20_H_GET = (CQK_T18B20_H_GET << 4) | (CQK_T18B20_L_GET >> 4);			//加热盘温度值合并高低位存入整数位
				 if(CQK_T18B20_H_GET&0x80)//是负数
				 {
								CQK_T18B20_L_GET =(((~CQK_T18B20_L_GET)&0x0F)*10)/16;
								temp=(~CQK_T18B20_H_GET)*10+CQK_T18B20_L_GET;
				 } else
				 {
							CQK_T18B20_L_GET = ((CQK_T18B20_L_GET&0x0F)*10)/16;
							temp=CQK_T18B20_H_GET*10+CQK_T18B20_L_GET;
				 }
		
				 if((CQK_Temp_Last ==  temp) && (temp == 850))  //错误时一直显示85度 2014-07-18
				 {
						CQK_Sensor_Temp_No_Change_Times++;//2014-07-03 出气口温度传感器温度相同的次数
						if(CQK_Sensor_Temp_No_Change_Times > 10000)CQK_Sensor_Temp_No_Change_Times =10000;
				 }
				 else
				 {
						CQK_Sensor_Temp_No_Change_Times = 0;		
				 }
				 
				 if(temp > 800)
				 {
//					 if(CQKSensor_Hithen800_Times < 2000) CQKSensor_Hithen800_Times++;						
				 }
				 else 
					 CQKSensor_Hithen800_Times = 0;
					 
			 
					 if(((CQK_Temp_Last >=  temp) && (CQK_Temp_Last - temp < 200))
				 ||((CQK_Temp_Last <=  temp) && (temp - CQK_Temp_Last  < 200)))//EFT对策：温度不能瞬变,1S变化不能超过20度
				 {	
						if((temp > 0)&&(temp < 850))//不读零
						{
							CQK_Temp = temp;//实时显示温度
							if(Work_State != UI_STATE_SERVICE_MODE) //注意,测试模式下显示真实温度
							 {
									CQK_Temp=CQK_Temp*51/50;	
							 }
					  }
				 }
					else
					{
					}	
					CQK_Temp_Last =  temp;	
				}
			}
		 
		 	if(CQKSensorExist==1)// || CQK_Temp > 800)
			{
//				if(No_CQKSensor_Times<2000)No_CQKSensor_Times++;	  	 	  
			}else
	  	 	  No_CQKSensor_Times=0;  
		 

		  T18B20_H_GET = T18B20_H;
	  	T18B20_L_GET = T18B20_L;

		if(Work_State == UI_STATE_SERVICE_MODE)//在测试模式时显示读取的数据
		{
//			LCD_ShowxNum(10,220,24,3,T18B20_H_GET,0x80,BLACK18); //8表示高位为0也显示,0表示非叠加显示 加热盘
//			LCD_ShowxNum(10,268,24,3,T18B20_L_GET,0x80,BLACK18); //8表示高位为0也显示,0表示非叠加显示
		}		 


		T18B20_H_GET = (T18B20_H_GET << 4) | (T18B20_L_GET >> 4);			//加热盘温度值合并高低位存入整数位
		if(T18B20_H_GET&0x80)//是负数
		{
					T18B20_L_GET =(((~T18B20_L_GET)&0x0F)*10)/16;
					temp=(~T18B20_H_GET)*10+T18B20_L_GET;
		} 
		else
		{
				T18B20_L_GET = ((T18B20_L_GET&0x0F)*10)/16;
				temp=T18B20_H_GET*10+T18B20_L_GET;
		}
		  
			 
		 if((JRP_Temp_Last ==  temp))// && (temp == 850))  //错误时一直显示85度 2014-07-18
		 {
		 	JRP_Sensor_Temp_No_Change_Times++;//2014-07-03 加热盘温度传感器温度相同的次数
			if(JRP_Sensor_Temp_No_Change_Times > 6000)JRP_Sensor_Temp_No_Change_Times = 6000;
		 }
		 else
		 {
		 	JRP_Sensor_Temp_No_Change_Times = 0;
		 }
		 JRP_Temp_Last =  temp;		

		  JEP_Temp =  temp;	  //实时显示温度

	if(HeaterSensorExist==1 || JEP_Temp>=0x0FFF || JEP_Temp > 1250)
	  {
//	  	 if(No_HeatSensor_Times<2000)No_HeatSensor_Times++; 
	  }else
	     No_HeatSensor_Times=0;
	}	
} 

static int  Temp1_Int = 260;
static int  Temp2_Int = 400;
static uint16_t NoneWire_Heat_Sec=0;
static int  Set_CQK_Temp_Comp;//出气口校正过后的目标温度
void  HeaterPlateWireControlFunc(void)//温度控制
{
//    uint8_t  Temp3;
	  uint16_t  Val_Calc;

		int  Humidity_Err; //未达到目标湿度的差值
		int  Humidity_Comp;//超过目标温度的差值 
	  static uint16_t Last_Rem_Humidity;	
	
	   static uint16_t WarmUp_S_Cnt;	
	   //uint16_t  CQK_Vaul_Err;//差值

	  if((Work_State != UI_STATE_SCREENSAVER_MODE)&&(Test_Mode_Dis_Jrp_Ctl == 1))
	  {	//8表示高位为0也显示,0表示非叠加显示 出气口
			Back_Color=WHITE18;
			LCD_ShowxNum(POS_RT_RH_X-50,5,16,2,Micro_Adj_Mode_Test,0x80,BLACK18); //发热盘加热的模式显示
			LCD_ShowxNum(POS_RT_RH_X-50,34,16,4,JEP_Temp,0x80,BLACK18); //发热盘的温度
			LCD_ShowxNum(POS_RT_RH_X-50,73,16,3,CQK_Temp,0x80,BLACK18); //出气口的温度
			LCD_ShowxNum(POS_RT_RH_X-50,107,16,3,Set_CQK_Temp,0x80,BLACK18); //出气口设定温度
			LCD_ShowxNum(POS_RT_RH_X-50,141,16,3,Micro_Temp_Val,0x80,BLACK18); //发热盘的加热时长
			
			LCD_ShowxNum(POS_RT_RH_X-50,185,16,3,CONTROL_RT_SHIDU,0x80,BLACK18); //真实相对湿度
			LCD_ShowxNum(POS_RT_RH_X-50,219,16,3,Aim_SHIDU,0x80,BLACK18); //目标湿度
			LCD_ShowxNum(POS_RT_RH_X-50,253,16,3,Micro_Temp_In,0x80,BLACK18); //In加热时长
			LCD_ShowxNum(POS_RT_RH_X-50,287,16,3,Micro_Temp_Out,0x80,BLACK18); //Exp加热时长
			
			LCD_ShowxNum(POS_RT_RH_X-33,2,16,1,Wire_Mode_Sel,0x80,BLACK18); //加热丝模式			
			LCD_ShowxNum(POS_RT_RH_X-33,10,16,1,WireIn_State,0x80,BLACK18); //IN加热丝状态
			LCD_ShowxNum(POS_RT_RH_X-33,18,16,1,WireOut_State,0x80,BLACK18); //EXP加热丝状态	
			
			//LCD_ShowxNum(POS_RT_RH_X-20,185,16,1,SHT21_Heater_State,0x80,BLACK18); //SHT加热状态
			LCD_ShowxNum(POS_RT_RH_X-33,253,16,3,Wire_Warm_Up_Sec,0x80,BLACK18); //600S计时	
			LCD_ShowxNum(POS_RT_RH_X-33,107,16,3,Set_CQK_Temp_Comp,0x80,BLACK18); //校正过后的出气口目标湿度
			//LCD_ShowxNum(POS_RT_RH_X-33,141,16,3,Plate_Warm_Up_Sec,0x80,BLACK18); //600S计时	
			LCD_ShowxNum(POS_RT_RH_X-33,185,16,3,Humidity_No_Change_Sec,0x80,BLACK18); //600S计时			
		
			if(WireIn_State!=0)	//有发热丝
			{
					LCD_ShowxNum(POS_RT_RH_X-33,34,16,4,JEP_Temp_Aim,0x80,BLACK18); //发热盘的目标温度
					LCD_ShowxNum(POS_RT_RH_X-16,34,16,3,WarmUp_S_Cnt,0x80,BLACK18); //无发热丝回路时改变时间	
				
					if(PID_temp3.Sum_error >0)
						LCD_ShowxNum(POS_RT_RH_X+41,20,16,1,1,0x80,BLACK18); //
					else
						LCD_ShowxNum(POS_RT_RH_X+41,20,16,1,0,0x80,BLACK18); //
					
					LCD_ShowxNum(POS_RT_RH_X+41,34,16,4,abs(PID_temp3.Sum_error),0x80,BLACK18); //
					
					
					if(PID_temp2.Sum_error >0)
						LCD_ShowxNum(POS_RT_RH_X+41,80,16,1,1,0x80,BLACK18); //
					else
						LCD_ShowxNum(POS_RT_RH_X+41,80,16,1,0,0x80,BLACK18); //
					
					LCD_ShowxNum(POS_RT_RH_X+41,94,16,4,abs(PID_temp2.Sum_error),0x80,BLACK18); //

			}
			else
			{
					LCD_ShowxNum(POS_RT_RH_X-33,34,16,4,Temp2_Int,0x80,BLACK18); //无发热丝回路时发热盘的目标温度
					LCD_ShowxNum(POS_RT_RH_X-16,34,16,3,NoneWire_Heat_Sec,0x80,BLACK18); //无发热丝回路时改变时间	
			}
			
			LCD_ShowxNum(POS_RT_RH_X+40,170,16,3,Temp1_Int,0x80,BLACK18); //开机渐进式温度目标
			LCD_ShowxNum(POS_RT_RH_X+20,170,16,3,RT_Temp,0x80,BLACK18); //患者端温度
			LCD_ShowxNum(POS_RT_RH_X+20,210,16,4,Nochange_Times,0x80,BLACK18); //温湿度连续不变计数		

			LCD_ShowxNum(2,2,16,5,Working_Normal,0x80,BLACK18); //正常工作时间计时	
			LCD_ShowxNum(2,60,16,5,HP_CNT_Int_End_Rem,0x80,BLACK18); //显示每4秒加热盘方波的次数			
	  }

	  if(		     
				(ERR_Kind & 0x01)!=0//传感器错误				
				||(ERR_Kind & 0x02)!=0 //高温
		    ||(ERR_Kind & 0x10)!=0 //无水		
	      || Wire_Mode_Mismatch == 1  //发热丝未选
				|| HeaterPlate_State==0)//水罐未装好
	  {
	  	  Micro_Temp_In=0;
	  	  Micro_Temp_Out=0;
	      Micro_Temp_Val=0;  //停止加热
				Micro_Adj_Mode_Test = 1;//加热模式测试
				Wire_Warm_Up_Sec = 0; //当<=300时限制最大加热功率	 
				//Plate_Warm_Up_Sec = 0;
				Humidity_No_Change_Sec = 0;			
	  }
		else
		{				
				//管内发热丝工作,控制温度调节========================== 			
			
			 if(Work_Mode==Noninvasive_Mode) //无创模式时,人体端温度对目标湿度的控制逻辑
			 {	  	 	
					Aim_SHIDU=790;  
			 }else	  //有创模式时,人体端温度对目标湿度的影响
			 {
					Aim_SHIDU=890; //2014-11-21	
			 }

			Val_Calc = 0;				
			//if(Boot_Start_RT_Temp_Heating_Timer_Sec > 300) //患者端开机渐进式加热结束
			if(Low_Power_Mode_Flag == 0)//非低功率模式才可以控制
			{//回路发热丝的控制
				if(WireIn_State!=0)		//当有管内IN发热丝时，管内发热丝的加热逻辑
				{
			
					if(Working_Normal >= 1200)
					{
						Temp1_Int = Set_RT_Temp;//20分钟以后为设定温度
					}
					else
					{						
						Temp1_Int = CQK_Temp + 30;//20分钟以前跟随出气口温度
					}
					if(Temp1_Int > Set_RT_Temp)Temp1_Int = Set_RT_Temp;
					
					
					//PID_temp.Ek = Set_RT_Temp - RT_Temp;//当前的误差量 设定温度-当前实际温度
					PID_temp.Ek = Temp1_Int - RT_Temp;//当前的误差量 设定温度-当前实际温度
					PID_Calc(); 
					Val_Calc = (uint8_t)PID_temp.Uk;	//计算出加热时间					
					
						if(WireIn_State!=0)
						{
							 if (RT_Temp<Set_RT_Temp-20)
							 {
									
							 }
							 else if (RT_Temp<Set_RT_Temp-10)
							 {
			
							 }
							 else if (RT_Temp<=Set_RT_Temp)
							 {
		
							 }
								else if (RT_Temp<=Set_RT_Temp+10)
							 {

							 }
								else
									 Val_Calc=0;	
						}
						else
						{
									 Val_Calc=0;
						}
				} 
				else	  //当无管内发热丝时
				{
							 Val_Calc=0;
				}
				
				if(Working_Normal >= 1200)//正常工作20分钟之后
				{

				}
				else
				{
					if(Val_Calc > 160)Val_Calc = 160;//20分钟内限制加热功率
				}
				
				
				Micro_Temp_In = Val_Calc;

				Val_Calc=0;
				if(WireOut_State!=0)	  //当有管内OUT发热丝时，管内发热丝的加热逻辑
				{
					 {
						 Val_Calc = Micro_Temp_In *(10+In_Exp_Ratio-1)/10;
						 if(Val_Calc > 195)Val_Calc = 195;
						 if(In_Exp_Ratio > 1)
						 {	
								if(Val_Calc==0)
								{
									Val_Calc=40*(10+In_Exp_Ratio-1)/10;
								}

						 }
						 if (RT_Temp >= Set_RT_Temp+10)//大于1度,停止加热,保护措施
						 {
							 Val_Calc=0; 
						 }
					}	         
				}
				else
				{	
					Val_Calc=0;	
				}
				Micro_Temp_Out = Val_Calc;
			}

			//有无创模式时发热盘的加热逻辑
			{
				//不区分加热线/////////////////////////////	  
					if(CONTROL_RT_SHIDU<=Aim_SHIDU)//主要是针对大流量时的补偿
					{
							Humidity_Err = (Aim_SHIDU - CONTROL_RT_SHIDU);///2;//湿度误差计算
							if(Humidity_Err > 100)Humidity_Err = 100;//最大限定为100
					}		
					else Humidity_Err = 0;	
					
					Humidity_Comp =  CONTROL_RT_SHIDU - Aim_SHIDU;	//控制湿度以减少冷凝水		
					if(Humidity_Comp > 100)Humidity_Comp  = 100;
					
					if(Last_Rem_Humidity != CONTROL_RT_SHIDU)//真实湿度改变则清零
					{
							Humidity_No_Change_Sec = 0;
					}
					Last_Rem_Humidity = CONTROL_RT_SHIDU;
					
					if(Humidity_No_Change_Sec >= 900)//可能进入冷凝状态
					{
							Humidity_No_Change_Sec = 901;
							Humidity_Err = 0;	 //进入冷凝状态则湿度控制无效
							Humidity_Comp = 0;
					}				
								
				if((CQK_Temp>=660) ||(JEP_Temp>=HeatingTemperature_MAX))	   //出气口温度最高66度
				{
					 Micro_Temp_Val=0;//大于65度必须停止加热
					 Micro_Adj_Mode_Test = 2;//加热模式测试       	 
				}				
				//带加热线的温度调节===========================
				else if(WireIn_State!=0)
				{
					 if(Low_Power_Mode_Flag == 0)//非低功率模式才可以控制
					 {
							if(JEP_Temp>=HeatingTemperature_MAX)	 //105度
							{
								Val_Calc = 0;
								Micro_Adj_Mode_Test = 7;//加热模式测试            	
							}
							else
							{	
								Set_CQK_Temp_Comp = Set_CQK_Temp; //出气口设定温度
								
								if(Work_Mode==Invasive_Mode)//
								{	
										if(Humidity_Err >= 50)  //当目标湿度相差5%以上且为无创时，允许最高超过度
										{
											//Set_CQK_Temp_Comp += Humidity_Err/6;//最高允许超过1.6度
											Set_CQK_Temp_Comp += Humidity_Err/10;//最高允许超过1度
										}	
								}	
								else
								{
										if(Humidity_Err >= 50)  //当目标湿度相差5%以上且为无创时，允许最高超过度
										{
											Set_CQK_Temp_Comp += Humidity_Err/2;//最高允许超过5度 120L 28环境温度
										}
								}	

								if(Humidity_Comp >= 10)
								{
									//Set_CQK_Temp_Comp = Set_CQK_Temp_Comp - Humidity_Comp/5;//湿度每上1%则出气口温度下降0.2度，最多2度
									Set_CQK_Temp_Comp = Set_CQK_Temp_Comp - Humidity_Comp/10;//湿度每上1%则出气口温度下降0.1度，最多1度
								}		

								if(Work_Mode==Invasive_Mode)//
								{	
										//Set_CQK_Temp_Comp -= 5;//有创时降低0.5度	
										if(Set_CQK_Temp_Comp < 320)Set_CQK_Temp_Comp = 320;//最极限的温度
								}
								else
								{	
										if(Set_CQK_Temp_Comp < 300)Set_CQK_Temp_Comp = 300;//最极限的温度
								}	

								////if(CQK_Temp < Set_CQK_Temp_Comp - 30) //	

								//算法开始
								if(JEP_Temp < (JEP_Temp_Aim - 20))//加热盘温度小于加热盘目标温度-2		
								{
									WarmUp_S_Cnt = 0;//清零，等待达到加热目标
								}	
								else
								{
									WarmUp_S_Cnt++;
									if(WarmUp_S_Cnt >= 20)//每两分钟调整一次加热盘目标温度
									{
										WarmUp_S_Cnt = 0;
										PID_temp3.Ek = Set_CQK_Temp_Comp - CQK_Temp;//当前的误差量 设定温度-当前实际温度
										HeatPlateTemp_Aim_PID_Calc();												

										JEP_Temp_Aim = JEP_Temp_Aim + PID_temp3.Uk;//												
										
									  if(Work_Mode==Invasive_Mode)//
										{
											if(JEP_Temp_Aim < 500)JEP_Temp_Aim = 500;//最小50度
										}
										else
										{
											if(JEP_Temp_Aim < 400)JEP_Temp_Aim = 400;//最小50度
										}	
										
										if(Working_Normal >= 1200)
//										if(Work_Min>=20||Work_Day>0 ||Work_Hour>0)//开机20分钟之后
										{
											if(JEP_Temp_Aim > 1045)JEP_Temp_Aim = 1045;//最大102度
										}
										else
										{
											if(JEP_Temp_Aim > 980)JEP_Temp_Aim = 980;//开机20分钟以内最大98度
										}
									}
								}
								
								
								PID_temp2.Ek = JEP_Temp_Aim - JEP_Temp;//当前的误差量 设定温度-当前实际温度
								HeatPlate_PID_Calc(); 
								Val_Calc = (uint8_t)PID_temp2.Uk;	//计算出加热时间

								if((CQK_Temp - Set_CQK_Temp_Comp) > 20)//出气口温度比目标温度高
								{
									if(Val_Calc > 20)Val_Calc = 20;	
									JEP_Temp_Aim = JEP_Temp;//更改加热盘目标温度
								}	
								else if((CQK_Temp - Set_CQK_Temp_Comp) > 10)//出气口温度比目标温度高
								{

								}
						 }
					 }					 
					
					
					if(Val_Calc > 195)Val_Calc = 195;					
					if(Low_Power_Mode_Flag == 0)//非低功率模式才可以控制
							Micro_Temp_Val = Val_Calc;//便于计算，以免超过190进入中断		
				}
				else if(Low_Power_Mode_Flag == 0)//非低功率模式才可以控制
				{	
						PatientTemp_NoneHeatWire_Adj();
						//不带加热线的温度调节==============================================================
				 }  
			}		
		}
} 


void   Store_Data(void)   //存放数据地址从1开始
{
	  uint16_t i;
	  uint8_t N,j;
	  if (Run_Count<1) return;
	  i=(Run_Count-1)/8+1;     //计算地址
	  N=Run_Count%8;       //计算位
	  if (Run_Count!=0 && N==0) N=8;
	  j=(0xFF<<N);
	  SPI_Write_nBytes(i,1,&j);
	 
        //delay_us(100);
}

void  Get_RunCount(void)   //得到存放数据的条数
{
	  uint16_t Start1,Start2,StartAdr;
	  uint8_t N,K,j;
	  Start1=0x1;
	  Start2=0x2000;
	  for(j=0;j<14;j++) //查找计数位
	  {
	  	  StartAdr=(Start1+Start2)/2;
	  	  SPI_Read_nBytes(StartAdr,1,&N);
	  	  delay_us(100);
	  	  if(N==0xFF)
	  	  {
	  	  	 Start2=StartAdr;
	  	  }else
	  	  {
	  	  	 Start1=StartAdr;
	  	  	 Start2=Start1+(2<<(12-j));
	  	  	 SPI_Read_nBytes(StartAdr+1,1,&K);
	  	  	 if(K==0xFF)
	  	  	 {	 break;  }
	  	  	 else
	  	  	 {
	  	  	 	  StartAdr=StartAdr+1;
	  	  	 	  N=K;
	  	  	 }
	  	  }
	 }
	 Run_Count=(StartAdr-1)*8;//计算时1个字节表8
	 for(j=0;j<8;j++)
	 {
	 	 if(((0x01<<j) & N)!=0) break;
	 }
	 Run_Count=Run_Count+j;      //最多可记数44896条,256条为缓冲

	 //Run_Count = 2000;//测试使用 2014-12-26
	 //SPI_Read_nBytes(8000,1,&N);
	 // DS1302_GetTime(NowTime);
	 // if(NowTime[0]&0x80)         //发现掉电则重新初始化时间，是否要询问
	 //    DS1302_SetTime(initTime);    //初始化时间
	 /* if(N==1)                    //时间必须自行设定
	 {
	  	DS1302_SetTime(NowTime);
	  	j=2;
	  	SPI_Write_nBytes(8000,1,&j);
	  	delay_ms(10);
	 }else if(N==2)
	 {

	 }*/

	 
   /* 
   DISP_TIME_10X16(POS_RT_RH_X-20,10,Run_Count/10000,BLACK18);
   DISP_TIME_10X16(POS_RT_RH_X-20,18,Run_Count%10000/1000,BLACK18);
   DISP_TIME_10X16(POS_RT_RH_X-20,26,Run_Count%1000/100,BLACK18);
   DISP_TIME_10X16(POS_RT_RH_X-20,34,Run_Count%100/10,BLACK18);
   DISP_TIME_10X16(POS_RT_RH_X-20,42,Run_Count%10,BLACK18);
   */
   //DISP_TIME_10X16(POS_RT_RH_X-20,POS_RT_RH_Y+150-90,Enable_Save_Data,BLACK18);
}

static uint8_t  Wire_Mode_Sel_Rem = 0xff;//记忆状态
static uint8_t  Disp_ERR_VHB80_Code = 0xff;

void Error_Base_HeaterWire_DISP_Enable(void)//屏保时置相关标志,以便亮屏时显示主机和线的图形
{
	Disp_ERR_VHB80_Code = 0xff;
	Wire_Mode_Sel_Rem = 0xff;//记忆状态
}


void AlarmErrorFunc(void)    //错误事件
{
	uint8_t color;
	static uint8_t  xdata   Err_Event_Cnt;  //减小工作次数
	uint8_t  xdata   Err_Event_1;
	uint8_t  xdata   Err_Event_2;
	  
	static uint8_t AlarmInfoRem = 0;//报警状态记忆
	{
		Err_Event_Cnt++;//闪烁计数及减少程序运行时间
		if(Err_Event_Cnt > 1)
		{	
			Err_Event_Cnt = 0;//红	
		}
		Err_Event_1 = 0;
		Err_Event_2 = 1;			
	
		//LCD_ShowxNum(POS_RT_RH_X-40,5,16,4,RT_Temp,0x80,BLACK18); //8表示高位为0也显示,0表示非叠加显示
		//LCD_ShowxNum(POS_RT_RH_X-40,60,16,4,ERR_RT_Temp_Times,0x80,BLACK18); //
	
		if((No_CQKSensor_Times>1000 || CQK_Sensor_Temp_No_Change_Times > 6000) //出气口温度传感器错误检测
			||(CQKSensor_Hithen800_Times > 250)//连续10S > 80度
			||((No_ReadData_Temp_Times>20))//加速显示
		  )
		{
			 CQK_Sensor_Err=1;
			 CQK_Temp = 0;	
			 LoTemp_CQK_Count	= 0;//传感器错误,不可检测低温报警
			 Working_Normal = 0;//正常工作时间清零	
			 Set_bit(ERR_Kind,Alarm_Const_CQK_Sensor); 				
    }
		else
		{
			 CQK_Sensor_Err=0;
			 Clear_bit(ERR_Kind,Alarm_Const_CQK_Sensor); 				
		}
			
		if(Nochange_Times>900 || No_ReadData_SHIDU_Times>20 || RT_Temp_Not_Updated_10mS > 3000)//人体端湿度传感器错误检测
		{
			 SHIDU_Sensor_Err=1;
			 RTD_Sensor_Err=1;
			 RT_SHIDU = 0; 
			 HiTemp_Count = 0;//传感器错误,不可检测高温报警
			 LoTemp_Count = 0;//传感器错误,不可检测低温报警
			 LoHumity_Count = 0;//传感器错误,不可检测低湿报警
			 Working_Normal = 0;//正常工作时间清零		
			 Set_bit(ERR_Kind,Alarm_Const_RTD_Sensor); 
		}		
		else
		{
				SHIDU_Sensor_Err=0;
				Clear_bit(ERR_Kind,Alarm_Const_RTD_Sensor); 
		}
			
		if(Nochange_Times>900 ||  No_ReadData_Temp_Times>20 
			|| ERR_RT_Temp_Times>20 || RT_Temp_Not_Updated_10mS > 3000)//人体端温度传感器错误检测 2014-07-03
  	{
	  	 RTD_Sensor_Err=1;
			 if(ERR_RT_Temp_Times<10)//高温异常报警时温度不清零
				{
					RT_Temp = 0; 		
					Diplay_RTtemp = 0;
				}
			 HiTemp_Count = 0;//传感器错误,不可检测高温报警
			 LoTemp_Count = 0;//传感器错误,不可检测低温报警
			 LoHumity_Count = 0;//传感器错误,不可检测低湿报警
			 Working_Normal = 0;//正常工作时间清零		
		}
  	else
  		   RTD_Sensor_Err=0;
		
    if( No_HeatSensor_Times>150 || JRP_Sensor_Temp_No_Change_Times > 600)//加热盘传感器错误检测	 
		{			
  		JRP_Sensor_Err=1;
			Set_bit(ERR_Kind,Alarm_Const_JRP_Sensor); 
		}
  	else
		{
  			JRP_Sensor_Err=0;
			  Clear_bit(ERR_Kind,Alarm_Const_JRP_Sensor); 
		}
  	if(HiTemp_Count>2)  //高温错误检测
		{
  	   Set_bit(ERR_Kind,Alarm_Const_HiTemp); 	   
		}
  	else
  		 Clear_bit(ERR_Kind,Alarm_Const_HiTemp);
    if(LoTemp_Count>120)  //低温错误检测
  		 Set_bit(ERR_Kind,Alarm_Const_LoTemp);
  	else
  	 	 Clear_bit(ERR_Kind,Alarm_Const_LoTemp);

    if(LoHumity_Count>120) //低湿度错误检测
    {
    	   Set_bit(ERR_Kind,Alarm_Const_LoHumity);//设置低湿度报警标志
    }
    else
    {
			if (Bit_is_one(ERR_Kind,Alarm_Const_LoHumity))
			{
				Clear_bit(ERR_Kind,Alarm_Const_LoHumity);//清除低湿度报警标志
			}
    }

	//色块报警  VHB15A_FUNCTION_ALARM_COLOR_DISPLAY
	/*人体探头出错   	---红
	 出气口探头出错 	---绿
	 加热盘探头出错 	---蓝
	 高温报警 			---黑
	 低温报警 			---灰
	 低湿度报警 		---黄
	 水罐未安装好报警 	---青
	 发热线未安装或开路	---紫
	 无水/干烧		    ---黑
	*/
		
	if(Work_State != UI_STATE_SCREENSAVER_MODE)//屏保时不显示
	{
		if((SHIDU_Sensor_Err==1)||(RTD_Sensor_Err))	//人体端探头
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+5,RED18);
		}
		if(CQK_Sensor_Err==1)//出气口探头
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+5,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+10,GREEN18);
		}
		if(JRP_Sensor_Err==1)//加热盘探头
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+10,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+15,BLUE18);
		}
		if(HiTemp_Count>2)//高温
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+15,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+20,BLACK18);
		}
		if(LoTemp_Count>120)//低温
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+20,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+25,GRAY18);
		}
		if(LoHumity_Count>120)//低湿度
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+25,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+30,YELLOW18);
		}
		if(HeaterPlate_State==0)//水罐未装好或发热盘开路
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+30,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+35,BRIGHT_BLUE18);
		}
		if(Wire_Mode_Mismatch == 1)//回路发热丝模式和实际不匹配	
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+35,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+40,PURPLE18);
		}
		if(Bit_is_one(ERR_Kind,Alarm_Const_NoWater)) //无水(干烧)报警
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+40,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+45,BLACK18);
		} 
	}


	  if(HeaterPlate_State==0)  //没有放入罐子
    {
    	AlarmInfoIndex = 1;
		}
		else if(Bit_is_one(ERR_Kind,Alarm_Const_NoWater)) //无水错误
    {
    	AlarmInfoIndex = 2;
    }
		else if(CQK_Sensor_Err || RTD_Sensor_Err || SHIDU_Sensor_Err)
    {
			AlarmInfoIndex = 3;
    } 
		else if(Bit_is_one(ERR_Kind,Alarm_Const_HiTemp)) //高温错误
    {
    	AlarmInfoIndex = 4;
    }
		else if(Bit_is_one(ERR_Kind,Alarm_Const_LoTemp)) //患者端持续低温
    {
    	AlarmInfoIndex = 5;
    }
		else if(LoTemp_CQK_Count>120)//出气口持续低温
	  {
    	AlarmInfoIndex = 6;
    }	
		else if(Bit_is_one(ERR_Kind,Alarm_Const_LoHumity))//持续低湿度错误
    {
			AlarmInfoIndex = 7;
    }
		else if(Wire_Mode_Mismatch == 1)//回路发热丝模式和实际不匹配	
    {
      AlarmInfoIndex = 8;
		}	
		else
    {
			AlarmInfoIndex = 0;
			AlarmSoundPauseStatus = 0;//无报警，不可声音暂停
    }
		
		if(AlarmInfoIndex !=  AlarmInfoRem)//报警状态改变
		{
			AlarmSoundPauseStatus = 0;//声音暂停取消
			Sound_Alarm_Pause_Disable();	
		}	
					
		AlarmInfoRem = AlarmInfoIndex;
		
	
  	//传感器错误==============================================================
   ERR_Kind&=0xFE;  ////清除第一位是传感器错误
   if(CQK_Sensor_Err || RTD_Sensor_Err || JRP_Sensor_Err || SHIDU_Sensor_Err)
   {
   	  ERR_Kind|=0x01;//第一位是传感器错误
   } 

   if(Work_State != UI_STATE_SCREENSAVER_MODE)//非屏保模式，否则要先退出屏保模式
	 {	
		 if(HeaterPlate_State==0)    //无加热盘，显示所有的图片   Plate_State==0
		 {
			 Working_Normal = 0;//正常工作时间清零

			 if(Work_State != UI_STATE_SCREENSAVER_MODE)
			 {
				 if(Disp_ERR_VHB80_Code!=1)
				 {
						Disp_ERR_VHB80_Code=1;
						DISP_VHB80_PIC(BLUE18);	

				 if(Wire_Mode_Sel_Rem != Wire_Mode_Sel)
				 {
					 if(Wire_Mode_Sel == Wire_Sel_None)
					 { 
							if(Err_Event_Cnt == Err_Event_1) //无加热
							{ 
									Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,GRAY18);  //画吸气管上部，下部
									Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,GRAY18); 
							}
						} 

						if((Wire_Mode_Sel==Wire_Sel_In_Only || Wire_Mode_Sel== Wire_Sel_None))
						{
								if(Err_Event_Cnt == Err_Event_1)
								{
									Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,GRAY18);  //画出气管上部，下部
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,GRAY18);   //画出气管上部，下部
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,GRAY18);  //画封口 	       	 	 
								}
						}
					}
					Wire_Mode_Sel_Rem = Wire_Mode_Sel;								 
				 }
				 if(Err_Event_Cnt == Err_Event_1)  	 
						 color=RED18;//改颜色
				 else if(Err_Event_Cnt == Err_Event_2)
						 color=GRAY18;
				 
//				 if((Err_Event_Cnt == Err_Event_1)||(Err_Event_Cnt == Err_Event_2))
					DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,color); 
				}
				else
				{
					Disp_ERR_VHB80_Code = 0;				
				}			
		 }
			else  if(Bit_is_one(ERR_Kind,Alarm_Const_NoWater)) //无水(干烧)报警
			// if(0)
		 {
				 //显示无水错误================================
				 if(Disp_ERR_VHB80_Code!=2)
				 {
						Disp_ERR_VHB80_Code=2; 
						DISP_VHB80_PIC(BLUE18);
				 }
				 if(Err_Event_Cnt == Err_Event_1)
						color=RED18;          //无水错误改为红色
				 else if(Err_Event_Cnt == Err_Event_2)
						color=WHITE18;
				 Draw_Rectangle_Real(POS_HEAT_X,POS_HEAT_Y+2,POS_HEAT_X+8,POS_HEAT_Y+33,color);
							
		 }
		 else if(CQK_Sensor_Err || RTD_Sensor_Err || SHIDU_Sensor_Err  ||HiTemp_Count>2 || LoTemp_Count>120 ||LoTemp_CQK_Count>120) //探头错误或高低温报警
	//	 || Heat_ERR
		 //else if(0)
		 {
			 //显示图形，闪烁错误部分==========================================
			 if(Disp_ERR_VHB80_Code!=3)
			 {
					Disp_ERR_VHB80_Code=3; 
					DISP_VHB80_PIC(GRAY18);
			 }	   	

			 if(Err_Event_Cnt == Err_Event_1)
			 {
						color=RED18;  //红
			 }
				else if(Err_Event_Cnt == Err_Event_2)
			 {
						color=GRAY18;	//灰
			 }


			 {

				 if(CQK_Sensor_Err||LoTemp_CQK_Count>120)//显示出气口错误///
				 {
							DISP_CQK25X24(POS_CQK_X,POS_CQK_Y,color);
				 }else
				 {
							DISP_CQK25X24(POS_CQK_X,POS_CQK_Y,GRAY18);
				 } 
				 if(RTD_Sensor_Err||HiTemp_Count>2 || LoTemp_Count>120)  //人体端探头错误
				 { 
							DISP_RTD28X24(POS_RTD_X,POS_RTD_Y,color);
				 }else
				 {
							DISP_RTD28X24(POS_RTD_X,POS_RTD_Y,GRAY18);
				 } 
				 if(JRP_Sensor_Err) //加热盘探头
				 {
							//DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,color);  
							DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,GRAY18);
							//DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,RED18); 
				 }else
				 {
							DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,GRAY18); 
							//DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,RED18); 				 
				 } 
			}

		}
		else if(Wire_Mode_Mismatch == 1)//回路发热丝模式和实际不匹配	
			//else if(0)
		{
					Working_Normal = 0;//正常工作时间清零	

					 {
						 if(Disp_ERR_VHB80_Code!=4)
						 {
								Disp_ERR_VHB80_Code=4; 
								DISP_VHB80_PIC(BLUE18);
						 }
						 
						 //画In加热线段
						if(WireIn_State > 0)//有In回路发热丝，显示蓝色
						{
							color=BLUE18;
						}
						else
						{
							color=GRAY18;//无In回路发热丝，显示灰色
						}
						if(Err_Event_Cnt <= Err_Event_1)//显示当前状态，有或者无，蓝色或灰色
						{//按上面的颜色显示
						}								
						else if(Err_Event_Cnt <= Err_Event_2)//根据模式确定显示红色报警还是当前状态颜色
						{
							if(((WireIn_State == 0)&&(Wire_Mode_Sel == Wire_Sel_None))//无In回路发热丝且是None模式不需要报警
								||((WireIn_State == 1)&&(Wire_Mode_Sel == Wire_Sel_Double))//有In回路发热丝且是双支模式不需要报警
								||((WireIn_State == 1)&&(Wire_Mode_Sel == Wire_Sel_In_Only)))//有In回路发热丝且是单支模式不需要报警
							
							{
							}
							else//除以上情况均需要报警:如无In时单/双支/Un,有In时None/Un
							{
								color=RED18;
							}
						}
						
						Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,color);  //画吸气管上部，下部
						Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,color); 			
				
						
						 //画Exp加热线段
						if(WireOut_State > 0)//有Out回路发热丝，显示蓝色
						{
							color=BLUE18;
						}
						else
						{
							color=GRAY18;//无Out回路发热丝，显示灰色
						}
						if(Err_Event_Cnt <= Err_Event_1)//显示当前状态，有或者无，蓝色或灰色
						{//按上面的颜色显示
						}								
						else if(Err_Event_Cnt <= Err_Event_2)//根据模式确定显示红色报警还是当前状态颜色
						{
							if(((WireOut_State == 0)&&(Wire_Mode_Sel == Wire_Sel_None))//无Out回路发热丝且是None模式不需要报警
								||((WireOut_State == 0)&&(Wire_Mode_Sel == Wire_Sel_In_Only))//无Out回路发热丝且是单支模式不需要报警
								||((WireOut_State == 1)&&(Wire_Mode_Sel == Wire_Sel_Double)))//有In回路发热丝且是双支模式不需要报警							
							
							{
							}
							else//除以上情况均需要报警:如无Out时双支/Un,有Out时单/None/Un
							{
								color=RED18;
							}
						}
						
						Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,color);  //画出气管上部，下部
						Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,color);   //画出气管上部，下部
						Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,color);  //画封口 
					}    		 
		 }
		 else if(JRP_Sensor_Err)//加热盘探头错误
		 {
			 Working_Normal = 0;//正常工作时间清零		
			 //显示图形，闪烁错误部分==========================================
			 if(Disp_ERR_VHB80_Code!=5)
			 {
					Disp_ERR_VHB80_Code=5; 
					DISP_VHB80_PIC(GRAY18);
			 }	
			 if(Err_Event_Cnt == Err_Event_1)
			 {
						//color=RED18;  //红
				 color=BLUE18;	//灰
			 }
			 //else
				 else if(Err_Event_Cnt == Err_Event_2)
			 {
						color=GRAY18;	//灰
			 }
			 DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,color); 	
		 
		 }
		 else if((Wire_Mode_Sel==Wire_Sel_In_Only || Wire_Mode_Sel== Wire_Sel_None))//经过确认后的单支加热或无加热回路	
		{	

			 if(Work_State != UI_STATE_SCREENSAVER_MODE)  //没有屏幕保护时才能显示加热模式
			 {
						 if(Disp_ERR_VHB80_Code!=6)
						 {
								Disp_ERR_VHB80_Code=6; 
								DISP_VHB80_PIC(BLUE18);
						 }

				 //画加热线 
				if(Err_Event_Cnt == Err_Event_1) //		
				{	
				 if(Wire_Mode_Sel_Rem != Wire_Mode_Sel)
				 {				 
					 if(Wire_Mode_Sel == Wire_Sel_None)//无加热丝
					 { 
									Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,GRAY18);  //画吸气管上部，下部
									Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,GRAY18); 
								
									Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,GRAY18);  //画出气管上部，下部
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,GRAY18);   //画出气管上部，下部
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,GRAY18);  //画封口 				
						} 
						else if(Wire_Mode_Sel==Wire_Sel_In_Only)
						{
									Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,BLUE18);  //画吸气管上部，下部
									Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,BLUE18); 
									
									Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,GRAY18);  //画出气管上部，下部
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,GRAY18);   //画出气管上部，下部
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,GRAY18);  //画封口 
						}
					}
					Wire_Mode_Sel_Rem = Wire_Mode_Sel;
				 }	
			 }			
		 }
		 else   //消除所有错误显示
		 {
				 if(Disp_ERR_VHB80_Code!=0)
				 {
						Disp_ERR_VHB80_Code=0;
						DISP_VHB80_PIC(WHITE18);  	  	
				 }     
		 }
	 }
   //DISP_TIME_10X16(POS_RT_RH_X,POS_RT_RH_Y+145,Disp_ERR_VHB80_Code,BLACK18);
 }
}	  

//void Event_1s(void)       //1秒钟事件 内部定时中断置标志
void LowTempDet1SFunc(void)
{
	uint16_t a,b,c;//出气口温度达到设定温度-1
	static uint8_t  xdata   No_Water_Times=0;	
	static uint8_t RT_SHIDU_More_then_90_Cnt = 0;//连续超过90%达到10次(1秒1次)	
	static uint16_t    xdata Last1s_RThumity;
	static uint16_t    xdata Last1s_RTtemp;
	
	if(Working_Normal < 65000)Working_Normal++;
	
	
	if((Work_State != UI_STATE_SCREENSAVER_MODE)&&(Test_Mode_Dis_Jrp_Ctl == 1))
	{
		LCD_ShowxNum(105,180,16,3,RT_Temp_Reach_Set_Cnt,0x80,BLACK18); //
		LCD_ShowxNum(105,215,16,3,LoTemp_Count,0x80,BLACK18); //	
	}


	if(Wire_Warm_Up_Sec < 900)Wire_Warm_Up_Sec++;

	if(Plate_Warm_Up_Sec < 900)Plate_Warm_Up_Sec++;
	Humidity_No_Change_Sec++;//湿度不改变的计数，当大于600时可能探头失效，湿度控制失效
	
  if(HeaterPlate_State)  //
  {	
  	if(Last1s_RThumity==CONTROL_RT_SHIDU && Last1s_RTtemp==RT_Temp)	//人体端温度和湿度同时不改变
  	{
  		  if(Nochange_Times<1000) Nochange_Times++;
    }
		else
    {	
    	   Nochange_Times=0;
    }	
		

		//Last1s_RThumity=RT_SHIDU;
		Last1s_RThumity=CONTROL_RT_SHIDU;
		Last1s_RTtemp=RT_Temp;
  }
	else
  {
  	  Nochange_Times=0;
  }
 
	 //无水报警,应通过加热盘的加热上升速度判断水量======================================
	 //从有水到无水，人体端温度应该会降低,降低的幅度估计在3度以上
	 //有加热丝的无水判断，出气口温度高于人体端温度，加热盘温度在100度以上，人体端湿度低于80%，或者有20%左右的下降
	 //无加热丝的判断，如果水分不够，估计是人体端温度较低，加热盘温度在100度以上
   if(HeaterPlate_State)
   {

   }
	 else
   {   	
   	    Clear_bit(ERR_Kind,Alarm_Const_NoWater);   	
   } 
	 
	 if(RT_SHIDU >= 900) //便于快速检测，只要湿度达到90%持续10S，然后连续2分钟小于60%则报警
	 {		 
		 RT_SHIDU_More_then_90_Cnt++;
		 if(RT_SHIDU_More_then_90_Cnt >= 10)//连续10秒超过90%
		 {
			 RT_SHIDU_More_then_90_Cnt = 10;
		 }
	 }
	 else
	 {
		 if(RT_SHIDU_More_then_90_Cnt < 10)//小于90%且在10次以内则清零,10次以上则不清零
		 {
			 RT_SHIDU_More_then_90_Cnt = 0;
		 }
	 }		 

	 //干烧报警	<60% 
   //if(Work_Min>=20||Work_Day>0 ||Work_Hour>0) //|| RT_SHIDU_More_then_90_Cnt >= 10)
	 if(Working_Normal >= 1200)
   {
		   if(RT_SHIDU<600) //连续2分钟少于60%报警	 认为是干烧，必须重启机器
		   {  		   		
		       if(No_Water_Times<240)  No_Water_Times++;
			   if(RTD_Sensor_Err || SHIDU_Sensor_Err) //传感器错误不可依湿度来检测无水报警
			   {
			   		No_Water_Times = 0;
			   }
		   }else
		   {
		   	    No_Water_Times=0;
		   }
		   if( No_Water_Times>=120)
	     {
	      	Set_bit(ERR_Kind,Alarm_Const_NoWater);
	     } 
   }
	 
	 if(RT_SHIDU>650)//温度大于65%，取消无水报警
	 {
		 No_Water_Times=0;
		 Clear_bit(ERR_Kind,Alarm_Const_NoWater);
	 }

   //============================================================
	 //人体端温度达到识别180S 
	 //if(Work_Mode==Invasive_Mode) //有创模式
	 if(1) //
	 {
		if(RT_Temp >= Set_RT_Temp - 15) //便于快速检测，只要达到设定温度-1.5度10S，然后连续2分钟小于60%则报警
		{		 
			 RT_Temp_Reach_Set_Cnt++;
			 if(RT_Temp_Reach_Set_Cnt >= 180)//连续180秒超过设定-1
			 {
				 RT_Temp_Reach_Set_Cnt = 180;
			 }
		}
		else
		{
			 if(RT_Temp_Reach_Set_Cnt < 180)//小于设定且在10次以内则清零,10次以上则不清零
			 {
				 RT_Temp_Reach_Set_Cnt = 0;
			 }
		}
	}	
		
	 if(Work_Mode==Noninvasive_Mode)//无创模式
	 {
		 a = Const_NoninvasivePatientTemperature_Max+10;//高温报警限值
		 if(WireIn_State!=0)//有加热丝单支或双支
		 {
			b = Const_NoninvasivePatientTemperature_Min-10;//无创低温报警限值 29度
		 }
		 else 
		 {
			 b = 260;//无回路加热丝时无创低温报警为26度
		 }
		 //c = 700;//低湿报警限值 V2.0
		 c = 300;//低湿报警限值 V2.02 无创模式取消低湿度报警
	 }
	 else if(Work_Mode==Invasive_Mode) //有创模式
	 {
		 a = Const_InvasivePatientTemperature_Max+10;
		 if(WireIn_State!=0)//有加热丝单支或双支
		 {
			 b = Const_InvasivePatientTemperature_Min-10;//有创低温报警限值 34度
		 }
		 else
		 {
			 b = 290;//无回路加热丝时有创报警为29度
		 }
		
		 c = 800;//低湿报警限值
	 }

		if(Diplay_RTtemp>a)
		{
				if(HiTemp_Count<240)	HiTemp_Count++;
		}else
		{
				 HiTemp_Count=0;
		} 
		
/*		
		为应对热比焓，增加新的低温报警逻辑，需要同时满足以下条件触发低温报警
			1，有创模式
			2，单支或双支加热回路模式
			3，患者端达到设定温度-1度 连续180S，防止误动作
			4，患者端低于设定温度-2度且回路加热功率为全功率(190)连续10S*/

//人体端低温报警		
		//if(Work_Min>=20||Work_Day>0 ||Work_Hour>0)// || RT_Temp_Reach_Set_Cnt >= 180)	 //人体低温报警
		if(Working_Normal >= 1200)//20分钟
		{
			 if((RT_Temp_Reach_Set_Cnt >= 180)//达到温度连续180S
				//&&(WireIn_State!=0)//有加热丝单支或双支
				&&(Diplay_RTtemp <= Set_RT_Temp - 30))//温度低于设定温度3度	//气源中断或探头脱落		  
			 {
				 if(((WireIn_State!=0) && (Micro_Temp_In >= 190)))//有加热丝单支或双支
					 //||(WireIn_State==0))//无发热丝不一定是全功率加热 无发热丝不使用 
				
				 {
						if(LoTemp_Count<240) LoTemp_Count+= 12;	//当温度达到设定温度后，连续10S下降到设定温度-2.5度，低温报警	
				 }
				 else 
				 {
					 //if(LoTemp_Count<240) LoTemp_Count++;	//当温度达到设定温度后，连续10S下降到设定温度-2.5度，低温报警	
				 }
			 }
			 else if(Diplay_RTtemp<b)//55分钟开始检测温度过低报警
			 {
				 if(Working_Normal >= 3300)
				 {
					 if(LoTemp_Count<240) 
					 {
							LoTemp_Count++;
					 }
				}
			 }
			 else
			 {
				 LoTemp_Count=0;
			 }
		}
		else
		{
			 LoTemp_Count=0;
		}

//人体端低湿报警			
		//if(Work_Min>=20||Work_Day>0 ||Work_Hour>0)// || RT_SHIDU_More_then_90_Cnt >= 10)	 //人体低湿报警
		if(Working_Normal >= 1200)
		{
			 if(RT_SHIDU<c)	  //2014-07-23	  低湿度报警
			 {
					if(LoHumity_Count<250)   LoHumity_Count++;
			 }
			 else
					LoHumity_Count=0;
	
		}else
		{

			 LoHumity_Count=0;
		}	 
	 
//出气口低温报警
	 	//if(CQK_Temp >= Set_CQK_Temp - 10) //便于快速检测，只要达到设定温度10S，然后连续2分钟小于60%则报警
	  if(CQK_Temp >= 300) //便于快速检测，只要达到设定温度10S，然后连续2分钟小于60%则报警
		{		 
			 CQK_Temp_Reach_Set_Cnt++;
			 if(CQK_Temp_Reach_Set_Cnt >= 10)//连续10秒达到
			 {
				 CQK_Temp_Reach_Set_Cnt = 10;
			 }
		}
		else
		{
			 if(CQK_Temp_Reach_Set_Cnt < 10)//10次以内则清零,10次以上则不清零
			 {
				 CQK_Temp_Reach_Set_Cnt = 0;
			 }
		}	

		if(Working_Normal >= 1200)
		{
			 if((CQK_Temp < Const_NoninvasiveChamberTemperature_Min - 10)
				 &&(Work_Mode==Invasive_Mode))//有创模式才有出气口低温报警
			 {
				 if(LoTemp_CQK_Count<240)
				 {
					  if(CQK_Temp_Reach_Set_Cnt>= 10)//若达到过温度则加快速度
						{
							//LoTemp_CQK_Count += 12;
							LoTemp_CQK_Count++;
						}
						else
							LoTemp_CQK_Count++;
				 }
			 }else
			 {
				 LoTemp_CQK_Count=0;
			 }	    
		}else
		{
			 LoTemp_CQK_Count=0;
		}
}

void SaveDateToFlashFunc(void)
{
	 static uint8_t   xdata  Enable_Save_Data=0;  //保存数据计数
	 uint16_t a,b,c;//
	 unsigned long Adress;	
   if(Enable_Save_Data>=1)
   {
     	Enable_Save_Data++;
     	if(Enable_Save_Data>=10) Enable_Save_Data=0;

   }
   if(Enable_Save_Data==2)
   {
     if((Run_Count-1)%256==0)//一个扇区为4K，每次存储的数据为16字节，即256组数据即为4K 256*16=4096;
     {
   	   //清除下面256的数据
   	    Adress=Run_Count-1;	 //假设为257-1=256
   	    Adress=(Adress<<4);	 //	 256*16= 4096
   	    Adress=Adress+0x2000;//起始地址为0X2000，即8192为保留字节
   	    SPI_Erase_Sector(Adress);
     }
   }else  if(Enable_Save_Data==3)//存放数据
   {
		Adress=Run_Count-1;
		Adress=(Adress<<4);
		Adress=Adress+0x2000; 
		 
			{
				 RX8010_GetTime(SaveData);
				
				//LCD_ShowxNum(POS_RT_RH_X-40,5,16,3,SaveData[2],0x80,BLACK18); //
				
				 a = (SaveData[2]&0x0F)%10 + ((SaveData[2]>>4)& 0x03)*10;
				 SaveData[2] = a;//转换为24小时制小时
				
				//LCD_ShowxNum(POS_RT_RH_X-40,45,16,3,SaveData[2],0x80,BLACK18); //
				
				 if(SaveData[2] >= 12)
				 {
					 a = 0x80 | 0x20;//PM
					 SaveData[2] -= 12;
				 }
				 else
				 {
					 a = 0x80;//AM
				 }
				 b = SaveData[2]/10;
				 c = SaveData[2]%10;
				 SaveData[2] = ((b << 4) + c) | a;//时间转换为DS1302格式
				 
				 //LCD_ShowxNum(POS_RT_RH_X-40,95,16,3,SaveData[2],0x80,BLACK18); //

			}
				
		SaveData[7]=ERR_Kind;
		if(RTD_Sensor_Err) SaveData[7]|=0x20;
		if(CQK_Sensor_Err) SaveData[7]|=0x40;
		if(JRP_Sensor_Err) SaveData[7]|=0x80;    		
		SaveData[8]=RT_Temp/256;
		SaveData[9]=RT_Temp%256;
		SaveData[10]=RT_SHIDU/256;
		SaveData[11]=RT_SHIDU%256; 
		//SaveData[10]=CONTROL_RT_SHIDU/256;	//2015-06-04 存储真实的湿度
		//SaveData[11]=CONTROL_RT_SHIDU%256; 
		
		SaveData[12]=CQK_Temp/256;   	
		SaveData[13]=CQK_Temp%256;
		SaveData[14]=JEP_Temp/256;
		SaveData[15]=JEP_Temp%256;
		SPI_Write_nBytes(Adress,16,&SaveData[0]);

   }else if(Enable_Save_Data==4)  //存放数据地址
   {
   	   Store_Data();   	 
   }else if(Enable_Save_Data==5)  //擦调数据
   {
       //清除所有数据,重新计数
       if(Run_Count>=44896) 
        SPI_Erase_Sector(0x000);
  
   }else if(Enable_Save_Data==6)  //擦调数据
   {
       //清除所有数据,重新计数
       if(Run_Count>=44896) 
       {	
         SPI_Erase_Sector(0x1000);        
         Run_Count=0;
       }  
   } else if(Enable_Save_Data==9)  //擦数据
   {

   } 
}


void	Init_port(void) //初始化端口状态--------------------------------
{
	uint16_t j=0;
	
	P1M1=0x08;  //P13为高阻输入 Wire_out_FB
	P1M0=0x0;
	P2M1=0x70;//P24-P26全设定成开漏输出
	P2M0=0xF0;
	P3M1=0x18;  //P34,P33为高阻输入 Wire_In_FB  Plant_FB 
	P3M0=0x64;  //P35,P36,P32为强输出  Heat_Plant_Out  Heat_Wire_In  Heat_JRP_EN_OUT  
	P4M1=0x00;  //IO Port initialization
	P4M0=0x09;  //P40,P43为强输出
	P4SW=0x70;	

	WDT_CONTR = 0x3F; //WDT Enable(4.55S)		
	LCD_LIGHT_CLOSE; 	
	Rec_Count=0;
	Buzzer_Port=0;
	Heat_JRP_Port=0;
	Heat_WireIn_Port=0;
	Heat_WireOut_Port=0;
	Heat_WIRE_EN_OUT = 1;
	EX1=1;
	IT1=1;	 
	LCD_1DIR_L;			
	JEP_Temp=0;
	EA=1;  //
	 
	EUSART_Initialize();//Uart initialization
  SPI_init();//SPI Flash initialization
	LCD_Initial();//TFT LCD initialization	
	 
	//Holding down these two keys for three seconds at boot time will enter service mode. 
	if((KEY_LEFT_UP_IN==0)&&(KEY_LEFT_DOWN_IN==0))
	{
		Work_State = UI_STATE_SERVICE_MODE;
  }
	else
	{
		Work_State = UI_STATE_POST_MODE;
  }
	
//Holding down these two keys for three seconds at boot time will enter Data Reading Interface. 
	if((KEY_LEFT_DOWN_IN==0) &&(KEY_RIGHT_DOWN_IN==0))
	{
		Work_State = UI_STATE_DATAREADER_MODE;//数据传输界面
		ET0=0;
		TR0=0;
		Back_Color=WHITE18;	

		Draw_Rectangle_Real(0,0,239,319,WHITE18);
//Display Data Reading Interface===============	DATA OUTPUT...
		for(j=4;j<8;j++)
		{
				DISP_FNT10X24(115,100+(j-4)*12,j,BLUE18);
		}
		for(j=j;j<14;j++)
		{
				DISP_FNT10X24(115,100+(j-3)*12,j,BLUE18);
		}		
		DISP_FNT10X24(115,100+11*12,14,BLUE18);
		DISP_FNT10X24(115,100+12*12,14,BLUE18);
		DISP_FNT10X24(115,100+13*12,14,BLUE18);	

//Display soft version
		LCD_Show_Verion();	
		LCD_LIGHT_OPEN;
	}
	
	Test_Mode_Dis_Jrp_Ctl = DISPLAY_TEST_DATA_EN_DIS_DEF; //LCD Displays Working Data at Runningtime
}

void	Main_Init(void)	//主程序初始化
{
	Get_RunCount();	 //得到存放数据的条数  

	Diplay_RTtemp = RT_Temp;
	
	CQK_Sensor_Err=0;
	RTD_Sensor_Err=0;
	JRP_Sensor_Err=0;
	SHIDU_Sensor_Err=0;

	Back_Color=BRIGHT_BLUE18 ;

	WireOut_State=0;
	WireIn_State=0;		

	Wire_Mode_Sel = Wire_Sel_Double;//默认为双支加热回路
	Wire_Mode_Mismatch = 0;//回路发热丝模式和实际不匹配标志

	Wire_Warm_Up_Sec = 0; //当<=300时限制最大加热功率
	Plate_Warm_Up_Sec = 0;

	Set_CQK_Temp_Comp = 360;//初始化
	Humidity_No_Change_Sec = 0;//湿度不改变的计数	
	
	JEP_Temp_Aim = Set_CQK_Temp + 100;     //加热盘目标温度 400 - 530
	PID_temp3.Sum_error = 0;
	PID_temp3.Ek = 0;
	PID_temp3.Ek1 = 0;
	PID_temp3.Ek2 = 0;
}



//测试模式时显示温度
//和正常工作时的区别在于实时性
void ServiceMode_TempHumidy_Disp(void)
{
	uint16_t Disp,color;
	bit Disp_Err;
	Back_Color=WHITE18;	
	Disp_Err=0;		

	  if(CQK_Temp>=0x0FFF || No_CQKSensor_Times>20 ) //出气口温度传感器错误检测
	  {
		 CQK_Sensor_Err=1;
		 CQK_Temp = 0;
    }
  	else
  		   CQK_Sensor_Err=0;
		
  	if(No_ReadData_SHIDU_Times>10)//人体端湿度传感器错误检测
  	 {
	     SHIDU_Sensor_Err=1;
			RT_SHIDU = 0; 
		}		
  	else
  		  SHIDU_Sensor_Err=0;
		
    if(No_ReadData_Temp_Times>20 || ERR_RT_Temp_Times>10)//人体端温度传感器错误检测 2014-07-03
  	 { 	 
				RTD_Sensor_Err=1;
				RT_Temp = 0; 		
				Diplay_RTtemp = 0;
			}
			else
  		   RTD_Sensor_Err=0;
			
    if( No_HeatSensor_Times>20)//加热盘传感器错误检测
		{
					JRP_Sensor_Err=1;
				JEP_Temp = 0;
		}

  	else
  			JRP_Sensor_Err=0;

	//人体端温度
	{
	  Disp=RT_Temp;
	  Disp_Err=RTD_Sensor_Err;		  
	}
	color=BLACK18;	 

  if(Disp>=1000)
  {
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+34,(Disp%1000)/100,color);   
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+73,(Disp%100)/10,color);    
    Draw_Rectangle_Real(POS_RT_TEMP_X+1-8,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8-8,POS_RT_TEMP_Y+71,WHITE18); //去掉小数点
  }else
  {
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y,(Disp/100)%10,color);  
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+34,Disp%100/10,color);   
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+34*2+5,Disp%10,color);    
    Draw_Rectangle_Real(POS_RT_TEMP_X+1-8,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8-8,POS_RT_TEMP_Y+71,color); //画点
  }		  

	//出气口温度
	Disp=CQK_Temp; 
  if(Disp>=1000)
  {
    DISP_RH_17X40(55,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20,(Disp%1000)/100,color);   
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20*2+5,(Disp%100)/10,color);    
    Draw_Rectangle_Real(56,POS_RT_TEMP_Y+40,61,POS_RT_TEMP_Y+43,WHITE18); //去掉小数点
  }else
  {
    DISP_RH_17X40(55,POS_RT_TEMP_Y,(Disp/100)%10,color);  
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20,Disp%100/10,color);   
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20*2+5,Disp%10,color);    
    Draw_Rectangle_Real(56,POS_RT_TEMP_Y+40,61,POS_RT_TEMP_Y+43,color); //画点
  }
   
	//加热盘温度
	Disp=JEP_Temp;
	if(Disp>=1000)
	{
	DISP_RH_17X40(5,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20,(Disp%1000)/100,color);   
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20*2+5,(Disp%100)/10,color);    
	Draw_Rectangle_Real(51,POS_RT_TEMP_Y+40,56,POS_RT_TEMP_Y+43,WHITE18); //去掉小数点
	}else
	{
	DISP_RH_17X40(5,POS_RT_TEMP_Y,(Disp/100)%10,color);  
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20,Disp%100/10,color);   
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20*2+5,Disp%10,color);    
	Draw_Rectangle_Real(6,POS_RT_TEMP_Y+40,11,POS_RT_TEMP_Y+43,color); //画点
	}	

	//显示湿度
	{	
	 DISP_RH_17X40(POS_RT_RH_X+2,POS_RT_RH_Y,RT_SHIDU%1000/100,color);
	 DISP_RH_17X40(POS_RT_RH_X+2,POS_RT_RH_Y+19,RT_SHIDU%100/10,color);
	}   
}

//将设定的参数写入到STC EEPROM
//注意：开机时必须把FLASH的数据读入到RAM内
void Setting_write_to_flash(void)
{ 
		STC_IAP_Sector_Erase(DATA_FLASH_ADDRESS_Set_RT_YCTemp);           //擦除整个扇区
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_First, data_flash.First);//将 DEBUG_DATA 写入 EEPROM
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Work_Mode, data_flash.Work_Mode);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_RT_WCTemp, data_flash.Set_RT_WCTemp);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_RT_YCTemp, data_flash.Set_RT_YCTemp);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_In_Exp_Ratio, data_flash.In_Exp_Ratio);//
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_CQK_WCTemp, data_flash.Set_CQK_WCTemp);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_CQK_YCTemp, data_flash.Set_CQK_YCTemp);//
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Language, data_flash.Language);//
}

//将默认的参数写入到STC EEPROM
void Write_Default_Setting_to_flash(void) //把默认出厂设定写入FLASH
{
				data_flash.First = 'Z';
			data_flash.Work_Mode = Invasive_Mode;//有创
			data_flash.Set_RT_WCTemp = (340/5);//
			data_flash.Set_RT_YCTemp = (390/5);//
			data_flash.In_Exp_Ratio = 4;//1:1.3
			data_flash.Set_CQK_WCTemp = (310/5);//
			data_flash.Set_CQK_YCTemp = (360/5);//
			//data_flash.Language = Lan_English;

			Setting_write_to_flash();//写入到FLASH
}


void Mem_Flash_Recall(void) //读取上次的记忆并验证数据
{
	data_flash.First = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_First);    //读EEPROM的值
	data_flash.Work_Mode = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Work_Mode);    
	data_flash.Set_RT_WCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_RT_WCTemp);    
	data_flash.Set_RT_YCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_RT_YCTemp);    
	data_flash.In_Exp_Ratio = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_In_Exp_Ratio);  
	data_flash.Set_CQK_WCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_CQK_WCTemp);    
	data_flash.Set_CQK_YCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_CQK_YCTemp); 
	data_flash.Language = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Language); 
	
	if( //校验数据是否正确
			(data_flash.First != 'Z')
		  ||(data_flash.Work_Mode > 1) 
		  ||(data_flash.Set_RT_WCTemp > Const_NoninvasivePatientTemperature_Max/5) //370/5 = 74;  
		  ||(data_flash.Set_RT_WCTemp < Const_NoninvasivePatientTemperature_Min/5) //300/5 = 60;  
		  ||(data_flash.Set_RT_YCTemp > Const_InvasivePatientTemperature_Max/5)	//400/5 = 80; 
		  ||(data_flash.Set_RT_YCTemp < Const_InvasivePatientTemperature_Min/5)	//350/5 = 70;
		  ||(data_flash.In_Exp_Ratio > 6)
		  ||(data_flash.In_Exp_Ratio < 1)
			||(data_flash.Set_CQK_WCTemp > Const_NoninvasiveChamberTemperature_Max/5) //360/5 = 74;  
		  ||(data_flash.Set_CQK_WCTemp < Const_NoninvasiveChamberTemperature_Min/5) //310/5 = 60;  
		  ||(data_flash.Set_CQK_YCTemp > Const_InvasiveChamberTemperature_Max/5)	//400/5 = 80; 
		  ||(data_flash.Set_CQK_YCTemp < Const_InvasiveChamberTemperature_Min/5)	//350/5 = 70;
			||(data_flash.Language > Lan_French)//最多4种语言
	  )
	  {
			data_flash.Language = Lan_English;
			Write_Default_Setting_to_flash();//把默认出厂设定写入FLASH
	  }
}



void Load_Settings_Before_Choice(void)//在选择方框出现之前，要处理的数据，以同时兼容有无选择记忆界面程序
{
	if(defalut_mode == Load_Fac_MODE)  //若是工厂默认设定  
	{
		Write_Default_Setting_to_flash();//把默认出厂设定写入FLASH
	}

		Work_Mode = data_flash.Work_Mode;  //工作模式
		Set_RT_WCTemp = data_flash.Set_RT_WCTemp * 5;//
		Set_RT_YCTemp = data_flash.Set_RT_YCTemp * 5;//
		Set_CQK_WCTemp = data_flash.Set_CQK_WCTemp * 5;//
		Set_CQK_YCTemp = data_flash.Set_CQK_YCTemp * 5;//
		In_Exp_Ratio = data_flash.In_Exp_Ratio;	 //

		if(Work_Mode==0)	
		{			
			Set_RT_Temp=Set_RT_WCTemp;
			Set_CQK_Temp=Set_CQK_WCTemp;
		}
		else
		{
			Set_RT_Temp=Set_RT_YCTemp; 
			Set_CQK_Temp=Set_CQK_YCTemp; 
		}
}

/*低功率模式
功能---进入待机模式时，a,加热盘加热时长被限定在50且温度不超过50度
											 b,回路发热丝加热时长被限定在40
目的---当出现人体端或出气口低温报警时，除了气源，回路被堵等原因外，还有可能是探头脱落，
				若完全不加热，则在用户未干预时完全不加热至冷却到室温，用户可能会认为机器有严重故障
				若按正常模式加热，则可能会由于探头脱落导致大功率加热，引起超温而产生风险。
进入待机模式条件---人体端或出气口出现低温报警，且有报警音时
退出待机模式条件---低温报警解除，或按消音键可暂时退出90S
*/
void LowPowerModeFunc(void)	 
{	
	if((LoTemp_Count > 120 || LoTemp_CQK_Count > 120)||(Bit_is_one(ERR_Kind,Alarm_Const_NoWater))) //无水(干烧)报警
	{
			if(		
				   (ERR_Kind & 0x01)!=0//传感器错误
				|| (ERR_Kind & 0x02)!=0//高温
				|| Wire_Mode_Mismatch == 1  //发热丝未选
				|| HeaterPlate_State==0)//水罐未装好
			{	
			}
			else	
			{			
				if(AlarmSoundPauseStatus == 0)//未按下静音键
				{
					Low_Power_Mode_Flag = 1;//低功率模式标志
					if(JEP_Temp<=450)
					{				
						Micro_Temp_Val=50;  //停止加热
					}
					else
					{
						Micro_Temp_Val=0;  //停止加热
					}
					Micro_Temp_In=40;
					Micro_Temp_Out=Micro_Temp_In;			
					Micro_Adj_Mode_Test = 4;//加热模式测试
				}
				else
					Low_Power_Mode_Flag = 0;
			}
	}
	else
		Low_Power_Mode_Flag = 0;
}




//增量PID算法
//ΔU = U(k)-U(k-1) = Kp*[e(k)-e(k-1)]+Ki*e(k)+Kd*[e(k)-2*e(k-1)+e(k-2)]
//* 其中：Kp、Ki、Kd分别为比例、积分和微分放大系数，u(k)表示第k个采样时刻
//的控制量,e(k)表示第k个采样时刻的航向输入偏差.
//PIE_UK = Kp*(Ek-Ek1)+Ki*EK+Kd*(Ek+Ek2-EK1)

//struct PID
//	 	{
//		 	int Uk; //总的控制量
//		 	int Uk1;//上次的总控制量
//		 	int Sum_error;//误差总量
//		 	int Ek;//当前的总误差量
//		 	int Ek1;//前一次的误差量
//		 	int Ek2;//前二次的误差量       
//       }PID_temp;//

//有发热丝时的患者端温度控制PID算法---根据患者端目标温度和实际温度，计算出加热回路驱动值  每1秒工作一次
void PID_Calc(void) //设定温度最高400,最低000,实际0-800,  
{

 	int Vlaue1,tmp;
 	static int PID_Kp = 40;//比例系数 5度为1500;1度为300;0.1度为30
 	static int PID_Ki = 8;//积分系数
 	static int PID_Kd = 20;//微分系数 	

	if(PID_temp.Ek > 100)//10度为极限 
	{
		PID_temp.Ek = 100;
	}
	if(PID_temp.Ek < -100)
	{
		PID_temp.Ek = -100;
	}
	
 	PID_temp.Sum_error += PID_temp.Ek;//误差总量
 	if(PID_temp.Sum_error > 3500) //0.1度内最多是250一周期
 	{
 		PID_temp.Sum_error = 3500;
 	}
 	if(PID_temp.Sum_error < -3500)
 	{
		
 	}
	Vlaue1 = PID_Kp * PID_temp.Ek;//比例 最大100*30=3000;  误差*比例系数(假定相差5度即50*25=1250)
	
	tmp = PID_temp.Sum_error * PID_Ki / 50; //积分 最大3000*2/100=60,一个加热周期是250次  250*5/15*3=250;
	Vlaue1 += tmp;
 	tmp = (PID_temp.Ek - (2 * PID_temp.Ek1) + PID_temp.Ek2) * PID_Kd;//微分 
	PID_temp.Sum_error = PID_temp.Sum_error + tmp;//2015-06-16 把微分计入积分和
 	Vlaue1 += tmp;
	PID_temp.Uk = Vlaue1;// PID_temp.Uk1 + Vlaue1;       // 结果 /100最大为250次
 	if(PID_temp.Uk > 780) 
 		PID_temp.Uk = 780;
	else if(PID_temp.Uk < 0)
		PID_temp.Uk = 0;//不能为负数

	PID_temp.Uk /= 4;//最高190; 	
		
 	PID_temp.Ek2 = PID_temp.Ek1;        // 保存新的K-1次输入值
 	PID_temp.Ek1 = PID_temp.Ek;
}


//加热盘驱动的PID算法--- 根据加热盘的目标温度和实际温度，计算出加热回路驱动值  每1秒工作一次
void HeatPlate_PID_Calc(void) //设定温度最高400,最低000,实际0-800,  
{

 	int Vlaue1,tmp;
//	uint16_t  Val_Calc;
 	static int PID_Kp2 = 38;//比例系数 //比例系数 10度为4000/20=200;1度为400/20=20;0.1度为2
 	static int PID_Ki2 = 8;//积分系数
 	static int PID_Kd2 = 20;//微分系数 	

	if(PID_temp2.Ek > 100)//10度为极限 
	{
		PID_temp2.Ek = 100;
	}
	if(PID_temp2.Ek < -100)
	{
		PID_temp2.Ek = -100;
	}
	
 	PID_temp2.Sum_error += PID_temp2.Ek;//误差总量
 	if(PID_temp2.Sum_error > 3500) //0.1度内最多是250一周期
 	{
 		PID_temp2.Sum_error = 3500;
 	}
 	if(PID_temp2.Sum_error < -3500)
 	{
 		PID_temp2.Sum_error = -3500;
 	} 	

	Vlaue1 = PID_Kp2 * PID_temp2.Ek;//比例 最大100*30=3000;  误差*比例系数(假定相差5度即50*25=1250)
	
	tmp = PID_temp2.Sum_error * PID_Ki2/50; //积分 最大3000*2/100=60,一个加热周期是250次  250*5/15*3=250;
	Vlaue1 += tmp;
 	tmp = (PID_temp2.Ek - (2 * PID_temp2.Ek1) + PID_temp2.Ek2) * PID_Kd2;//微分 
	PID_temp2.Sum_error = PID_temp2.Sum_error + tmp;//2015-06-16 把微分计入积分和
 	Vlaue1 += tmp;
	PID_temp2.Uk = Vlaue1;//;       // 结果 /100最大为250次
 	if(PID_temp2.Uk > 780) 
 		PID_temp2.Uk = 780;
	else if(PID_temp2.Uk < 0)
		PID_temp2.Uk = 0;//不能为负数

	PID_temp2.Uk /= 4;//最高195; 		

 	PID_temp2.Ek2 = PID_temp2.Ek1;        // 保存新的K-1次输入值
 	PID_temp2.Ek1 = PID_temp2.Ek;
}


//加热盘目标温度的PID算法--- 根据出气口的目标温度和实际温度，计算出加热盘目标温度的偏差, 每20秒工作一次
static void HeatPlateTemp_Aim_PID_Calc(void) // 
{
 	int Vlaue1,tmp;
//	uint16_t  Val_Calc;
 	static int PID_Kp3 = 40;//比例系数 10度为4000/100=40;1度为400/100=4;0.1度为0.4
 	static int PID_Ki3 = 1;//积分系数
 	static int PID_Kd3 = 80;//微分系数 	

	if(PID_temp3.Ek > 100)//10度为极限 
	{
		PID_temp3.Ek = 100;
	}
	if(PID_temp3.Ek < -100)
	{
		PID_temp3.Ek = -100;
	}
	
 	PID_temp3.Sum_error += PID_temp3.Ek;//误差总量
 	if(PID_temp3.Sum_error > 1000) //0.1度内最多是250一周期
 	{
 		PID_temp3.Sum_error = 1000;
 	}
 	if(PID_temp3.Sum_error < -1000)
 	{
 		PID_temp3.Sum_error = -1000;
 	} 	

	Vlaue1 = PID_Kp3 * PID_temp3.Ek;//比例 最大100*40=4000;  误差*比例系数(假定相差5度即50*25=1250)

	
	tmp = PID_temp3.Sum_error * PID_Ki3; //积分 最大3500*8/100=280
	Vlaue1 += tmp;
 	//tmp = (PID_temp3.Ek - (2 * PID_temp3.Ek1) + PID_temp3.Ek2) * PID_Kd3;//微分 
	tmp = (2 * PID_temp3.Ek - PID_temp3.Ek1 - PID_temp3.Ek2) * PID_Kd3;//微分 
	PID_temp3.Sum_error = PID_temp3.Sum_error + tmp;//把微分计入积分和
 	Vlaue1 += tmp;
	PID_temp3.Uk = Vlaue1;// PID_temp.Uk1 + Vlaue1;       // 结果 /100最大为250次
 	if(PID_temp3.Uk > 5000) 
 		PID_temp3.Uk = 5000;
	else if(PID_temp3.Uk < -5000)
		PID_temp3.Uk = -5000;//负数

	PID_temp3.Uk /= 100;//最高50; 		

 	PID_temp3.Ek2 = PID_temp3.Ek1;        // 保存新的K-1次输入值
 	PID_temp3.Ek1 = PID_temp3.Ek;
}


//无回路发热丝时的患者端温度调节
static uint16_t NoneWire_Warm_Up_Sec=0;
static void PatientTemp_NoneHeatWire_Adj(void)
{
	  static int err_sum=0;
   	static int RT_Temp_Mem = 0;
//	static uint16_t WarmUpstate=0;
	  int Calc_int=0;
		uint16_t Val_Calc;	   	

		{
				if(CQK_Temp < 330)//若出气口温度低于33度则全功率加热
				{
					//Micro_Temp_Val = 190;
					//以下为发热盘温度的PID控制
					
					PID_temp2.Ek = 980 - JEP_Temp;//当前的误差量 设定温度-当前实际温度
					HeatPlate_PID_Calc(); 
					Val_Calc = (uint8_t)PID_temp2.Uk;	//计算出加热时间
					Micro_Temp_Val = Val_Calc;//便于计算，以免超过190进入中断	
					
					Temp2_Int = JEP_Temp;
					
				}
				else
				{	
					NoneWire_Warm_Up_Sec++;
					if(NoneWire_Warm_Up_Sec > 2000)NoneWire_Warm_Up_Sec = 2000;
					if(NoneWire_Warm_Up_Sec < 3)//00)//10分钟内设定
					{
						//Temp2_Int = 500 + Set_RT_Temp - 300;//发热盘温度设定在50度-60度
						if(Temp2_Int < (550 + (Set_RT_Temp - 300)*2))//发热盘温度设定在50度-60度
							Temp2_Int = 550 + (Set_RT_Temp - 300)*2;
						  RT_Temp_Mem = RT_Temp;
					}
					else
					{	
						err_sum = err_sum + Set_RT_Temp - RT_Temp;//积分
						if(err_sum > 180)err_sum=180;
						else if(err_sum < -180)err_sum=-180;//每秒10度(100)，30000/100=300,即为5分钟数据
						
						//if((RT_Temp - Set_RT_Temp) >= 1){if(err_sum > 0)err_sum = 0;}//达到设定温度，误差不能为正
						//else if((Set_RT_Temp - RT_Temp) >= 1){if(err_sum < 0)err_sum = 0;}//未达到设定温度，误差不能为负
						
						if((Work_State != UI_STATE_SCREENSAVER_MODE)&&(Test_Mode_Dis_Jrp_Ctl == 1))
						{	//8表示高位为0也显示,0表示非叠加显示 出气口
							Back_Color=WHITE18;
							if(err_sum >=0)LCD_ShowxNum(220,10,16,1,1,0x80,BLACK18); //1-表示为正数
											  else LCD_ShowxNum(220,10,16,1,0,0x80,BLACK18); //0-表示为负数
							LCD_ShowxNum(220,20,16,5,abs(err_sum),0x80,BLACK18); //发热盘加热的模式显示
						}						

						NoneWire_Heat_Sec++;
						if(NoneWire_Heat_Sec >= 180)//3分钟调整一次温度
						{
							NoneWire_Heat_Sec = 0;//-Temp2_Int
							
							Calc_int = err_sum;
							//if(Calc_int > 6000)Calc_int=6000;
							//else if(Calc_int< -6000)Calc_int=-6000;//每秒5度(50)，2分钟数据为6000
							
						  Temp2_Int = Temp2_Int + (Set_RT_Temp - RT_Temp)/6 + (RT_Temp_Mem-RT_Temp)/2 + Calc_int/40;//(400-300)/10
							//if(Temp2_Int > 1050)Temp2_Int = 1050;
							if(Temp2_Int > 980)Temp2_Int = 980;

							RT_Temp_Mem = RT_Temp;
						}
					}						

					//以下为发热盘温度的PID控制
					PID_temp2.Ek = Temp2_Int - JEP_Temp;//当前的误差量 设定温度-当前实际温度
					HeatPlate_PID_Calc(); 
					Val_Calc = (uint8_t)PID_temp2.Uk;	//计算出加热时间
					if(RT_Temp > (Set_RT_Temp + 10))
					{
						Micro_Temp_Val = 0;//超过1度 
						
					}
					else	
						Micro_Temp_Val = Val_Calc;//便于计算，以免超过190进入中断	
			}				
		}
}

//正常运行界面,按下确认键确定回路的模式
void WireInOut_State_Confirm(void)
{
			if((WireIn_State > 0) && (WireOut_State > 0)) //双管加热模式
			{
					Wire_Mode_Sel = Wire_Sel_Double;	
			} 
			else if((WireIn_State > 0) && (WireOut_State == 0)) //单管加热模式
			{
					Wire_Mode_Sel = Wire_Sel_In_Only;	
			}
			else if((WireIn_State == 0) && (WireOut_State == 0)) //无加热丝模式
			{
					Wire_Mode_Sel = Wire_Sel_None;	
			}	
}

void HeaterPlateWireDriveFbTask(void)//16
{
	static uint8_t Micro_Temp_CLK = 0;
	static uint8_t xdata   Check_WireInTimes = 0;
	static uint8_t xdata   Check_WireOutTimes =0 ;
	static uint8_t WireIn_FB_Count = 0;     //检测加热线In反馈信号
	static uint8_t WireOut_FB_Count = 0;     //检测加热线Out反馈信号
	static uint8_t NoHpCheck_Times = 0;//无加热盘的次数.
	
	Micro_Temp_CLK++;
	if(Micro_Temp_CLK > 200)//时基
	{		
			Micro_Temp_CLK = 0;
		
//每个周期检测加热盘是否存在
			 HP_CNT_Int_End_Rem = HP_CNT_Int;
			if(HP_CNT_Int < 1)//未检测到
			{
				NoHpCheck_Times++;
				if(NoHpCheck_Times >=2)
				{
					NoHpCheck_Times = 2;
					HeaterPlate_State=0; //未检测到加热盘
				}
			}
			else
			{
				NoHpCheck_Times = 0;
				HeaterPlate_State=1; //检测加热盘存在					
			}
			HP_CNT_Int = 0;//清零
	 }
	 
//加热盘驱动	
	 if (Micro_Temp_Val>Micro_Temp_CLK)
	 {
			Heat_JRP_Port=1;
	 }	 
	 else
	 {
			Heat_JRP_Port=0;
	 }
	
//INSP进支气加热驱动和检测
	if (Micro_Temp_In>Micro_Temp_CLK)
	{
			 Heat_WireIn_Port=1;
			 Heat_WIRE_EN_OUT=0;//防止单一故障	
	}
	else  //不加热时的检测
	{	 	  
		  if(Heat_WireIn_Port!=0)
	 	  {
	  	   Heat_WireIn_Port=0;
				 Heat_WIRE_EN_OUT=1;//防止单一故障	  	
	  	   Check_WireInTimes=0;
	  	   WireIn_FB_Count=0;
	  	}
			else
	  	{
	  		Check_WireInTimes++;
	  		if(WireIn_Exist_Port==0) WireIn_FB_Count++;
	  		if(Check_WireInTimes>4)
	  		{
	  			 if(WireIn_FB_Count<3)   //连续检测到五次不加热报错
	  			 {
						 WireIn_State=0; //未检测到发热丝						 
	  			 }
	  			 else
	  			 {						 
						 WireIn_State=1; //没加热，代表检测到发热丝
	  			 }
	  			 Check_WireInTimes=0;
	  			 WireIn_FB_Count=0;
	     }
	   }
	}			
	
//EXP出支气加热驱动和检测
	if (Micro_Temp_Out>Micro_Temp_CLK)
	{
			 Heat_WireOut_Port=1;
			 Heat_WIRE_EN_OUT = 0;//防止单一故障	  
	 }
	 else
	 {
	 	  if(Heat_WireOut_Port!=0)
	 	  {
	  	   Heat_WireOut_Port=0;
				 Heat_WIRE_EN_OUT = 1;//防止单一故障	  
	  	   Check_WireOutTimes=0;
	  	   WireOut_FB_Count=0;
	  	}else
	  	{
	  		Check_WireOutTimes++;
	  		if(WireOut_Exist_Port==0) WireOut_FB_Count++;
	  		if(Check_WireOutTimes>4  )
	  		{
	  			 if(WireOut_FB_Count<3)   //连续检测到五次不加热报错
	  			 {
	  			 	  WireOut_State=0; //没加热，代表检测到发热丝
	  			 }
	  			 else
	  			 {
	  			 	  WireOut_State=1;
	  			 }
	  			 Check_WireOutTimes=0;
	  			 WireOut_FB_Count=0;
	     }
	   }
	 }	 
}

			
											
											