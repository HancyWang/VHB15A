#include "all.h"

//2019.03.29
sfr WDT_CONTR = 0xC1;
sbit P21 = (u8)0xA0^(u8)1;
sbit P22 = (u8)0xA0^(u8)2;
sbit P23 = (u8)0xA0^(u8)3;
//sbit P40 = (u8)0xC0^(u8)0;
sbit P40 = (u8)0xC0;

static uint8_t Tik_POST_Tick_100mS = 0;//运行模式的时标
static uint8_t Tik_HmiFac_Tick_100mS_10S = 0;//10S内不响应则闪烁放入前台任务
static uint8_t OK_not_Pressed_flag = 0;//OK键没有被按下标志，10S后启动此标志
static uint8_t HmiFac_Mode_Changed = 1;//模式被改变
//static uint8_t HmiLan_Sel_Changed = 1;//模式被改变
static uint8_t Tik_HmiNon_InvasiveSel_Tick_100mS = 0;//有创无创选择界面的时标
static uint16_t  Tik_ScreenSaver_Tick_100mS = 0;

static uint16_t   Remeber_Temp_Value;//记忆设定的数值

static void Display_SET_Temp(uint16_t Temp);
static void EnterSetTime(void);//进入时间设定
static void EnterSetInExp(void);//进入InExp设定
static void Date_Is_Correct(void);//日期校正
static void Display_In_Exp_Ratio(uint8_t Dis,uint8_t Dp_en);//显示In/Exp 

static void Display_In_Exp_Ratio(uint8_t Dis,uint8_t Dp_en)//显示In/Exp  
{
	if(Dp_en!=(uint8_t)0)
	{
		LCD_ShowString(POS_RT_RH_X ,POS_RT_RH_Y+120,16,200,(const u8*)"In/Exp:",0,BLACK18);		

		show_str[0]=(u8)'1';
		show_str[1]=(u8)':';
		show_str[2]=(u8)'1';
		if(Dis <= (uint8_t)1)  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
		{
			show_str[3]=(u8)' ';
			show_str[4]=(u8)' ';
		}
		else
		{
			show_str[3]=(u8)'.';
			show_str[4]=(u8)'0' + Dis-(u8)1;
		}
		show_str[5] = (u8)'\0';
		LCD_ShowString(POS_RT_RH_X ,POS_RT_RH_Y+180,16,200,(const u8*)show_str,0,BLACK18); 
	}
	else
	{
		LCD_ShowString(POS_RT_RH_X ,POS_RT_RH_Y+120,16,200,(const u8*)"In/Exp:",0,WHITE18); //不显示
		LCD_ShowString(POS_RT_RH_X ,POS_RT_RH_Y+180,16,200,(const u8*)"      ",0,WHITE18);	
	}
	
}

void DisPlayTime(const uint8_t TempTime[7],uint8_t TimeKind)//显示时间
{
	uint8_t i;
	uint8_t color;
	//2-7//年/月/日/时/分/秒
	
	color = BLACK18;
	if(TimeKind == (uint8_t)7)
	{	//显示秒-------------------------------------------------	   
		i=(TempTime[0]&(uint8_t)0xF)%(u8)10;
		DISP_TIME_10X16(POS_SYSTIME_X,POS_SYSTIME_Y+68,i,BLACK18);	 
		i=(TempTime[0]>>4)& (uint8_t)0x7;
		DISP_TIME_10X16(POS_SYSTIME_X,POS_SYSTIME_Y+56,i,color);
	}
	else if(TimeKind == (uint8_t)6)
	{ //显示分========------------------------------------------	   
		i=(TempTime[1]&(uint8_t)0xF)%(u8)10;
		DISP_TIME_10X16(POS_SYSTIME_X,POS_SYSTIME_Y+40,i,color);
		i=(TempTime[1]>>4) & (uint8_t)0x7;
		DISP_TIME_10X16(POS_SYSTIME_X,POS_SYSTIME_Y+28,i,color);
	}
	else if(TimeKind == (uint8_t)5)
	{ //显示时,采用24小制=========================== 		  
		DISP_TIME_10X16(POS_SYSTIME_X,POS_SYSTIME_Y,TempTime[2]>>4,color);//BCD码
		DISP_TIME_10X16(POS_SYSTIME_X,POS_SYSTIME_Y+12,TempTime[2]&(uint8_t)0x0f,color);
	}
	else if(TimeKind == (uint8_t)4)	 
	{//显示日
		i=(TempTime[3]&(uint8_t)0xF)%(u8)10;
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+92,i,color);
		i=(TempTime[3]>>4)& (uint8_t)0x03;
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+80,i,color);	 
	}
	else if(TimeKind == (uint8_t)3)	 	   
	{//显示月
		i=(TempTime[4]&(uint8_t)0xF)%(u8)10;
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+64,i,color);
		i=(TempTime[4]>>4)& (uint8_t)0x1;
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+52,i,color);
	}
	else if(TimeKind == (uint8_t)2)
	{//显示年	 
		i=(TempTime[6]&(uint8_t)0xF)%(u8)10;
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+36,i,color);
		i=(TempTime[6]>>4)%(u8)10;
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+24,i,color);
	}	
	else
	{
		//do nothing
	}
}

//显示设定温度
static void Display_SET_Temp(uint16_t Temp)
{
	Back_Color=WHITE18;  	//显示设定值
	//温度图标开始闪烁,进入温度设定
	DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y,(uint8_t)(Temp/(u16)100),BLACK18);  
	DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34,(u8)(Temp%(u16)100/(u16)10),BLACK18);   
	DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*2)+5,(u8)(Temp%(u16)10),BLACK18);
	Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,BLACK18); //画点 //画点
}


//-----------进入某个工作模式界面
void	HmiEnterToWorkStateFunc(WORK_STATUS State)
{
	switch(State)	
	{	
		case UI_STATE_SERVICE_MODE:
		{	
			LCD_LIGHT_CLOSE;
			Back_Color=WHITE18;
			Draw_Rectangle_Real(0,0,239,319,WHITE18);	//清屏

			break;
		}		
		case UI_STATE_POST_MODE:
		{
			Tik_POST_Tick_100mS = 0;			
			LCD_LIGHT_CLOSE;
			Back_Color=WHITE18;
			Draw_Rectangle_Real(0,0,239,319,WHITE18);	//清屏
			break;
		}
		case UI_STATE_FACTORY_DEFAULT_SEL_MODE:
		{
			Tik_HmiFac_Tick_100mS_10S = 0;			
			OK_not_Pressed_flag = 0;//OK键没有被按下标志，10S后启动此标志
			

			{
				Mem_Flash_Recall(); //读取上次的记忆并验证数据					

				defalut_mode = Load_User_Pre_MODE;	//预设模式选择	
				HmiFac_Mode_Changed = 1;//模式被改变
				
				LCD_LIGHT_CLOSE;
				Draw_Rectangle(0,0,239,319,BLUE18,4);
				Draw_Rectangle_Real(0,0,239,319,WHITE18);	
				
				Back_Color=WHITE18;
				{
					CHAR str_previousSetting[]="Previous Settings";
					CHAR str_defaultSetting[]="Default Settings";
					CHAR str_select[]="Select";
					CHAR str_enter[]="Enter";
					CHAR str_Patient39C[]="Patient:39 C  ";
					CHAR str_chamber36C[]="Chamber:36 C  ";
					
					strcpy((CHAR*)show_str,str_previousSetting);				
					LCD_ShowString(Load_User_Pre_Set_X,Load_User_Pre_Set_Y+14,24,280,(const u8*)show_str,0,BLUE18);	
					strcpy((CHAR*)show_str,str_defaultSetting);				
					LCD_ShowString(Load_Fac_Set_X,Load_Fac_Set_Y+14,24,280,(const u8*)show_str,0,BLUE18);	
					strcpy((CHAR*)show_str,str_select);				
					LCD_ShowString(20,125,16,280,(const u8*)show_str,0,BLUE18);
					strcpy((CHAR*)show_str,str_enter);				
					LCD_ShowString(20,245,16,280,(const u8*)show_str,0,BLUE18);
						
					Back_Color=WHITE18;		
					strcpy((CHAR*)show_str,str_Patient39C);					
					LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y,16,280,(const u8*)show_str,0,BLUE18);
					Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*10),2,BLUE18);//温度图标		
					strcpy((CHAR*)show_str,str_chamber36C);						
					LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*15),16,280,(const u8*)show_str,0,BLUE18);
					Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*10)+(8*15),2,BLUE18);//温度图标						
					LCD_ShowString(Load_User_Pre_Set_X - 76,Load_User_Pre_Set_Y,16,280,(const u8*)"In/Exp:1:1.3",0,BLUE18);//						
					Draw_Rectangle(Load_Fac_Set_X+39,Load_User_Pre_Set_Y-10,Load_User_Pre_Set_X-15,Load_User_Pre_Set_Y+242,RED18,2);//画参数框					
					Draw_Rectangle_Real(143,Load_User_Pre_Set_Y-30,144,Load_User_Pre_Set_Y-10,RED18);//画选择参数横线						
				}
			
				Back_Color=WHITE18;
				DISP_ICO_40X40(15,30,3,BLUE18);//显示UP图标	
				DISP_ICO_40X40(15,80,4,BLUE18);//显示DOWN图标	
				DISP_ICO_40X40(15,200,2,BLUE18);//显示OK图标
			}		
			break;
		}	
			
		case UI_STATE_NON_INVASIVE_MODE:
		{			
			Tik_HmiNon_InvasiveSel_Tick_100mS = 0;//有创无创的时标
			LCD_LIGHT_CLOSE;
			Draw_Rectangle_Real(0,0,239,319,WHITE18);	//清屏
			Back_Color=WHITE18;
			Disp_WuChuang(POS_Select_WC_X,POS_Select_WC_Y,BLUE18);
			Disp_YouChuang(POS_Select_YC_X,POS_Select_YC_Y,BLUE18);
			Draw_Rectangle(75,182,162,262,WHITE18,4);
			Draw_Rectangle(75,58,162,136,RED18,4);
			LCD_LIGHT_OPEN;	
			break;
		}
		
		case UI_STATE_RUNNING_NORMAL_MODE:
		{
//			Tik_HmiRunning_Tick_100mS = 0;
			LCD_LIGHT_CLOSE;			
			DrawWorkWindows();			
			RefreshRunTimeFunc(1);//强制刷新运行时间
			RefreshTempHumidyFunc(1);//强制刷新温度显示			
			LCD_LIGHT_OPEN;
			Tik_ScreenSaver_Tick_100mS = 0;
			break;
		}	

		case UI_STATE_SetTempPatient_MODE:
		{
			Remeber_Temp_Value = (uint16_t)Set_RT_Temp;	
			DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);		
			Display_SET_Temp(Remeber_Temp_Value);			
//			Set_CQK_Subtract_RT = Set_CQK_Temp - Set_RT_Temp;//出气口和人体端设定温度差，记忆此差值	
			break;
		}
		
		case UI_STATE_SetTempChamber_MODE:
		{
			Remeber_Temp_Value = Set_CQK_Temp;	
			DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);		
			Display_SET_Temp(Remeber_Temp_Value);		
			break;
		}
		
		case UI_STATE_SetTime_MODE: 
		{
			EnterSetTime();
			break;
		}
		
		case UI_STATE_SetInExp_MODE: 
		{
//			Display_In_Exp_Ratio(In_Exp_Ratio,1);	
			EnterSetInExp();//进入InExp设定
			break;
		}
		case UI_STATE_POWER_OFF_MODE:
			break;
		case UI_STATE_SCREENSAVER_MODE:
			break;
		case UI_STATE_DATAREADER_MODE:
			break;
		default:
			//do nothing
			break;
	}
}

//
void HmiServiceModeFunc(void)
{
	static INT ServiceMode_Step = 0;
	
  //显示屏纯色测试 	
	if(ServiceMode_Step == 0)
	{
		ServiceMode_Step = 1;
		Draw_Rectangle_Real(0,0,239,319,RED18);	  //红
		Back_Color=RED18;
		LCD_ShowString(220,20,16,200,(const u8*)"LCD TEST_RED(Test 1/6)",0,WHITE18);
		LCD_ShowString(200,20,16,200,(const u8*)"Press Left Up Key...",0,WHITE18); 
		LCD_LIGHT_OPEN;	
	}
	else if(ServiceMode_Step == 1)
	{
		if(Key_State_Present == KEY_STATE_UP_Short)//向下键到工厂设定
		{
			ServiceMode_Step = 2;			
			Draw_Rectangle_Real(0,0,239,319,GREEN18);  //绿
			Back_Color=GREEN18;
			LCD_ShowString(220,20,16,200,(const u8*)"LCD TEST_GREEN(Test 2/6)",0,RED18);
			LCD_ShowString(200,20,16,200,(const u8*)"Press Left Down Key...",0,RED18);
		}		
	}
	else if(ServiceMode_Step == 2)
	{
		if(Key_State_Present == KEY_STATE_Down_Short)
		{
			ServiceMode_Step = 3;
			Draw_Rectangle_Real(0,0,239,319,BLUE18);   //蓝
			Back_Color=BLUE18;
			LCD_ShowString(220,20,16,200,(const u8*)"LCD TEST_BLUE(Test 3/6)",0,RED18);
			LCD_ShowString(200,20,16,200,(const u8*)"Press Right Down Key...",0,RED18);	
		}
	}
	else if(ServiceMode_Step == 3)
	{	
		if(Key_State_Present == KEY_STATE_Mute_Short)
		{
			ServiceMode_Step = 4;
			Draw_Rectangle_Real(0,0,239,319,BLACK18);  //黑
			Back_Color=BLACK18;
			LCD_ShowString(220,20,16,200,(const u8*)"LCD TEST_BLACK(Test 4/6)",0,RED18);
			LCD_ShowString(200,20,16,200,(const u8*)"Press Right Up Key...",0,RED18);	
		}
	}
	else if(ServiceMode_Step == 4)
	{	
		if(Key_State_Present == KEY_STATE_OK_Short)
		{
			uint8_t TRUE=(uint8_t)1;
			//LCD测试结束	
			ServiceMode_Step = 5;
			Draw_Rectangle_Real(0,0,239,319,WHITE18);  //W25X读写测试
			Back_Color=WHITE18;
			LCD_ShowString(220,20,16,280,(const u8*)"W25X Write&Read Test(Test 5/6)",0,GRAY18);
			LCD_ShowString(200,20,16,200,(const u8*)"Press Left Up Key...",0,GRAY18);		

			//WX25 测试
			SPI_Erase_Sector(0x100000);//1M字节起启扇区擦除
			WDT_CONTR = 0x3F; ;	
			delay_ms(300);
			SaveData[0] = (u8)'O';
			SaveData[1] = (u8)'K'; 
			SPI_Write_nBytes(0x100000,2,SaveData);
			delay_ms(10);
			SaveData[0] = (u8)0xff;
			SaveData[1] = (u8)0xff;
			SPI_Read_nBytes(0x100000,2,SaveData); 
			//*/
			
			if((SaveData[0] == (u8)'O')&&(SaveData[1] == (u8)'K'))
			{								 
				LCD_ShowString(160,70,16,200,(const u8*)"W25X TEST OK!",0,RED18);
			}
			else
			{
				LCD_ShowString(160,70,16,200,(const u8*)"W25X TEST Fail!",0,RED18);
				while(TRUE!=(uint8_t)0)
				{
					WDT_CONTR = 0x3F;
				}
			}	
			//显示版本号
			LCD_Show_Verion();
		}
	}
	else if(ServiceMode_Step == 5)
	{
		if((KEY_LEFT_DOWN_IN == (bit)0)&&(KEY_RIGHT_DOWN_IN == (bit)0))  //显示版本日期
		{
			//显示日期	
			LCD_ShowString((u16)20,(u16)20,(u16)16,(u16)200,(const u8*)DATE_VER,0,GRAY18);		
		}
		else if(Key_State_Present == KEY_STATE_UP_Short)
		{
			ServiceMode_Step = 6;
			
			LCD_LIGHT_CLOSE;//背光 
			Back_Color=WHITE18;		
			Draw_Rectangle_Real(0,0,239,319,WHITE18);  //白
			LCD_ShowString(220,20,16,280,(const u8*)"Temperature & Humidity(Test 6/6)",0,RED18);

			DISP_ICO_52X64(POS_ICO_TEMP_X-8,POS_ICO_TEMP_Y,0,BLACK18);//人体端图标
			DISP_ICO_32X40(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);//温度单位
			DISP_ICO_40X40(POS_ICO_SHIDU_X,POS_ICO_TEMP_Y,0,BLACK18); //湿度图标
			DISP_ICO_32X40(POS_RT_RH_X+2,POS_RT_RH_Y+(19*2),ICO_PER,BLACK18); //显示湿度单位 	
			DISP_RH14X24(POS_RT_RH_X+5,POS_RT_RH_Y+70,0,BLACK18);
			DISP_RH14X24(POS_RT_RH_X+5,POS_RT_RH_Y+84,1,BLACK18);
			DISP_ICO_52X64(55,POS_ICO_TEMP_Y,1,BLACK18); //出气口温度图标
			DISP_ICO_32X40(55,POS_ICO_TEMP_Y+(35*3)+5,ICO_DU,BLACK18);//温度单位
			DISP_HEAT_36X24(15,POS_ICO_TEMP_Y+5,BLACK18);//加热盘温度图标 
			DISP_ICO_32X40(5,POS_ICO_TEMP_Y+(35*3)+5,ICO_DU,BLACK18);//温度单位 
			LCD_LIGHT_OPEN;//开背光			
		}
		else
		{
			//do nothing
		}
	}
	else if(ServiceMode_Step == 6)
	{
		ServiceMode_TempHumidy_Disp(); //测试模式下显示温湿度
		
		if(KEY_RIGHT_UP_IN==0)
		{
			if(JEP_Temp<=800)
			{
				Micro_Temp_Val = 201;
			}
			else
			{
				Micro_Temp_Val = 0;
			}
			if(RT_Temp<=400)
			{
				Micro_Temp_In = 201;
				Micro_Temp_Out = 201;
			}
			else
			{
				Micro_Temp_In = 0;
				Micro_Temp_Out = 0;
			}
		}
		else
		{
			Micro_Temp_Val = 0;
			Micro_Temp_In = 0;
			Micro_Temp_Out = 0;
		}
		
		if((KEY_LEFT_DOWN_IN == 0)&&(KEY_RIGHT_DOWN_IN==0))//同时按下两键,进入正常工作模式
		{			
			Test_Mode_Dis_Jrp_Ctl = 1; //是从测试模式进入正常模式,即显示加热盘控制的数据 	
			Work_State = UI_STATE_POST_MODE;
			HmiEnterToWorkStateFunc(UI_STATE_POST_MODE);			
		}		
	}
	else
	{
		//do nothing
	}
	Key_State_Present = KEY_STATE_NONE;	//清除按键
}  

//-----------POST界面
void HmiPostFunc_Tik_Cnt(void)
{
	if(Tik_POST_Tick_100mS < (u8)250)
	{
		Tik_POST_Tick_100mS++;
	}
}

void	HmiPostFunc(void)//GUI --- 
{	
	static uint8_t Post_Step=0;
	uint8_t i;

	//LCD_LIGHT_OPEN;//开背光		
	if(Post_Step == (u8)0)//RTC自检1
	{
		//LCD_ShowString(220,20,16,200,"POST_STEP0!",0,RED18);
		i = RX8010_Initialize();//RTC初始化	
		if(i == (u8)RX8010_INIT_FAIL)//RTC初始化失效
		{
			Post_Step = 255;//FAIL,需重新开机或送修
			LCD_LIGHT_CLOSE;//背光
			Draw_Rectangle_Real(0,0,239,319,WHITE18);
			LCD_ShowString(180,20,16,200,(const u8*)"RTC Initialize Fail!",0,RED18);
			LCD_ShowString(140,40,16,300,(const u8*)"Please restart,if the problem",0,RED18);
			LCD_ShowString(120,20,16,300,(const u8*)"reproduce,contact to the supplier!",0,RED18);
			WDT_CONTR = 0x3F; 
			LCD_LIGHT_OPEN;//背光
		}
		else if(i == (u8)RX8010_INIT_OK)//RTC未初始化，则自动初始化成功
		{
			Post_Step = 1;//
			Tik_POST_Tick_100mS = 0;
			LCD_LIGHT_CLOSE;//背光
			Draw_Rectangle_Real(0,0,239,319,WHITE18);
			LCD_ShowString(220,20,16,200,(const u8*)"RTC Initialize OK!",0,RED18);
			LCD_LIGHT_OPEN;//开背光			
			Draw_Rectangle_Real(0,0,239,319,WHITE18);
		}
		else
		{
			Post_Step = 2;//
		}
	}
	else if(Post_Step == (u8)1)//RTC自检2-自动初始化,等待2S
	{
		//LCD_ShowString(190,20,16,200,"POST STEP1!",0,RED18);
		if(Tik_POST_Tick_100mS > (u8)20)
		{
			Post_Step = 2;
		}		
	}
	else if(Post_Step == (u8)2)//FLASH自检1
	{
		//LCD_ShowString(160,20,16,200,"POST STEP2!",0,RED18);		
		SPI_Read_nBytes(0x100000,2,SaveData); 
		if((SaveData[0] == (u8)'O')&&(SaveData[1] == (u8)'K'))//校验成功
		{
			Post_Step = 4;
		}
		else
		{
			Post_Step = 3;
			SPI_Erase_Sector(0x100000);//1M字节起启扇区擦除
		}
	}
	else if(Post_Step == (u8)3)//FLASH自检2
	{
		//LCD_ShowString(130,20,16,200,"POST STEP3!",0,RED18);		
		SaveData[0] = (u8)'O';
		SaveData[1] = (u8)'K'; 
		SPI_Write_nBytes(0x100000,2,SaveData);
		delay_ms(10);
		SaveData[0] = 0xff;
		SaveData[1] = 0xff;
		SPI_Read_nBytes(0x100000,2,SaveData); 
			
		if((SaveData[0] == (u8)'O')&&(SaveData[1] == (u8)'K'))
		{								 
			Post_Step = 4;//OK
		}
		else
		{
			Post_Step = 255;//FAIL,需重新开机或送修				
			LCD_ShowString(180,20,16,200,(const u8*)"Memory Initialize Fail!",0,RED18);
			LCD_ShowString(140,40,16,300,(const u8*)"Please restart,if the problem",0,RED18);
			LCD_ShowString(120,20,16,300,(const u8*)"reproduce,contact to the supplier!",0,RED18);
		}			
	}
	else if(Post_Step == (u8)4)//HP SENSOR-2
	{
		//LCD_ShowString(100,20,16,200,"POST STEP4!",0,RED18);		
		Tik_POST_Tick_100mS = 0;
		Post_Step = 5;
	}
	else if(Post_Step == (u8)5)//HP SENSOR-2
	{
		//LCD_ShowString(70,20,16,200,"POST STEP5!",0,RED18);	
		//LCD_ShowxNum(130,70,16,3,No_HeatSensor_Times,0x80,BLACK18); //
		if(Tik_POST_Tick_100mS > (u8)10)//延时1S
		{
			if(No_HeatSensor_Times>(u8)25)//加热盘传感器错误检测,1S可以检测到50次
			{
				Post_Step = 6;
				Tik_POST_Tick_100mS = 0;
			}	
			else
			{
				Post_Step = 7;//PASS		
			}
		}
	}
	else if(Post_Step == (u8)6)//HP SENSOR-3 检测两次
	{
		//LCD_ShowString(40,20,16,200,"POST STEP6!",0,RED18);
		//LCD_ShowxNum(130,150,16,3,No_HeatSensor_Times,0x80,BLACK18); //发热盘加热的模式显示		
		if(Tik_POST_Tick_100mS > (u8)30)//延时3S
		{
			if(No_HeatSensor_Times>(u8)100)//加热盘传感器错误检测 4S检测到200次
			{
				Post_Step = 255;
				LCD_ShowString(180,20,16,200,(const u8*)"Sensor Initialize Fail!",0,RED18);
				LCD_ShowString(140,40,16,300,(const u8*)"Please restart,if the problem",0,RED18);
				LCD_ShowString(120,20,16,300,(const u8*)"reproduce,contact to the supplier!",0,RED18);				 
			}
			else
			{
				Post_Step = 7;//PASS		
			}
		}
	}
	else if(Post_Step == (u8)7)//
	{
		//LCD_ShowString(10,20,16,200,"POST_STEP7!",0,RED18);		
		Work_State = UI_STATE_FACTORY_DEFAULT_SEL_MODE;
		HmiEnterToWorkStateFunc(UI_STATE_FACTORY_DEFAULT_SEL_MODE);
	}
	else if(Post_Step == (u8)255)//FAIL
	{
		LCD_LIGHT_OPEN;	
	}
	else
	{
		//do nothing
	}
}


//-----------开机选择界面
static uint8_t Tik_HmiFac_Tick_100mS_OK_flash = 0;//OK闪烁读数
void HmiFactoryDefault_Tik_Cnt(void)
{
	if(Tik_HmiFac_Tick_100mS_10S < (u8)250)
	{
		Tik_HmiFac_Tick_100mS_10S++;
	}
	if(Tik_HmiFac_Tick_100mS_OK_flash <(u8)250 )
	{
		Tik_HmiFac_Tick_100mS_OK_flash++;
	}
}

void	HmiFactoryDefaultFunc(void)//GUI --- 
{
	uint16_t Pre_Disp_Deg_10;//上一次设定温度个位 显示
	uint16_t Pre_Disp_Deg_1;//上一次设定温度小数 显示
  uint16_t Pre_Disp_Deg_CQK_10;//上一次设定温度个位 显示
	uint16_t Pre_Disp_Deg_CQK_1;//上一次设定温度小数 显示

	if(Tik_HmiFac_Tick_100mS_10S >= (u8)100)//10S
	{
		OK_not_Pressed_flag = 1;
	}		
	
	if(OK_not_Pressed_flag!=(uint8_t)0)
	{			
		if(Tik_HmiFac_Tick_100mS_OK_flash == (u8)1)
		{
			
		}			
		else if(Tik_HmiFac_Tick_100mS_OK_flash == (u8)5)
		{

		}			
		else if(Tik_HmiFac_Tick_100mS_OK_flash == (u8)10)
		{
			Back_Color=WHITE18;
			DISP_ICO_40X40(15,200,2,BLUE18);//显示OK图标	
			Back_Color=WHITE18;					
		}
		else if(Tik_HmiFac_Tick_100mS_OK_flash >= (u8)20)//0.5Hz的闪烁频率
		{
			Tik_HmiFac_Tick_100mS_OK_flash = 0;				
			Back_Color=WHITE18;	
			DISP_ICO_40X40(15,200,2,RED18);//显示OK图标	
			Back_Color=WHITE18;
			Sound_Short();
		}
		else
		{
			//do nothing
		}
	}
				
//以下检测KEY 
	if((Key_State_Present == KEY_STATE_Down_Short)//向下键到工厂设定
		||(Key_State_Present == KEY_STATE_Down_Long_First))

	{
		if(defalut_mode == (u8)Load_User_Pre_MODE)
		{
			HmiFac_Mode_Changed = 1;				 
			defalut_mode = Load_Fac_MODE;  
		}
	}	
	else if((Key_State_Present == KEY_STATE_UP_Short)
		||(Key_State_Present == KEY_STATE_UP_Long_First))
	{
		if(defalut_mode == (u8)Load_Fac_MODE)
		{
			HmiFac_Mode_Changed = 1;	
			defalut_mode = Load_User_Pre_MODE;
		}
	}
	else if((Key_State_Present == KEY_STATE_OK_Short)//进入开机有创无创选择界面或直接进去开机运行界面
			 ||(Key_State_Present ==KEY_STATE_OK_Long_First))
	{
		Load_Settings_Before_Choice();//在选择方框出现之前，要处理的数据，以同时兼容有无选择记忆界面程序				 
		HmiFac_Mode_Changed = 0;	
		LCD_LIGHT_CLOSE;			
		if(defalut_mode == (u8)Load_Fac_MODE)//默许设定
		{					 
			Work_State = UI_STATE_NON_INVASIVE_MODE;
			HmiEnterToWorkStateFunc(UI_STATE_NON_INVASIVE_MODE);
		}
		else//上一次的设定
		{
			Work_State = UI_STATE_RUNNING_NORMAL_MODE;
			HmiEnterToWorkStateFunc(UI_STATE_RUNNING_NORMAL_MODE);					 
		}				 
	}
	else
	{
		//do nothing
	}

	if(HmiFac_Mode_Changed!=(uint8_t)0)
	{
		HmiFac_Mode_Changed = 0;					
				
		data_flash.Language = Lan_English;//非FANEM只有英语

		
		if(data_flash.Work_Mode==(u8)Noninvasive_Mode)	 	//计算要显示的数据	
		{						
			Pre_Disp_Deg_10 = data_flash.Set_RT_WCTemp * (u16)5 / (u16)10;//整数位
			Pre_Disp_Deg_1 = data_flash.Set_RT_WCTemp * (u16)5 % (u16)10;//若是0则无小数,是5则显示5
			
			Pre_Disp_Deg_CQK_10 = data_flash.Set_CQK_WCTemp * (u16)5 / (u16)10;//整数位
			Pre_Disp_Deg_CQK_1 = data_flash.Set_CQK_WCTemp * (u16)5 % (u16)10;//若是0则无小数,是5则显示5
		}
		else
		{
			Pre_Disp_Deg_10 = data_flash.Set_RT_YCTemp * (u16)5 / (u16)10;//整数位
			Pre_Disp_Deg_1 = data_flash.Set_RT_YCTemp * (u16)5 % (u16)10;//若是0则无小数,是5则显示5
			
			Pre_Disp_Deg_CQK_10 = data_flash.Set_CQK_YCTemp * (u16)5 / (u16)10;//整数位
			Pre_Disp_Deg_CQK_1 = data_flash.Set_CQK_YCTemp * (u16)5 % (u16)10;//若是0则无小数,是5则显示5
		}	
					
		if(defalut_mode == (u8)Load_User_Pre_MODE)  //用户上一次的设定   
		{ 	  //画红色框
			Draw_Rectangle(Load_Fac_Set_X-10,Load_User_Pre_Set_Y-10,Load_Fac_Set_X+34,Load_User_Pre_Set_Y+242,WHITE18,4);
			Draw_Rectangle(Load_User_Pre_Set_X-10,Load_User_Pre_Set_Y-10,Load_User_Pre_Set_X+34,Load_User_Pre_Set_Y+242,RED18,4);//
			
			Draw_Rectangle_Real(83,Load_User_Pre_Set_Y-30,84,Load_User_Pre_Set_Y-10,WHITE18);//
			Draw_Rectangle_Real(83,Load_User_Pre_Set_Y-30,144,Load_User_Pre_Set_Y-29,WHITE18);// 
		 
			Draw_Rectangle_Real(203,Load_User_Pre_Set_Y-30,204,Load_User_Pre_Set_Y-10,RED18);//
			Draw_Rectangle_Real(143,Load_User_Pre_Set_Y-30,204,Load_User_Pre_Set_Y-29,RED18);//		
			
			if(data_flash.Work_Mode==(u8)Noninvasive_Mode)	 	//
			{
				CHAR str_modeNoinvasive[]="Mode:Noninvasive      ";
				strcpy((CHAR*)show_str,str_modeNoinvasive);				
				LCD_ShowString(Load_User_Pre_Set_X - 36,Load_User_Pre_Set_Y,16,280,(const u8*)show_str,0,BLUE18);
			}
			else
			{
				CHAR str_modeInvasive[]="Mode:Invasive         ";
				strcpy((CHAR*)show_str,str_modeInvasive);
				LCD_ShowString(Load_User_Pre_Set_X - 36,Load_User_Pre_Set_Y,16,280,(const u8*)show_str,0,BLUE18);
			}
			
			LCD_ShowxNum(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*8),16,2,Pre_Disp_Deg_10,0x00,BLUE18);//患者端温度
			if(Pre_Disp_Deg_1 == (u16)0)//无小数
			{
				LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*11),16,280,(const u8*)"C  ",0,BLUE18);//
				Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*10),2,BLUE18);//温度图标
			}
			else//有小数
			{
				LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*10),16,280,(const u8*)".5",0,BLUE18);//.5
				LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*13),16,280,(const u8*)"C ",0,BLUE18);//
				Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*12),2,BLUE18);//温度图标								
			}	

			LCD_ShowxNum(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*8)+(8*15),16,2,Pre_Disp_Deg_CQK_10,0x00,BLUE18);//出气口端温度
			if(Pre_Disp_Deg_CQK_1 == (u16)0)//无小数
			{
				LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*11)+(8*15),16,280,(const u8*)"C  ",0,BLUE18);//
				Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*10)+(8*15),2,BLUE18);//温度图标
			}
			else//有小数
			{
				LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*10)+(8*15),16,280,(const u8*)".5",0,BLUE18);//.5
				LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*13)+(8*15),16,280,(const u8*)"C ",0,BLUE18);//
				Draw_Circle((Load_User_Pre_Set_X - 56) +12,(Load_User_Pre_Set_Y+4+(8*12))+(8*15),2,BLUE18);//温度图标								
			}								
			
			if(data_flash.In_Exp_Ratio == (u8)1)//1:1
			{
				LCD_ShowString(Load_User_Pre_Set_X - 76,Load_User_Pre_Set_Y+(8*10),16,280,(const u8*)"  ",0,BLUE18);//1:1
			}
			else
			{
				LCD_ShowString(Load_User_Pre_Set_X - 76,Load_User_Pre_Set_Y+(8*10),16,280,(const u8*)".",0,BLUE18); 
				LCD_ShowxNum(Load_User_Pre_Set_X - 76,Load_User_Pre_Set_Y+(8*11),16,1,(u32)data_flash.In_Exp_Ratio-(u32)1,(u8)0x0,BLUE18);//
			}							       	  
		}
		else  //工厂设定
		{
			CHAR str_invasive[]="Invasive         ";
			CHAR str_patient39C[]="Patient:39 C  ";
			CHAR str_chamber36C[]="Chamber:36 C  ";
			
			
			Draw_Rectangle(Load_Fac_Set_X-10,Load_User_Pre_Set_Y-10,Load_Fac_Set_X+34,Load_User_Pre_Set_Y+242,RED18,4);
			Draw_Rectangle(Load_User_Pre_Set_X-10,Load_User_Pre_Set_Y-10,Load_User_Pre_Set_X+34,Load_User_Pre_Set_Y+242,WHITE18,4);//
		 
			Draw_Rectangle_Real(203,Load_User_Pre_Set_Y-30,204,Load_User_Pre_Set_Y-10,WHITE18);//						 
			Draw_Rectangle_Real(143,Load_User_Pre_Set_Y-30,204,Load_User_Pre_Set_Y-29,WHITE18);//
		 
			Draw_Rectangle_Real(83,Load_User_Pre_Set_Y-30,84,Load_User_Pre_Set_Y-10,RED18);//
			Draw_Rectangle_Real(83,Load_User_Pre_Set_Y-30,144,Load_User_Pre_Set_Y-29,RED18);//	
			strcpy((CHAR*)show_str,str_invasive);
			LCD_ShowString(Load_User_Pre_Set_X - 36,Load_User_Pre_Set_Y+(8*5),16,280,(const u8*)show_str,0,BLUE18);//有创					
			strcpy((CHAR*)show_str,str_patient39C);				
			LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y,16,280,(const u8*)show_str,0,BLUE18);
			Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*10),2,BLUE18);//温度图标	
			strcpy((CHAR*)show_str,str_chamber36C);						
			LCD_ShowString(Load_User_Pre_Set_X - 56,Load_User_Pre_Set_Y+(8*15),16,280,show_str,0,BLUE18);
			Draw_Circle((Load_User_Pre_Set_X - 56) +12,Load_User_Pre_Set_Y+4+(8*10)+(8*15),2,BLUE18);//温度图标
			LCD_ShowString(Load_User_Pre_Set_X - 76,Load_User_Pre_Set_Y+(8*10),16,280,(const u8*)".3",0,BLUE18);//1:1.3 
		}
		LCD_LIGHT_OPEN; 
	}	
	Key_State_Present = KEY_STATE_NONE;	//清除按键
}

//-----------开机有创无创选择界面

void HmiNon_InvasiveSel_Tik_Cnt(void)
{
	if(Tik_HmiNon_InvasiveSel_Tick_100mS < (u8)250)
	{
		Tik_HmiNon_InvasiveSel_Tick_100mS++;
	}
}

void	HmiNon_InvasiveSelFunc(void)//有创无创选择界面
{
	WDT_CONTR = 0x3F; //开启WDT,在22.1184时时间为4.55S	
	{
		if((Key_State_Present == KEY_STATE_UP_Short)//
				 ||(Key_State_Present ==KEY_STATE_UP_Long_First))
		{

			if(Work_Mode==(u8)0)
			{
				Work_Mode=1;
			}
			else
			{
				Work_Mode=0;  
			}

			if(Work_Mode==(u8)0)  //无创模式
			{ 
				Draw_Rectangle(75,182,162,262,RED18,4);//
				Draw_Rectangle(75,58,162,136,WHITE18,4);
			}else  //无创模式
			{	
				Draw_Rectangle(75,182,162,262,WHITE18,4);
				Draw_Rectangle(75,58,162,136,RED18,4);
			}

		}
		else if((Key_State_Present == KEY_STATE_OK_Short)//进入运行状态
				 ||(Key_State_Present ==KEY_STATE_OK_Long_First)
				 ||(Tik_HmiNon_InvasiveSel_Tick_100mS > (u8)30))

		{
		 LCD_LIGHT_CLOSE;		 
		 Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		 HmiEnterToWorkStateFunc(UI_STATE_RUNNING_NORMAL_MODE);
		}
		else
		{
			//do nothing
		}
  }	
	data_flash.Work_Mode = Work_Mode;//选定的模式

	Setting_write_to_flash();//写入到FLASH		

	Key_State_Present = KEY_STATE_NONE;	//清除按键	
}


//-----------运行界面
DISPLAY_Temp_Kind Display_Temp_Kind = DISPLAY_Temperature_Patient;     //显示温度类型 0-患者端温度 1-出气口温度

void	HmiRunningFunc(void)//
{
	static uint8_t Display_Return_Cnt = 0;
	if((Key_State_Present == KEY_STATE_UP_Short)//切换显示
		||(Key_State_Present == KEY_STATE_UP_Long_First))
	{
		Display_Return_Cnt = 0;
		if(Display_Temp_Kind == DISPLAY_Temperature_Patient)
		{
			Display_Temp_Kind = DISPLAY_Temperature_Chamber;
			DISP_ICO_52X64(POS_ICO_TEMP_X,POS_ICO_TEMP_Y,1,BLACK18);
			Display_In_Exp_Ratio(In_Exp_Ratio,1);			
		}	
		else	
		{
			Display_Temp_Kind = DISPLAY_Temperature_Patient;
			DISP_ICO_52X64(POS_ICO_TEMP_X,POS_ICO_TEMP_Y,0,BLACK18);
			Display_In_Exp_Ratio(In_Exp_Ratio,0);
		}	
		RefreshTempHumidyFunc(1);		
	}
	else if(Key_State_Present == KEY_STATE_Down_Long_First)//改变模式
	{
		Working_Normal = 0;//工作时间清零
		Sound_Short();		
		Set_RT_WCTemp = 340;// 切换模式即温度设定回复出厂设定
		Set_RT_YCTemp = 390;//
		Set_CQK_WCTemp = 310;//
		Set_CQK_YCTemp = 360;//

		RT_Temp_Reach_Set_Cnt = 0;//人体端达到温度计数清零
		CQK_Temp_Reach_Set_Cnt = 0;

		if(Work_Mode==(u8)Invasive_Mode)	
		{ 
			Work_Mode=Noninvasive_Mode;	
			Set_RT_Temp=(INT)Set_RT_WCTemp;	
			Set_CQK_Temp=Set_CQK_WCTemp;		
		}
		else
		{
			Work_Mode=Invasive_Mode;	
			Set_RT_Temp=(INT)Set_RT_YCTemp;		
			Set_CQK_Temp=Set_CQK_YCTemp;				
		}
		data_flash.Set_RT_WCTemp = (uint8_t)(Set_RT_WCTemp/(u16)5);//
		data_flash.Set_RT_YCTemp = (uint8_t)(Set_RT_YCTemp/(u16)5);//
		data_flash.Set_CQK_WCTemp = (uint8_t)(Set_CQK_WCTemp/(u16)5);//
		data_flash.Set_CQK_YCTemp = (uint8_t)(Set_CQK_YCTemp/(u16)5);//
		data_flash.Work_Mode = Work_Mode;// 
		Setting_write_to_flash(); 
		Refresh_Work_Mode();//切换图标			  
	}
	else if(Key_State_Present == KEY_STATE_OK_Long_First)//长按OK进入设定状态
	{
		Sound_Short();
		if(Display_Temp_Kind == DISPLAY_Temperature_Patient)
		{
			Work_State = UI_STATE_SetTempPatient_MODE;//设定患者端温度
			HmiEnterToWorkStateFunc(UI_STATE_SetTempPatient_MODE);	
			Remeber_Temp_Value= (uint16_t)Set_RT_Temp;
		}
		else
		{
			Work_State = UI_STATE_SetTempChamber_MODE;//设定出气口温度
			HmiEnterToWorkStateFunc(UI_STATE_SetTempChamber_MODE);
			Remeber_Temp_Value = Set_CQK_Temp;			
		}
	}	
	else if(Key_State_Present == KEY_STATE_Mute_Long_First)//长按MUTE进入时间设定状态
	{
		Sound_Short();
		if(Display_Temp_Kind == DISPLAY_Temperature_Patient)
		{
			Work_State = UI_STATE_SetTime_MODE;//设定日期和时间
			HmiEnterToWorkStateFunc(UI_STATE_SetTime_MODE);	

		}
	}
	else if(Key_State_Present == KEY_STATE_OK_Short)//短按OK键清色块标记
	{	
		Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+45,WHITE18);
	  WireInOut_State_Confirm();//正常运行界面,按下确认键确定回路的模式		
	}
	else
	{
		//do nothing
	}

  //以下为10S后自动返回	
	if((uint16_t)Key_State_Present!=(uint16_t)0)
	{
		Display_Return_Cnt = 0;
	}
	if(Display_Return_Cnt < (u8)90)
	{
		Display_Return_Cnt++;
	}
	else
	{
		if(Display_Temp_Kind == DISPLAY_Temperature_Chamber)
		{
			Back_Color=WHITE18;
			Display_Temp_Kind = DISPLAY_Temperature_Patient;	
			DISP_ICO_52X64(POS_ICO_TEMP_X,POS_ICO_TEMP_Y,0,BLACK18);
			Display_In_Exp_Ratio(In_Exp_Ratio,0);
		}			
	}
	Key_State_Present = KEY_STATE_NONE;	//清除按键	
}


//-----------患者端温度设置界面
void	HmiSetTempPatientFunc(void)//GUI --- 
{
	static uint8_t Set_Temp_Cnt=0; 
	static uint8_t Display_Return_Cnt = 0;
	Set_Temp_Cnt++;
	if(Set_Temp_Cnt > (u8)10)
	{
		Set_Temp_Cnt = 0;
 	}
	Back_Color=WHITE18;
	if(Set_Temp_Cnt == (u8)0)
	{
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
	}
	else if(Set_Temp_Cnt == (u8)5)
	{
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,WHITE18);
	}
	else
	{
		//do nothing
	}
		
	
	if((Key_State_Present == KEY_STATE_UP_Short)//增加
			||(Key_State_Present == KEY_STATE_UP_Long_First)
			||(Key_State_Present == KEY_STATE_UP_Long_Repeat))
	{	
		if(Work_Mode==(u8)Noninvasive_Mode)
		{
			if(Remeber_Temp_Value<(u16)Const_NoninvasPatientTemp_Max)
			{
				Remeber_Temp_Value=Remeber_Temp_Value+(u16)5;
			} 
		}
		else
		{
			if(Remeber_Temp_Value<(u16)Const_InvasPatientTemp_Max)
			{
				Remeber_Temp_Value=Remeber_Temp_Value+(u16)5;
			}	 
		}	
		Display_SET_Temp(Remeber_Temp_Value);				
	}
	else if((Key_State_Present == KEY_STATE_Down_Short)//减小
				 ||(Key_State_Present == KEY_STATE_Down_Long_First)
				 ||(Key_State_Present == KEY_STATE_Down_Long_Repeat))
	{	
		if(Work_Mode==(u8)Noninvasive_Mode)  //无创
		{
			if(Remeber_Temp_Value>(u16)Const_NoninvasPatientTemp_Min)
			{
				Remeber_Temp_Value=Remeber_Temp_Value-(u16)5;
			} 
		} 
		else
		{
			if(Remeber_Temp_Value>(u16)Const_InvasPatientTemp_Min)
			{
				Remeber_Temp_Value=Remeber_Temp_Value-(u16)5;
			}
		}
		Display_SET_Temp(Remeber_Temp_Value);		
	}
	else if(Key_State_Present == KEY_STATE_Mute_Short)//MUTE
	{		
		Work_State = UI_STATE_SetInExp_MODE;
		HmiEnterToWorkStateFunc(UI_STATE_SetInExp_MODE);
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
		RefreshTempHumidyFunc(1);//强制刷新温温度显示	
		Display_Return_Cnt = 0;		
	}
	else if(Key_State_Present == KEY_STATE_OK_Short)//确认
	{
		Sound_Short();
		if(Set_RT_Temp != (INT)Remeber_Temp_Value)//若未改变人体端温度设定则不进行操作
		{
			Working_Normal = 0;//工作时间清零
			Set_RT_Temp=(INT)Remeber_Temp_Value;
			if(Work_Mode == (u8)Noninvasive_Mode)
			{
				Set_RT_WCTemp=(uint16_t)Set_RT_Temp;
				data_flash.Set_RT_WCTemp = (uint8_t)(Set_RT_WCTemp/(u16)5);//无创设定温度 
				
				if(Set_RT_WCTemp >= (u16)320) 
				{
					Set_CQK_WCTemp = 310;//人体端>=32时出气口温度为31
				}
				else
				{
					Set_CQK_WCTemp = 300;//人体端>=32时出气口温度为31	
				}					
				data_flash.Set_CQK_WCTemp = (uint8_t)(Set_CQK_WCTemp/(u16)5);														
				Set_CQK_Temp = (uint8_t)Set_CQK_WCTemp;	
			}
			else
			{
				Set_RT_YCTemp=(uint16_t)Set_RT_Temp; 
				data_flash.Set_RT_YCTemp = (uint8_t)(Set_RT_YCTemp/(u16)5);//有创设定温度 
				
				if(Set_RT_YCTemp >= (u16)370) 
				{
					Set_CQK_YCTemp = Set_RT_YCTemp - (u16)30;//人体端>=37时出气口温度为RT-3
				}
				else
				{
					Set_CQK_YCTemp = 340; //有创出气口设定温度 
				}
				data_flash.Set_CQK_YCTemp = (uint8_t)(Set_CQK_YCTemp/(u16)5);
				Set_CQK_Temp = Set_CQK_YCTemp;													
			}
		}		
		
		Setting_write_to_flash(); 
		Display_Return_Cnt = 0;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
		RefreshTempHumidyFunc(1);//强制刷新温温度显示		
	}
	else
	{
		//do nothing
	}
	
	
	//以下为10S后自动返回	
	if((uint16_t)Key_State_Present!=(uint16_t)0)
	{
		Display_Return_Cnt = 0;
	}
	if(Display_Return_Cnt < (u8)90)
	{
		Display_Return_Cnt++;
	}
	else
	{
		Display_Return_Cnt = 0;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
		RefreshTempHumidyFunc(1);//强制刷新温温度显示		
	}
	Key_State_Present = KEY_STATE_NONE;	//清除按键	
}

//-----------出气口温度设置界面
void	HmiSetTempChamberFunc(void)//GUI
{
	static uint8_t Set_Temp_Cnt=0; 
	static uint8_t Display_Return_Cnt = 0;
	Set_Temp_Cnt++;
	if(Set_Temp_Cnt > (u8)10)
	{
		Set_Temp_Cnt = 0;
 	}
	Back_Color=WHITE18;
	if(Set_Temp_Cnt == (u8)0)
	{
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
	}	
	else if(Set_Temp_Cnt == (u8)5)
	{
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,WHITE18);
	}
	else
	{
		//do nothing
	}
		
	
	if((Key_State_Present == KEY_STATE_UP_Short)//增加
			||(Key_State_Present == KEY_STATE_UP_Long_First)
			||(Key_State_Present == KEY_STATE_UP_Long_Repeat))
	{	
		if(Work_Mode==(u8)Noninvasive_Mode)//无创
		{//出气口温度最大32度
			if(Remeber_Temp_Value<(uint16_t)Const_NoninvasChamberTemp_Max)
			{
				Remeber_Temp_Value=Remeber_Temp_Value+(u16)5;
			} 
		}
		else if(Work_Mode==(uint8_t)Invasive_Mode)
		{//出气口和患者端温度差为-4/+3
			if((Remeber_Temp_Value<(u16)Const_InvasChamberTemp_Max)&&(Remeber_Temp_Value<((uint16_t)Set_RT_Temp+(u16)30)))
			{
				Remeber_Temp_Value=Remeber_Temp_Value+(u16)5;
			}
		}	
		else
		{
			//do nothing
		}
		Display_SET_Temp(Remeber_Temp_Value);				
	}
	else if((Key_State_Present == KEY_STATE_Down_Short)//减小
				 ||(Key_State_Present == KEY_STATE_Down_Long_First)
				 ||(Key_State_Present == KEY_STATE_Down_Long_Repeat))
	{	
		if(Work_Mode==(u8)Noninvasive_Mode)  //无创
		{//最小30度
			if(Remeber_Temp_Value>(u16)Const_NoninvasChamberTemp_Min)
			{
				Remeber_Temp_Value=Remeber_Temp_Value-(u16)5;
			} 
		} 
		else if(Work_Mode==(u8)Invasive_Mode) //有创 出气口和患者端温度差为-4/+3 //最小34度
		{
			if((Remeber_Temp_Value>(u16)Const_InvasChamberTemp_Min)&&(Remeber_Temp_Value>((uint16_t)Set_RT_Temp-(u16)40)))
			{
				Remeber_Temp_Value=Remeber_Temp_Value-(u16)5;
			}
		}	
		else
		{
			//do nothing
		}
		Display_SET_Temp(Remeber_Temp_Value);		
	}
	else if(Key_State_Present == KEY_STATE_OK_Short)//确认
	{
		Sound_Short();
		if(Set_CQK_Temp != Remeber_Temp_Value)//若未改变人体端温度设定则不进行操作
		{
			Working_Normal = 0;//工作时间清零
			Set_CQK_Temp=Remeber_Temp_Value;
			if(Work_Mode==(u8)Noninvasive_Mode)
			{
				Set_CQK_WCTemp=Set_CQK_Temp;
				data_flash.Set_CQK_WCTemp = (uint8_t)(Set_CQK_WCTemp/(u16)5);//无创设定温度 																		
			}
			else
			{
				Set_CQK_YCTemp=Set_CQK_Temp; 
				data_flash.Set_CQK_YCTemp = (uint8_t)(Set_CQK_YCTemp/(u16)5);//有创设定温度 																					
			}	
			
			Display_Return_Cnt = 0;
			Work_State = UI_STATE_RUNNING_NORMAL_MODE;
			DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
			RefreshTempHumidyFunc(1);//强制刷新温温度显示	
		}		
	}
	else
	{
		//do nothing
	}
	
	Setting_write_to_flash(); 
		
	//以下为10S后自动返回	
	if((uint16_t)Key_State_Present!=(uint16_t)0)
	{
		Display_Return_Cnt = 0;
	}
	if(Display_Return_Cnt < (u8)90)
	{
		Display_Return_Cnt++;
	}
	else
	{
		Display_Return_Cnt = 0;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+(34*3)+5,ICO_DU,BLACK18);
		RefreshTempHumidyFunc(1);//强制刷新温温度显示		
	}
	Key_State_Present = KEY_STATE_NONE;	//清除按键	
}


static uint8_t SetTimeKind = 2;//2-7//年/月/日/时/分/秒
static uint8_t TempTime[7];

static void EnterSetTime(void)//进入时间设定
{
	SetTimeKind = 2;
	RX8010_GetTime(TempTime);	
//	Set_Hour_Value
}

static void  Date_Is_Correct(void)
{
	uint16_t temp1;
	u8  Temp_Bit1;
	//将BCD码转化为10进制,设定时间从2000年到2099年======================================
	Temp_Bit1=(u8)0;
	temp1=(((u16)TempTime[6]& (u16)0x0F)%(u16)10)+ ((((u16)TempTime[6]>>(u16)4)%(u16)10)*(u16)10);
	//能被4整除，但是不能被100整除为闰年，或者能被400整除=====
	if((temp1%(u8)4)==(u8)0 )
	{
		Temp_Bit1=1;
	}	
	if((TempTime[4]==(u8)0x04) || (TempTime[4]==(u8)0x06) || (TempTime[4]==(u8)0x09) || (TempTime[4]==(u8)0x11))
	{
		if(TempTime[3]>(u8)0x30) 
		{
			TempTime[3]=0x30;
		}
	}
	if(Temp_Bit1!=(u8)0) //闰年29天，平年28天
	{
		if(TempTime[4]==(u8)2)
		{
			if(TempTime[3]>(u8)0x29)
			{
				TempTime[3]=0x29;
			}				
		}
	}else
	{
		if(TempTime[4]==(u8)2)
		{
			if(TempTime[3]>(u8)0x28)
			{
				TempTime[3]=0x28;
			}				
		}
	}	
}

//-----------日期时间设置界面
void	HmiSetTimeFunc(void)
{
	static uint8_t Display_Return_Cnt = 0;
	static uint8_t Set_Time_Cnt=0; 
//	uint8_t i;
//	uint8_t color;
	
	Set_Time_Cnt++;
	if(Set_Time_Cnt > (u8)6)
	{
		Set_Time_Cnt = 0;
 	}
	
	if(Key_State_Present == KEY_STATE_Mute_Short)//MUTE
	{
		SetTimeKind++;
		
		Back_Color=WHITE18;	
//		color=BLACK18;		
		DisPlayTime(TempTime,2); //显示时间
		DisPlayTime(TempTime,3); //显示时间
		DisPlayTime(TempTime,4); //显示时间
		DisPlayTime(TempTime,5); //显示时间
		DisPlayTime(TempTime,6); //显示时间
		DisPlayTime(TempTime,7); //显示时间
		
		if(SetTimeKind > (u8)7)
		{
			SetTimeKind = 2;
			Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		}		
	}	
	else if((Key_State_Present == KEY_STATE_UP_Short)//UP
				  ||(Key_State_Present == KEY_STATE_UP_Long_First)
					||(Key_State_Present == KEY_STATE_UP_Long_Repeat))	
	{
		Set_Time_Cnt = 0;//强制刷新
		if(SetTimeKind==(u8)2) //设定年份
		{
			TempTime[6]++;
			if((TempTime[6]&(uint8_t)0x0F)>=(u8)10)
			{
					 TempTime[6]=(TempTime[6]& (uint8_t)0xF0)+(u8)0x10;;
			}
			if(TempTime[6]>(u8)0x99)
			{
						TempTime[6]=0x99;
			}
			Date_Is_Correct();	 	   	    

		}else if(SetTimeKind==(u8)3) //设定月份
		{
			TempTime[4]++;
			if((TempTime[4]&(uint8_t)0x0F)>=(u8)10)
			{
					 TempTime[4]=(TempTime[4]& (uint8_t)0xF0)+(u8)0x10;
			}
			if(TempTime[4]>(u8)0x12)
			{
						TempTime[4]=0x12;
			}
			Date_Is_Correct();
		}else if(SetTimeKind==(u8)4) //设定日
		{
			TempTime[3]++;
			if((TempTime[3]&(uint8_t)0x0F)>=(u8)10)
			{
					 TempTime[3]=(TempTime[3]& (uint8_t)0xF0)+(u8)0x10;
			}
			if(TempTime[3]>(u8)0x31)
			{
						TempTime[3]=0x31;
			}
			Date_Is_Correct();	
		}else if(SetTimeKind==(u8)5) //设定时
		{
			{
				TempTime[2]++;								
				if((TempTime[2]&(uint8_t)0x0F)>=(u8)10)
				{
						 TempTime[2]=(TempTime[2] & (uint8_t)0xF0)+(u8)0x10;
				}
				if(TempTime[2]>(u8)0x23)
				{
							TempTime[2]=0x23;	
				}
			}							
		}else if(SetTimeKind==(u8)6) //设定分
		{
			TempTime[1]++;
			if((TempTime[1]&(uint8_t)0x0F)>=(u8)10)
			{
					 TempTime[1]=(TempTime[1]& (uint8_t)0xF0)+(u8)0x10;
			}
			if(TempTime[1]>(u8)0x59)
			{
						TempTime[1]=0x59;
			}
		}else if(SetTimeKind==(u8)7) //设定秒
		{
			TempTime[0]++;
			if((TempTime[0]&(uint8_t)0x0F)>=(u8)10)
			{
					 TempTime[0]=(TempTime[0]& (uint8_t)0xF0)+(u8)0x10;
			}
			if(TempTime[0]>(u8)0x59)
			{
						TempTime[0]=0x59;
			}
		}
		else
		{
			//do nothing
		}
	}
	else if((Key_State_Present == KEY_STATE_Down_Short)//
			   ||(Key_State_Present == KEY_STATE_Down_Long_First)
				 ||(Key_State_Present == KEY_STATE_Down_Long_Repeat))
	{
		Set_Time_Cnt = 0;//强制刷新
		if((SetTimeKind==(u8)2) && (TempTime[6]>(u8)0x10)) //设定年份
		{
			TempTime[6]--;
			if((TempTime[6]&(uint8_t)0x0F)>(u8)9)
			{
				TempTime[6]&=(uint8_t)0xF9;
			}				
			Date_Is_Correct();	
		}else if((SetTimeKind==(u8)3) && (TempTime[4]>(u8)1)) //设定月份
		{
			TempTime[4]--;
			if((TempTime[4]&(uint8_t)0x0F)>(u8)9)
			{
				TempTime[4]&=(uint8_t)0xF9;
			}				
			Date_Is_Correct();	
		}else if((SetTimeKind==(u8)4) && (TempTime[3]>(u8)1)) //设定日
		{
			TempTime[3]--;
			if((TempTime[3]&(uint8_t)0x0F)>(u8)9)
			{
				TempTime[3]&=(uint8_t)0xF9;
			}				
		}else if((SetTimeKind==(u8)5) && (TempTime[2]>(u8)0))  //设定时
		{	
			{
				TempTime[2]--;
				if((TempTime[2]&(uint8_t)0x0F)>(u8)9)
				{
					TempTime[2]&=(uint8_t)0xF9;
				}					  
			}
		}else if((SetTimeKind==(u8)6)  && (TempTime[1]>(u8)0)) //设定分
		{
			TempTime[1]--;
			if((TempTime[1]&(uint8_t)0x0F)>(u8)9)
			{
				TempTime[1]&=(uint8_t)0xF9;
			}				
		}else if((SetTimeKind==(u8)7) && (TempTime[0]>(u8)0)) //设定秒
		{
			TempTime[0]--;
			if((TempTime[0]&(uint8_t)0x0F)>(u8)9)
			{
				TempTime[0]&=(uint8_t)0xF9;
			}				
		}
		else
		{
			//do nothing
		}
	}	
	else if(Key_State_Present == KEY_STATE_OK_Short)//
	{
		Sound_Short();
		RX8010_SetTime(TempTime);		
		Display_Return_Cnt = 0;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;		
	}	
	else
	{
		//do nothing
	}
	
//	color = BLACK18;
	if(Set_Time_Cnt == (u8)0)
	{
		Back_Color=GREEN18;
	}
	else if(Set_Time_Cnt == (u8)3)
	{
		Back_Color=WHITE18;	
	}
	else
	{
		//do nothing
	}
			
	
	if((Set_Time_Cnt == (u8)0) ||(Set_Time_Cnt == (u8)3))
	{
		DisPlayTime(TempTime,SetTimeKind); //显示时间
	 }
		
	//以下为10S后自动返回	
	if((uint16_t)Key_State_Present!=(uint16_t)0)
	{
		Display_Return_Cnt = 0;
	}
	if(Display_Return_Cnt < (u8)90)
	{
		Display_Return_Cnt++;
	}
	else
	{
		Display_Return_Cnt = 0;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;
//		DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+34*3+5,ICO_DU,BLACK18);
//		RefreshTempHumidyFunc(1);//强制刷新温温度显示		
	}
	
		
	Key_State_Present = KEY_STATE_NONE;	//清除按键		
}


//uint8_t  In_Exp_Ratio=4;	 //In和Exp加热方式的比例  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
static uint8_t  In_Exp_Ratio_temp=3;	 //In和Exp加热方式的比例临时变量
static void	EnterSetInExp(void)//进入InExp设定
{	
	In_Exp_Ratio_temp = In_Exp_Ratio;	
	Display_In_Exp_Ratio(In_Exp_Ratio_temp,1);	
}

void	HmiSetInExpFunc(void)//InExp设置界面
{
	static uint8_t Display_Return_Cnt = 0;
	static uint8_t Set_InExp_Cnt=0; 
	
	Set_InExp_Cnt++;
	if(Set_InExp_Cnt > (u8)6)
	{
		Set_InExp_Cnt = 0;
 	}
	Back_Color=WHITE18;
	if(Set_InExp_Cnt == (u8)0)
	{
		LCD_ShowString(POS_RT_RH_X ,POS_RT_RH_Y+120,16,200,(const u8*)"In/Exp:",0,BLACK18);
	}	
	else if(Set_InExp_Cnt == (u8)3)
	{
		LCD_ShowString(POS_RT_RH_X ,POS_RT_RH_Y+120,16,200,(const u8*)"In/Exp:",0,WHITE18);	
	}
	else
	{
		//do nothing
	}
			
	
	if(Key_State_Present == KEY_STATE_Mute_Short)//MUTE
	{
		Work_State = UI_STATE_SetTempPatient_MODE;
		HmiEnterToWorkStateFunc(UI_STATE_SetTempPatient_MODE);
		Display_In_Exp_Ratio(In_Exp_Ratio_temp,0);		
	}
	else if(Key_State_Present == KEY_STATE_UP_Short)//UP
	{
		if(In_Exp_Ratio_temp < (u8)6)
		{
			In_Exp_Ratio_temp++;					
		}
		Display_In_Exp_Ratio(In_Exp_Ratio_temp,1);
	}	
	else if(Key_State_Present == KEY_STATE_Down_Short)//DOWN
	{
		if(In_Exp_Ratio_temp > (u8)1)
		{
			In_Exp_Ratio_temp--;
		}
		Display_In_Exp_Ratio(In_Exp_Ratio_temp,1);
	}
	else if(Key_State_Present == KEY_STATE_OK_Short)//OK
	{
		Sound_Short();
		In_Exp_Ratio = In_Exp_Ratio_temp;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		Display_In_Exp_Ratio(In_Exp_Ratio_temp,0);	
		data_flash.In_Exp_Ratio = In_Exp_Ratio;// 
		Setting_write_to_flash(); 		
	}	
	else
	{
		//do nothing
	}
	
	//	Setting_write_to_flash(); 
		//以下为10S后自动返回	
	if((uint16_t)Key_State_Present!=(uint16_t)0)
	{
		Display_Return_Cnt = 0;
	}
	if(Display_Return_Cnt < (u8)90)
	{
		Display_Return_Cnt++;
	}
	else
	{
		Display_Return_Cnt = 0;
		Work_State = UI_STATE_RUNNING_NORMAL_MODE;
		Display_In_Exp_Ratio(In_Exp_Ratio,0);		
	}	
	
	Key_State_Present = KEY_STATE_NONE;	//清除按键		
}


//--------屏保模式
//计时函数,放入中断
void HmiScreenSaverMode_Tik_Cnt(void)
{
	if(Tik_ScreenSaver_Tick_100mS < (u16)30000)
	{
		Tik_ScreenSaver_Tick_100mS++;
	}
}

void	HmiScreenSaverModeFunc(void)
{
	uint16_t i=0;
	uint8_t DispEnable = 0;
	static uint8_t ReCnt=0;
	
//	DispEnable = 0;
	ReCnt++;
	if(ReCnt > (u8)10)//1S
	{
		ReCnt = 0;	
		if(Work_State == UI_STATE_SCREENSAVER_MODE)
		{
			DispEnable = 1;	
		}
	}		
	
	if(Work_State != UI_STATE_SCREENSAVER_MODE)//不在屏保模式
	{
	  if(Tik_ScreenSaver_Tick_100mS > (u16)3000)//5分钟后进入屏保模式
		{
			Work_State = UI_STATE_SCREENSAVER_MODE;
			LCD_LIGHT_CLOSE;
			Draw_Rectangle_Real(0,0,239,319,BLACK18);						
			DispEnable = 1;	
		}
	}
	else//在屏保模式
	{		
		//do nothing
	}	
	
	if(((uint16_t)Key_State_Present!=(uint16_t)0)//按下按键
	//或出现错误
				||((ERR_Kind)!=(u8)0) //无水,传感器错误,高温等错误
	      ||(Wire_Mode_Mismatch == (uint8_t)1)  //发热丝未选定
				||(HeaterPlate_State==(u8)0))//水罐未装好或发热盘开路
	{
		Key_State_Present = KEY_STATE_NONE;	//清除按键	
		Tik_ScreenSaver_Tick_100mS = 0;
		DispEnable = 0;
		
		if(Work_State == UI_STATE_SCREENSAVER_MODE)//在屏保模式
		{
			Work_State = UI_STATE_RUNNING_NORMAL_MODE;
			HmiEnterToWorkStateFunc(UI_STATE_RUNNING_NORMAL_MODE);	
			Err_Base_HeaterWire_DISP_Enable();//屏保时置相关标志,以便亮屏时显示主机和线的图形
		}
	}		
		
	if(DispEnable!=(uint8_t)0)
	{
//		DispEnable = 0;		
		Back_Color=BLACK18;
		i=Diplay_RTtemp;
		if(i>=(u16)1000)//显示温度
		{
			DISP_TEMP_38X64(POS_SCREEN_Y,POS_SCREEN_X,(u8)((i/(u16)1000)%(u16)10),GREEN18);  
			DISP_TEMP_38X64(POS_SCREEN_Y,POS_SCREEN_X+42,(u8)((i/(u16)1000)%(u16)10),GREEN18); 
			Draw_Rectangle_Real(POS_SCREEN_Y,POS_SCREEN_X+82,POS_SCREEN_Y+8,POS_SCREEN_X +88,BLACK18);       
			DISP_TEMP_38X64(POS_SCREEN_Y,POS_SCREEN_X +90,(u8)((i/(u16)1000)%(u16)10),GREEN18);    
			DISP_ICO_32X40(POS_SCREEN_Y,POS_SCREEN_X +133,ICO_DU,GREEN18);	
		}
		else
		{
			DISP_TEMP_38X64(POS_SCREEN_Y,POS_SCREEN_X,(u8)((i/(u16)100)%(u16)10),GREEN18);  
			DISP_TEMP_38X64(POS_SCREEN_Y,POS_SCREEN_X+42,(u8)((i%(u16)100)/(u16)10),GREEN18); 
			Draw_Rectangle_Real(POS_SCREEN_Y,POS_SCREEN_X+82,POS_SCREEN_Y+8,POS_SCREEN_X +88,GREEN18);       
			DISP_TEMP_38X64(POS_SCREEN_Y,POS_SCREEN_X +90,(u8)(i%(u16)10),GREEN18);    
			DISP_ICO_32X40(POS_SCREEN_Y,POS_SCREEN_X +133,ICO_DU,GREEN18);					   
		}	
		LCD_LIGHT_OPEN;  
	}
}




