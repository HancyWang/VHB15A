/* VHB15Aʪ����*/	

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
		
//������ͱ�����ر���	
uint8_t  ERR_Kind=0;//���������//0������/1����/2����/3��ʪ/4��ˮ/5���ߴ�����/6�����ڴ�����/7�����̴�����   
bit Wire_Mode_Mismatch ; //��·����˿ģʽ��ʵ�ʲ�ƥ��		
static bit JRP_Sensor_Err;
static bit CQK_Sensor_Err;
static bit RTD_Sensor_Err;
static bit SHIDU_Sensor_Err;		
		
uint8_t AlarmSoundPauseStatus = 0;//����������ͣ 0-�������� 1-����������ͣ(������ʱ��Ч)
uint8_t AlarmInfoIndex = 0;//
          
static uint8_t  xdata  HiTemp_Count=0;	  //���¶ȴ���
static uint8_t  xdata  LoTemp_Count=0; 	  //���¶ȴ���
static uint8_t  xdata  LoTemp_CQK_Count=0;//�����ڵ��¶ȴ���
static uint8_t  xdata  LoHumity_Count=0;  //��ʪ�ȴ���

uint8_t No_HeatSensor_Times=0;//�������¶ȴ���������Ӧ����
static uint16_t  No_CQKSensor_Times=0;//�������¶ȴ���������Ӧ����
static uint16_t  CQKSensor_Hithen800_Times=0;//����80��
static uint16_t  JRP_Sensor_Temp_No_Change_Times=0;// �������¶���ȫ����Ĵ���2014-07-03
static uint16_t  CQK_Sensor_Temp_No_Change_Times=0;// �������¶���ȫ����Ĵ���		
static uint8_t ERR_RT_Temp_Times=0;//�����¶ȳ���80��
static uint8_t No_ReadData_SHIDU_Times=0;//���߶��¶�����Ӧ����
static uint16_t  RT_Temp_Not_Updated_10mS = 0;//����δ���������ݵ�ʱ��
static uint8_t No_ReadData_Temp_Times=0;//�������¶�����Ӧ����
static uint16_t  Nochange_Times=0;//���߶���ʪ����ȫ����Ĵ���

uint8_t RT_Temp_Reach_Set_Cnt = 0;//�����¶ȴﵽ�趨�¶�-1.5
uint8_t CQK_Temp_Reach_Set_Cnt = 0;//�������¶ȴﵽ30

uint8_t show_str[16]; 

static uint8_t xdata  Work_Day=0;
static uint8_t xdata  Work_Hour=0;
static uint8_t xdata  Work_Min=0;
static uint8_t xdata  Work_Sec=0;

//�洢���
struct stc_data_flash data_flash;//
static uint16_t  Run_Count;
BYTE  xdata  SaveData[16];
																		
//������ر���																		
static bit Buff_Full_flag;		
static uint8_t xdata  Rec_Count;
static BYTE  xdata  Rec_Buff[21];          //���յ���ָ��READ0001[CR]-READ0200[CR];
                                    //һ��ָ��ɽ���256������																		
static uint16_t   Tick_20ms=0;
uint8_t defalut_mode = Load_User_Pre_MODE;	//Ԥ��ģʽѡ��

//ʵʱ����
int  xdata  					RT_Temp=0;            //������¶�
int  xdata  					JEP_Temp=280;           //�������¶�
long  xdata  					CQK_Temp=280;           //�������¶�
static uint16_t  xdata  	RT_SHIDU=0;             //�����ʪ��
static uint16_t  xdata  	CONTROL_RT_SHIDU=550;     //���Ƽ��ȵ�ʪ��
uint16_t    xdata Diplay_RTtemp = 0;

//�趨����		
uint8_t  				Work_Mode = Invasive_Mode; //Ĭ��1Ϊ�д�ģʽ ��0Ϊ�޴�ģʽ��2������ģʽ(�޷���˿)				
int  									Set_RT_Temp=390;         //������¶��趨ֵ				
uint16_t  				Set_RT_WCTemp=340;       //�����޴��¶�
uint16_t  				Set_RT_YCTemp=390;       //�����д��¶�
uint16_t  				Set_CQK_Temp=370;        //�������¶�
uint16_t  				Set_CQK_WCTemp=310;      //�����޴��¶�
uint16_t  				Set_CQK_YCTemp=370;      //�����д��¶�
uint8_t 			  In_Exp_Ratio=4;	 //In��Exp���ȷ�ʽ�ı���  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5

//�����������ͼ��
uint16_t  				HP_CNT_Int = 0;//�������жϼ�����
uint16_t  				HP_CNT_Int_End_Rem = 0;//8S����ʱ�������жϼ�����
uint8_t 				WireIn_State=0;      //������In״̬ 1-�� 0-��
uint8_t 				WireOut_State=0;     //������Out״̬	  1-�� 0-��
uint8_t 				HeaterPlate_State = 0; //������״̬ 1-�� 0-��

uint8_t 				Micro_Temp_Val=0;   //���������Ϊ201;��СΪ0,1��ʾ10ms����ʱ��
uint16_t 					Micro_Temp_In=0;    //����֧
uint16_t 					Micro_Temp_Out=0;   //����֧

//���Ʋ���
static uint16_t   Aim_SHIDU=900;  //Ŀ��ʪ�� 
static int  xdata  JEP_Temp_Aim=400;           //������Ŀ���¶�
static uint16_t Wire_Warm_Up_Sec;    //��<=300ʱ���Ʒ���˿�����ȹ���
static uint16_t Plate_Warm_Up_Sec;   //��<=600ʱ���Ʒ����������ȹ���
static uint16_t Humidity_No_Change_Sec;//ʪ�Ȳ��ı�ļ�����������600ʱ����̽ͷʧЧ��ʪ�ȿ���ʧЧ
static uint16_t Low_Power_Mode_Flag = 0;//�͹���ģʽʹ�ܱ�־

uint8_t  xdata   Test_Mode_Dis_Jrp_Ctl=0; //�Ӳ���ģʽ��������ģʽ����ʾ���й�������	
static uint8_t  xdata   Micro_Adj_Mode_Test;  //���ȵ�ģʽ����
uint16_t xdata     Working_Normal=0;//��������������ÿ���1���������ã��ı�����Ҳ����

static struct PID
{
	int Uk; //�ܵĿ�����
	int Uk1;//�ϴε��ܿ�����
	int Sum_error;//�������
	int Ek;//��ǰ�������
	int Ek1;//ǰһ�ε������
	int Ek2;//ǰ���ε������       
}PID_temp,PID_temp2,PID_temp3;//����һ���¶�PID�ṹ


 //��    ��    ʱ    ��    ��  ����    ��
static BYTE   NowTime[7]= {0x00, 0x00, 0xB2, 0x01, 0x01, 0x05, 0x12};

//uint8_t Alarm_time_det_10mS_cnt = 0;

//================================================== 

static void Store_Data(void);           //�������
static void Get_RunCount(void);         //�������
static void Init_Erase_Flash_Data(void);//��ʼ��ʱ�����ⲿFLASH����
static void Init_port(void); //��ʼ���˿�״̬����ʾ�汾�ţ���ȡ���ݣ���Ļ���ԣ�W25X������
static void Main_Init(void);	//�õ�������ݵ�����,����������,ʹ��������,RTC���,����������ʼ��
static void Write_Default_Setting_to_flash(void); //��Ĭ�ϳ����趨д��FLASH
static void LCD_Show_Verion(void);//��ʾ�汾��
static void PatientTemp_NoneHeatWire_Adj(void);//�޻�·����˿ʱ�Ļ��߶��¶ȵ���
static void PID_Calc(void); //PID�㷨
static void HeatPlateTemp_Aim_PID_Calc(void); //������Ŀ���¶�PID�㷨---
static void HeatPlate_PID_Calc(void);//�����̵�PID�㷨---



static void  Interrupt_EXT1()  interrupt 2  //�ⲿ�ж�EXT1
{
	HP_CNT_Int++;	
}

static void  Interrupt_Time0()  interrupt 1  using  2  //10ms�ж�һ�� //22.1184M
{	  
	 Timer_T0_INT_Main();
	 Tick_20ms++;
	 
}

static void  Serial (void) interrupt 4 using 1		//�����ж�
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
    UART_REC_STATE_READY = 0, //׼������,��ʼ������,  1
    UART_REC_STATE_HEADER_DET,//�������ͷ "NEUNIT=1," ��9���ַ� 
    UART_REC_STATE_DATA_TAIL_DET,//��������(���9���ַ�)���������β"\CR"��3���ַ� 
    UART_REC_STATE_DONE, //������ȷ���
}UART_RecState = UART_REC_STATE_READY;


 static uint8_t   UART_Rec_Buf[20];//���ջ����� 
 
static void UART_RecBuf_Clear_Fun(void)  //��ջ�����
 {
   uint8_t i;
   
   for(i=0;i<=19;i++)
    {
       UART_Rec_Buf[i] = 0;        
    } 
 }

//��������
static uint8_t   DataToPc[8];
void UART_RecData_Func(void)
{
	  static uint8_t   UART_Rec_Cnt = 0;//������������ 
//	  static uint8_t UART_Len = 0;//���յ����ݳ���
    uint8_t   i;
    uint8_t   uart_data;	  
	  unsigned long   Adress;
//	  uint16_t    sum;
	
		while(EUSART_DataReady)//������������
    {
			uart_data = EUSART_Read(); //�ӻ�������һ������
			
			if(Work_State == UI_STATE_DATAREADER_MODE)
			{
					UART_Rec_Cnt++;//��������
					if(UART_RecState == UART_REC_STATE_READY)//��ʼ
					{
						UART_RecState = UART_REC_STATE_HEADER_DET;           
						UART_RecBuf_Clear_Fun();//��ջ�����
						UART_Rec_Buf[0] = uart_data;//��ʼ����
						UART_Rec_Cnt = 1; //����
						//EUSART_Write('A');
					}
					else if(UART_RecState == UART_REC_STATE_HEADER_DET)//������ͷ
					{//ע��:����8���ֽڴ�ŵ�˳�������յ��������λ����:�յ�����AXXLOGET����ŵ���TEGOLXXA
						UART_Rec_Buf[UART_Rec_Cnt-1] = uart_data;//�ӵ��ֽ������ֽڴ��
						if(UART_Rec_Cnt>=3)//���յ�3������ RD 13
						{
							if((UART_Rec_Buf[0] == 'R')//ͷ��־�ж�
								&&(UART_Rec_Buf[1] == 'D'))
							{
	//							EUSART_Write_Str("HEAR_OK",7);
								UART_RecState = UART_REC_STATE_DATA_TAIL_DET;//��־OK����������
								UART_RecBuf_Clear_Fun();//��ջ�����  
								UART_Rec_Cnt = 0; //����
							}
							else
							{
								UART_Rec_Cnt--;//����λ��
								for(i=0;i<=18;i++) //��������λ��һλ
								{
									 UART_Rec_Buf[i] = UART_Rec_Buf[i+1];        
								}//ע��:����8���ֽڴ�ŵ�˳�������յ��������λ����:�յ�����AXXLOGET����ŵ���TEGOLXXA               
							}
						} 
					}
					else if(UART_RecState == UART_REC_STATE_DATA_TAIL_DET)//������ݺͽ�β
					{
						UART_Rec_Buf[UART_Rec_Cnt-1] = uart_data;//�ӵ��ֽ������ֽڴ��

						if(UART_Rec_Cnt>=2)//�����15������
						{	
							UART_RecState = UART_REC_STATE_DONE;//������� 				
						}
					}
					
					if(UART_RecState == UART_REC_STATE_DONE)//��⵽β��־
					{
						UART_RecState = UART_REC_STATE_READY; //׼����һ�μ��
						
						Adress=UART_Rec_Buf[0];
						Adress=(Adress<<8)+UART_Rec_Buf[1]-1;
						Adress=(Adress<<4)+0x2000;
						SPI_Read_nBytes(Adress,16,&SaveData[0]);
						EUSART_Write_Str(&SaveData[0],16);//����16���ֽڸ�PC
					}						
			 }
		}
}

void HeaterWireModeDetFunc(void)
{
			if(Wire_Mode_Sel == Wire_Sel_Double)//˫֧���Ȼ�·��ʵ��δ��⵽IN��OUT	 			 	 
		{
			if((WireIn_State == 0)||(WireOut_State == 0))	
			{
				Wire_Mode_Mismatch = 1;//��·����˿ģʽ��ʵ�ʲ�ƥ���־
			}
			else
			{
				Wire_Mode_Mismatch = 0;//��·����˿ģʽ��ʵ�ʲ�ƥ���־
			}
		}	
	  else if(Wire_Mode_Sel == Wire_Sel_In_Only)	//��֧���Ȼ�·��ʵ��δ��⵽IN���⵽��OUT  	 
		{
			if((WireIn_State == 0)||(WireOut_State > 0))	
			{
				Wire_Mode_Mismatch = 1;//��·����˿ģʽ��ʵ�ʲ�ƥ���־
			}
			else
			{
				Wire_Mode_Mismatch = 0;//��·����˿ģʽ��ʵ�ʲ�ƥ���־
			}
		}
	  else if(Wire_Mode_Sel == Wire_Sel_None)	//�޼��Ȼ�·��ʵ�ʼ�⵽��IN��OUT
	  {
	  	if((WireIn_State > 0)||(WireOut_State > 0))	
			{
				Wire_Mode_Mismatch = 1;//��·����˿ģʽ��ʵ�ʲ�ƥ���־
			}
			else
			{
				Wire_Mode_Mismatch = 0;//��·����˿ģʽ��ʵ�ʲ�ƥ���־
			}
	  }	
}

void RefreshTempHumidyFunc(uint8_t Refresh_En)//ˢ����ʾ�¶Ⱥ�ʪ��
{
	static uint8_t  xdata   Refresh_WenDu_Cnt = 0; 
	static uint8_t  xdata   Alarm_WenDu_Cnt = 0; 

	uint16_t Disp,color;
	uint16_t Temp_Dis_En = 0;//�¶���ʾ��־
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

	  Temp_Dis_En = 0;//�¶���ʾ��־
		if(Refresh_En)Temp_Dis_En = 1;//ǿ��ˢ��һ��	
	
	  color=BLACK18;
	  if((Bit_is_one(ERR_Kind,Alarm_Const_HiTemp))   //���߶˸��º�ɫ
			&&(Display_Temp_Kind==DISPLAY_Temperature_Patient))
	  {
			 Temp_Dis_En = 1;//�¶���ʾ��־
			 color=RED18; 
		 	  if(Alarm_WenDu_Cnt == 1)
			  {
			  	color=WHITE18;
			  }
	  }
	  else if(((Bit_is_one(ERR_Kind,Alarm_Const_LoTemp))&&(Display_Temp_Kind==0))  //���߶˵��º�ɫ
					 ||((LoTemp_CQK_Count>120)&&(Display_Temp_Kind==1)))  //�����ڵ��º�ɫ
	  {
			 Temp_Dis_En = 1;//�¶���ʾ��־
			 color=RED18;
			 if(Alarm_WenDu_Cnt == 1)
			 color=WHITE18;
	  }
	  else
	  {
			if(Refresh_WenDu_Cnt == 0)Temp_Dis_En = 1;//�¶���ʾ��־
	  }
		
		if(Temp_Dis_En == 1)
		{			
			if(Display_Temp_Kind==DISPLAY_Temperature_Chamber)//�������¶�
			{
			  Disp=CQK_Temp;
			  Disp_Err=CQK_Sensor_Err; 

	    }
			else if(Display_Temp_Kind==DISPLAY_Temperature_Patient) //������¶�
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
		     //Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,RED18); //����	
		  }
			else  if(Disp>=1000)//��ʾ�¶�
		  {
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34,(Disp%1000)/100,color);   
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+73,(Disp%100)/10,color);    
		    Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,WHITE18); //ȥ��С����
		  }
			else
		  {
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y,(Disp/100)%10,color);  
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34,Disp%100/10,color);   
		    DISP_TEMP_30X56(POS_RT_TEMP_X,POS_RT_TEMP_Y+34*2+5,Disp%10,color);    
		    Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,color); //����
		  }
	   }		 
		 
	 //��ʾʪ��
		color=BLACK18;
		if(Bit_is_one(ERR_Kind,Alarm_Const_LoHumity))   //��ʪ�ȱ���
		{
			Temp_Dis_En = 1;//�¶���ʾ��־
			color=RED18;
			if(Alarm_WenDu_Cnt == 1)
			{
				color=WHITE18;
			}
		}
		else
		{
			if(Refresh_WenDu_Cnt == 0)Temp_Dis_En = 1;//�¶���ʾ��־				
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

//---------ˢ������ʱ��
void RefreshRunTimeFunc(uint8_t Refresh_En)
//Refresh_En 1-ǿ��ˢ��һ�Σ�����ÿ����ˢ��һ��
{ 
	uint8_t t;
	static uint8_t Last_Sec = 60;		
	
	RX8010_GetTime(NowTime);	
	t = (NowTime[0]&0xF)%10;
	
	if(t != Last_Sec)//ʱ�����б仯
	{
		 Last_Sec = t;
		 if(Work_Sec<59)
		 {
				Work_Sec++;
		 }
		 else
		 {
			 Work_Sec = 0;
			 t = 0xff;//ˢ�±�־		
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

//---------ˢ��RTCʱ��
void RefreshRTCTimeFunc(void)
{
	static uint8_t Last_Min;
	uint8_t color;
	uint8_t t;
	static uint8_t Last_Sec = 60;

	
//ʱ��ת��=================================================
	RX8010_GetTime(NowTime);	
	t = (NowTime[0]&0xF)%10;
	if(t != Last_Sec)//ʱ�����б仯
	{
		Last_Sec = t;
				 
		if(Last_Min!=NowTime[1])
		{
		 Last_Min=NowTime[1];
		}
		color=BLACK18;		
		DisPlayTime(NowTime,2); //��ʾʱ��
		DisPlayTime(NowTime,3); //��ʾʱ��
		DisPlayTime(NowTime,4); //��ʾʱ��
		DisPlayTime(NowTime,5); //��ʾʱ��
		DisPlayTime(NowTime,6); //��ʾʱ��
		DisPlayTime(NowTime,7); //��ʾʱ��
	}
}

void GetTempHumidity_PatientFunc(void)
{
	static uint8_t Get_time_out = 0;//��ʱ��ʵ��64mS�������100mS
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
	  if(Read_Order==0)		//���¶�ָ��	  	
		{
		 	Read_SHT21_Err = SHT21_WriteTemp(); 
			if(Read_SHT21_Err)
			{
				Read_Order = 0;//���մ���
			}
			else
			{				
				Read_Order = 1;
				Get_time_out = 0;
			}
		}
		else   if(Read_Order == 1)//�¶����ݽ���
	  {
			//SCL_INPUT_HIGH(); // set SCL I/O port as input
			Read_SCL_CONF=SCL_CONF;
			if(Read_SCL_CONF==1)
		  {
				Get_time_out = 0;
				TempLong =	SHT21_ReadData();
				//RT_Temp_Test = TempLong;
					//LCD_ShowxNum(223,160,16,5,TempLong,0x80,BLUE18);//	
					
			  if((TempLong == SHT21_ERROR) || (TempLong & 0x8000)) //��ȡ��������Ϊʪ�ȱ�־λ
				{
					//if(No_ReadData_Temp_Times<250) No_ReadData_Temp_Times++;	
					Read_Order = 0;		
				}
				else
				{					
					No_ReadData_Temp_Times = 0;
					Read_Order = 2;	
										
					if(Work_State != UI_STATE_SERVICE_MODE) //ע��,����ģʽ����ʾ��ʵ�¶�
					{

							if(WireIn_State!=0)    //�м���˿�߽����¶�����
								TempLong=TempLong*101/100-4*TempLong/388;
						 else
								TempLong=TempLong-4*TempLong/400;	 

							if(TempLong>=350)   //�¶�����
								TempLong=TempLong+6*(TempLong-350)/45;
							
							
					   if(TempLong>800) //�������˴ﵽ80˵�����쳣�������� //2014-07-07 
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
		else if(Read_Order==2)  //��ʪ��ָ��
		{										 
		  Read_SHT21_Err = SHT21_WriteRH();
			if(Read_SHT21_Err)
			{
				Read_Order = 2;//���մ���
			}
			else
			{				
				Read_Order = 3;
				Get_time_out = 0;
			}
		} 	  
		else  if(Read_Order == 3)//ʪ�����ݽ���
		{
			//SCL_INPUT_HIGH(); // set SCL I/O port as input
		Read_SCL_CONF=SCL_CONF;
		if(Read_SCL_CONF==1)
		{ 
				//Read_Order = 0;
				Get_time_out = 0;
				TempLong =	SHT21_ReadData();
				//LCD_ShowxNum(223,210,16,5,TempLong,0x80,BLUE18);//	
			 if((TempLong == SHT21_ERROR)||((TempLong & 0x8000) ==0)) //��ȡ����
			{
				if(No_ReadData_SHIDU_Times<250) No_ReadData_SHIDU_Times++;
				CONTROL_RT_SHIDU = 0;
				Read_Order = 2;	
				
			}
			else
			{
				Read_Order = 0;	
				TempLong &= 0x7fff;//���λ��1����ʪ��
				No_ReadData_SHIDU_Times = 0;
				if(TempLong > 999)TempLong = 999;  
				RT_SHIDU=TempLong;
				CONTROL_RT_SHIDU=TempLong;  //����ʪ��
				//75%���Ͻ���У׼
				if(Work_State != UI_STATE_SERVICE_MODE) //ע��,����ģʽ����ʾ��ʵʪ��
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
							 RT_SHIDU=RT_SHIDU+180*(RT_Temp-350)/100;  //��ʾʪ�� 16-05-06
						}
				}
				else
				{
					
				}	
				RT_SHIDU += 5;	//��������				  
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
		if(Get_time_out > 100)//����100mS
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
	
//	static uint16_t CQK_Temp_Not_Updated_40mS = 0;//����δ���������ݵ�ʱ��S
//  static uint16_t JRP_Temp_Not_Updated_S = 0;//����δ���������ݵ�ʱ��S

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
	    HeatingPlateSensor_Port=0;                                //���������͵�ƽ
	    ChamberOutletSensor_Port=0;
	    delay_us(609);                                //609
	    HeatingPlateSensor_Port=1;                                //�ͷ����ߵȵ�����������,������15~60us
	    ChamberOutletSensor_Port=1;
	    delay_us(86);                           //��ʱ70us                     //��ʱ70us
	    HeaterSensorExist=HeatingPlateSensor_Port;		            //����Ӧ���ź�
	    CQKSensorExist=ChamberOutletSensor_Port;
	    delay_us(488);                                //403
	    DS18B20_WriteByte(0xCC);                      //����ROM����
	    DS18B20_WriteByte(0x44);                      //��ʼת������
	 }
   else if(Get_Temp_Cnt == 2) //
	 //�����¶�ת��
	 {
	    HeatingPlateSensor_Port=0;                                //���������͵�ƽ
	    ChamberOutletSensor_Port=0;
	    delay_us(609);                                //503us
	    HeatingPlateSensor_Port=1;                                //�ͷ����ߵȵ�����������,������15~60us
	    ChamberOutletSensor_Port=1;
	    delay_us(86);                           //��ʱ70us
	    HeaterSensorExist=HeatingPlateSensor_Port;		            //����Ӧ���ź�
	    CQKSensorExist=ChamberOutletSensor_Port;
	    delay_us(488);
	    DS18B20_WriteByte(0xCC);                      //����ROM����
	    DS18B20_WriteByte(0xBE);                      //���ݴ�洢������
	    delay_us(10);
	    DS18B20_ReadByte();                           //���¶ȵ��ֽ�
	    T18B20_L=Read_18B20_Value;
	    CQK_T18B20_L=Read_18B20_Value2;
	    DS18B20_ReadByte();                           //���¶ȸ��ֽ�
	    T18B20_H=Read_18B20_Value;
	    CQK_T18B20_H=Read_18B20_Value2;
	
		  if((CQKSensorExist==0))//�����򲻽���У��
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

					 CQK_T18B20_H_GET = (CQK_T18B20_H_GET << 4) | (CQK_T18B20_L_GET >> 4);			//�������¶�ֵ�ϲ��ߵ�λ��������λ
				 if(CQK_T18B20_H_GET&0x80)//�Ǹ���
				 {
								CQK_T18B20_L_GET =(((~CQK_T18B20_L_GET)&0x0F)*10)/16;
								temp=(~CQK_T18B20_H_GET)*10+CQK_T18B20_L_GET;
				 } else
				 {
							CQK_T18B20_L_GET = ((CQK_T18B20_L_GET&0x0F)*10)/16;
							temp=CQK_T18B20_H_GET*10+CQK_T18B20_L_GET;
				 }
		
				 if((CQK_Temp_Last ==  temp) && (temp == 850))  //����ʱһֱ��ʾ85�� 2014-07-18
				 {
						CQK_Sensor_Temp_No_Change_Times++;//2014-07-03 �������¶ȴ������¶���ͬ�Ĵ���
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
				 ||((CQK_Temp_Last <=  temp) && (temp - CQK_Temp_Last  < 200)))//EFT�Բߣ��¶Ȳ���˲��,1S�仯���ܳ���20��
				 {	
						if((temp > 0)&&(temp < 850))//������
						{
							CQK_Temp = temp;//ʵʱ��ʾ�¶�
							if(Work_State != UI_STATE_SERVICE_MODE) //ע��,����ģʽ����ʾ��ʵ�¶�
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

		if(Work_State == UI_STATE_SERVICE_MODE)//�ڲ���ģʽʱ��ʾ��ȡ������
		{
//			LCD_ShowxNum(10,220,24,3,T18B20_H_GET,0x80,BLACK18); //8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ ������
//			LCD_ShowxNum(10,268,24,3,T18B20_L_GET,0x80,BLACK18); //8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ
		}		 


		T18B20_H_GET = (T18B20_H_GET << 4) | (T18B20_L_GET >> 4);			//�������¶�ֵ�ϲ��ߵ�λ��������λ
		if(T18B20_H_GET&0x80)//�Ǹ���
		{
					T18B20_L_GET =(((~T18B20_L_GET)&0x0F)*10)/16;
					temp=(~T18B20_H_GET)*10+T18B20_L_GET;
		} 
		else
		{
				T18B20_L_GET = ((T18B20_L_GET&0x0F)*10)/16;
				temp=T18B20_H_GET*10+T18B20_L_GET;
		}
		  
			 
		 if((JRP_Temp_Last ==  temp))// && (temp == 850))  //����ʱһֱ��ʾ85�� 2014-07-18
		 {
		 	JRP_Sensor_Temp_No_Change_Times++;//2014-07-03 �������¶ȴ������¶���ͬ�Ĵ���
			if(JRP_Sensor_Temp_No_Change_Times > 6000)JRP_Sensor_Temp_No_Change_Times = 6000;
		 }
		 else
		 {
		 	JRP_Sensor_Temp_No_Change_Times = 0;
		 }
		 JRP_Temp_Last =  temp;		

		  JEP_Temp =  temp;	  //ʵʱ��ʾ�¶�

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
static int  Set_CQK_Temp_Comp;//������У�������Ŀ���¶�
void  HeaterPlateWireControlFunc(void)//�¶ȿ���
{
//    uint8_t  Temp3;
	  uint16_t  Val_Calc;

		int  Humidity_Err; //δ�ﵽĿ��ʪ�ȵĲ�ֵ
		int  Humidity_Comp;//����Ŀ���¶ȵĲ�ֵ 
	  static uint16_t Last_Rem_Humidity;	
	
	   static uint16_t WarmUp_S_Cnt;	
	   //uint16_t  CQK_Vaul_Err;//��ֵ

	  if((Work_State != UI_STATE_SCREENSAVER_MODE)&&(Test_Mode_Dis_Jrp_Ctl == 1))
	  {	//8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ ������
			Back_Color=WHITE18;
			LCD_ShowxNum(POS_RT_RH_X-50,5,16,2,Micro_Adj_Mode_Test,0x80,BLACK18); //�����̼��ȵ�ģʽ��ʾ
			LCD_ShowxNum(POS_RT_RH_X-50,34,16,4,JEP_Temp,0x80,BLACK18); //�����̵��¶�
			LCD_ShowxNum(POS_RT_RH_X-50,73,16,3,CQK_Temp,0x80,BLACK18); //�����ڵ��¶�
			LCD_ShowxNum(POS_RT_RH_X-50,107,16,3,Set_CQK_Temp,0x80,BLACK18); //�������趨�¶�
			LCD_ShowxNum(POS_RT_RH_X-50,141,16,3,Micro_Temp_Val,0x80,BLACK18); //�����̵ļ���ʱ��
			
			LCD_ShowxNum(POS_RT_RH_X-50,185,16,3,CONTROL_RT_SHIDU,0x80,BLACK18); //��ʵ���ʪ��
			LCD_ShowxNum(POS_RT_RH_X-50,219,16,3,Aim_SHIDU,0x80,BLACK18); //Ŀ��ʪ��
			LCD_ShowxNum(POS_RT_RH_X-50,253,16,3,Micro_Temp_In,0x80,BLACK18); //In����ʱ��
			LCD_ShowxNum(POS_RT_RH_X-50,287,16,3,Micro_Temp_Out,0x80,BLACK18); //Exp����ʱ��
			
			LCD_ShowxNum(POS_RT_RH_X-33,2,16,1,Wire_Mode_Sel,0x80,BLACK18); //����˿ģʽ			
			LCD_ShowxNum(POS_RT_RH_X-33,10,16,1,WireIn_State,0x80,BLACK18); //IN����˿״̬
			LCD_ShowxNum(POS_RT_RH_X-33,18,16,1,WireOut_State,0x80,BLACK18); //EXP����˿״̬	
			
			//LCD_ShowxNum(POS_RT_RH_X-20,185,16,1,SHT21_Heater_State,0x80,BLACK18); //SHT����״̬
			LCD_ShowxNum(POS_RT_RH_X-33,253,16,3,Wire_Warm_Up_Sec,0x80,BLACK18); //600S��ʱ	
			LCD_ShowxNum(POS_RT_RH_X-33,107,16,3,Set_CQK_Temp_Comp,0x80,BLACK18); //У������ĳ�����Ŀ��ʪ��
			//LCD_ShowxNum(POS_RT_RH_X-33,141,16,3,Plate_Warm_Up_Sec,0x80,BLACK18); //600S��ʱ	
			LCD_ShowxNum(POS_RT_RH_X-33,185,16,3,Humidity_No_Change_Sec,0x80,BLACK18); //600S��ʱ			
		
			if(WireIn_State!=0)	//�з���˿
			{
					LCD_ShowxNum(POS_RT_RH_X-33,34,16,4,JEP_Temp_Aim,0x80,BLACK18); //�����̵�Ŀ���¶�
					LCD_ShowxNum(POS_RT_RH_X-16,34,16,3,WarmUp_S_Cnt,0x80,BLACK18); //�޷���˿��·ʱ�ı�ʱ��	
				
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
					LCD_ShowxNum(POS_RT_RH_X-33,34,16,4,Temp2_Int,0x80,BLACK18); //�޷���˿��·ʱ�����̵�Ŀ���¶�
					LCD_ShowxNum(POS_RT_RH_X-16,34,16,3,NoneWire_Heat_Sec,0x80,BLACK18); //�޷���˿��·ʱ�ı�ʱ��	
			}
			
			LCD_ShowxNum(POS_RT_RH_X+40,170,16,3,Temp1_Int,0x80,BLACK18); //��������ʽ�¶�Ŀ��
			LCD_ShowxNum(POS_RT_RH_X+20,170,16,3,RT_Temp,0x80,BLACK18); //���߶��¶�
			LCD_ShowxNum(POS_RT_RH_X+20,210,16,4,Nochange_Times,0x80,BLACK18); //��ʪ�������������		

			LCD_ShowxNum(2,2,16,5,Working_Normal,0x80,BLACK18); //��������ʱ���ʱ	
			LCD_ShowxNum(2,60,16,5,HP_CNT_Int_End_Rem,0x80,BLACK18); //��ʾÿ4������̷����Ĵ���			
	  }

	  if(		     
				(ERR_Kind & 0x01)!=0//����������				
				||(ERR_Kind & 0x02)!=0 //����
		    ||(ERR_Kind & 0x10)!=0 //��ˮ		
	      || Wire_Mode_Mismatch == 1  //����˿δѡ
				|| HeaterPlate_State==0)//ˮ��δװ��
	  {
	  	  Micro_Temp_In=0;
	  	  Micro_Temp_Out=0;
	      Micro_Temp_Val=0;  //ֹͣ����
				Micro_Adj_Mode_Test = 1;//����ģʽ����
				Wire_Warm_Up_Sec = 0; //��<=300ʱ���������ȹ���	 
				//Plate_Warm_Up_Sec = 0;
				Humidity_No_Change_Sec = 0;			
	  }
		else
		{				
				//���ڷ���˿����,�����¶ȵ���========================== 			
			
			 if(Work_Mode==Noninvasive_Mode) //�޴�ģʽʱ,������¶ȶ�Ŀ��ʪ�ȵĿ����߼�
			 {	  	 	
					Aim_SHIDU=790;  
			 }else	  //�д�ģʽʱ,������¶ȶ�Ŀ��ʪ�ȵ�Ӱ��
			 {
					Aim_SHIDU=890; //2014-11-21	
			 }

			Val_Calc = 0;				
			//if(Boot_Start_RT_Temp_Heating_Timer_Sec > 300) //���߶˿�������ʽ���Ƚ���
			if(Low_Power_Mode_Flag == 0)//�ǵ͹���ģʽ�ſ��Կ���
			{//��·����˿�Ŀ���
				if(WireIn_State!=0)		//���й���IN����˿ʱ�����ڷ���˿�ļ����߼�
				{
			
					if(Working_Normal >= 1200)
					{
						Temp1_Int = Set_RT_Temp;//20�����Ժ�Ϊ�趨�¶�
					}
					else
					{						
						Temp1_Int = CQK_Temp + 30;//20������ǰ����������¶�
					}
					if(Temp1_Int > Set_RT_Temp)Temp1_Int = Set_RT_Temp;
					
					
					//PID_temp.Ek = Set_RT_Temp - RT_Temp;//��ǰ������� �趨�¶�-��ǰʵ���¶�
					PID_temp.Ek = Temp1_Int - RT_Temp;//��ǰ������� �趨�¶�-��ǰʵ���¶�
					PID_Calc(); 
					Val_Calc = (uint8_t)PID_temp.Uk;	//���������ʱ��					
					
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
				else	  //���޹��ڷ���˿ʱ
				{
							 Val_Calc=0;
				}
				
				if(Working_Normal >= 1200)//��������20����֮��
				{

				}
				else
				{
					if(Val_Calc > 160)Val_Calc = 160;//20���������Ƽ��ȹ���
				}
				
				
				Micro_Temp_In = Val_Calc;

				Val_Calc=0;
				if(WireOut_State!=0)	  //���й���OUT����˿ʱ�����ڷ���˿�ļ����߼�
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
						 if (RT_Temp >= Set_RT_Temp+10)//����1��,ֹͣ����,������ʩ
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

			//���޴�ģʽʱ�����̵ļ����߼�
			{
				//�����ּ�����/////////////////////////////	  
					if(CONTROL_RT_SHIDU<=Aim_SHIDU)//��Ҫ����Դ�����ʱ�Ĳ���
					{
							Humidity_Err = (Aim_SHIDU - CONTROL_RT_SHIDU);///2;//ʪ��������
							if(Humidity_Err > 100)Humidity_Err = 100;//����޶�Ϊ100
					}		
					else Humidity_Err = 0;	
					
					Humidity_Comp =  CONTROL_RT_SHIDU - Aim_SHIDU;	//����ʪ���Լ�������ˮ		
					if(Humidity_Comp > 100)Humidity_Comp  = 100;
					
					if(Last_Rem_Humidity != CONTROL_RT_SHIDU)//��ʵʪ�ȸı�������
					{
							Humidity_No_Change_Sec = 0;
					}
					Last_Rem_Humidity = CONTROL_RT_SHIDU;
					
					if(Humidity_No_Change_Sec >= 900)//���ܽ�������״̬
					{
							Humidity_No_Change_Sec = 901;
							Humidity_Err = 0;	 //��������״̬��ʪ�ȿ�����Ч
							Humidity_Comp = 0;
					}				
								
				if((CQK_Temp>=660) ||(JEP_Temp>=HeatingTemperature_MAX))	   //�������¶����66��
				{
					 Micro_Temp_Val=0;//����65�ȱ���ֹͣ����
					 Micro_Adj_Mode_Test = 2;//����ģʽ����       	 
				}				
				//�������ߵ��¶ȵ���===========================
				else if(WireIn_State!=0)
				{
					 if(Low_Power_Mode_Flag == 0)//�ǵ͹���ģʽ�ſ��Կ���
					 {
							if(JEP_Temp>=HeatingTemperature_MAX)	 //105��
							{
								Val_Calc = 0;
								Micro_Adj_Mode_Test = 7;//����ģʽ����            	
							}
							else
							{	
								Set_CQK_Temp_Comp = Set_CQK_Temp; //�������趨�¶�
								
								if(Work_Mode==Invasive_Mode)//
								{	
										if(Humidity_Err >= 50)  //��Ŀ��ʪ�����5%������Ϊ�޴�ʱ��������߳�����
										{
											//Set_CQK_Temp_Comp += Humidity_Err/6;//���������1.6��
											Set_CQK_Temp_Comp += Humidity_Err/10;//���������1��
										}	
								}	
								else
								{
										if(Humidity_Err >= 50)  //��Ŀ��ʪ�����5%������Ϊ�޴�ʱ��������߳�����
										{
											Set_CQK_Temp_Comp += Humidity_Err/2;//���������5�� 120L 28�����¶�
										}
								}	

								if(Humidity_Comp >= 10)
								{
									//Set_CQK_Temp_Comp = Set_CQK_Temp_Comp - Humidity_Comp/5;//ʪ��ÿ��1%��������¶��½�0.2�ȣ����2��
									Set_CQK_Temp_Comp = Set_CQK_Temp_Comp - Humidity_Comp/10;//ʪ��ÿ��1%��������¶��½�0.1�ȣ����1��
								}		

								if(Work_Mode==Invasive_Mode)//
								{	
										//Set_CQK_Temp_Comp -= 5;//�д�ʱ����0.5��	
										if(Set_CQK_Temp_Comp < 320)Set_CQK_Temp_Comp = 320;//��޵��¶�
								}
								else
								{	
										if(Set_CQK_Temp_Comp < 300)Set_CQK_Temp_Comp = 300;//��޵��¶�
								}	

								////if(CQK_Temp < Set_CQK_Temp_Comp - 30) //	

								//�㷨��ʼ
								if(JEP_Temp < (JEP_Temp_Aim - 20))//�������¶�С�ڼ�����Ŀ���¶�-2		
								{
									WarmUp_S_Cnt = 0;//���㣬�ȴ��ﵽ����Ŀ��
								}	
								else
								{
									WarmUp_S_Cnt++;
									if(WarmUp_S_Cnt >= 20)//ÿ�����ӵ���һ�μ�����Ŀ���¶�
									{
										WarmUp_S_Cnt = 0;
										PID_temp3.Ek = Set_CQK_Temp_Comp - CQK_Temp;//��ǰ������� �趨�¶�-��ǰʵ���¶�
										HeatPlateTemp_Aim_PID_Calc();												

										JEP_Temp_Aim = JEP_Temp_Aim + PID_temp3.Uk;//												
										
									  if(Work_Mode==Invasive_Mode)//
										{
											if(JEP_Temp_Aim < 500)JEP_Temp_Aim = 500;//��С50��
										}
										else
										{
											if(JEP_Temp_Aim < 400)JEP_Temp_Aim = 400;//��С50��
										}	
										
										if(Working_Normal >= 1200)
//										if(Work_Min>=20||Work_Day>0 ||Work_Hour>0)//����20����֮��
										{
											if(JEP_Temp_Aim > 1045)JEP_Temp_Aim = 1045;//���102��
										}
										else
										{
											if(JEP_Temp_Aim > 980)JEP_Temp_Aim = 980;//����20�����������98��
										}
									}
								}
								
								
								PID_temp2.Ek = JEP_Temp_Aim - JEP_Temp;//��ǰ������� �趨�¶�-��ǰʵ���¶�
								HeatPlate_PID_Calc(); 
								Val_Calc = (uint8_t)PID_temp2.Uk;	//���������ʱ��

								if((CQK_Temp - Set_CQK_Temp_Comp) > 20)//�������¶ȱ�Ŀ���¶ȸ�
								{
									if(Val_Calc > 20)Val_Calc = 20;	
									JEP_Temp_Aim = JEP_Temp;//���ļ�����Ŀ���¶�
								}	
								else if((CQK_Temp - Set_CQK_Temp_Comp) > 10)//�������¶ȱ�Ŀ���¶ȸ�
								{

								}
						 }
					 }					 
					
					
					if(Val_Calc > 195)Val_Calc = 195;					
					if(Low_Power_Mode_Flag == 0)//�ǵ͹���ģʽ�ſ��Կ���
							Micro_Temp_Val = Val_Calc;//���ڼ��㣬���ⳬ��190�����ж�		
				}
				else if(Low_Power_Mode_Flag == 0)//�ǵ͹���ģʽ�ſ��Կ���
				{	
						PatientTemp_NoneHeatWire_Adj();
						//���������ߵ��¶ȵ���==============================================================
				 }  
			}		
		}
} 


void   Store_Data(void)   //������ݵ�ַ��1��ʼ
{
	  uint16_t i;
	  uint8_t N,j;
	  if (Run_Count<1) return;
	  i=(Run_Count-1)/8+1;     //�����ַ
	  N=Run_Count%8;       //����λ
	  if (Run_Count!=0 && N==0) N=8;
	  j=(0xFF<<N);
	  SPI_Write_nBytes(i,1,&j);
	 
        //delay_us(100);
}

void  Get_RunCount(void)   //�õ�������ݵ�����
{
	  uint16_t Start1,Start2,StartAdr;
	  uint8_t N,K,j;
	  Start1=0x1;
	  Start2=0x2000;
	  for(j=0;j<14;j++) //���Ҽ���λ
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
	 Run_Count=(StartAdr-1)*8;//����ʱ1���ֽڱ�8
	 for(j=0;j<8;j++)
	 {
	 	 if(((0x01<<j) & N)!=0) break;
	 }
	 Run_Count=Run_Count+j;      //���ɼ���44896��,256��Ϊ����

	 //Run_Count = 2000;//����ʹ�� 2014-12-26
	 //SPI_Read_nBytes(8000,1,&N);
	 // DS1302_GetTime(NowTime);
	 // if(NowTime[0]&0x80)         //���ֵ��������³�ʼ��ʱ�䣬�Ƿ�Ҫѯ��
	 //    DS1302_SetTime(initTime);    //��ʼ��ʱ��
	 /* if(N==1)                    //ʱ����������趨
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

static uint8_t  Wire_Mode_Sel_Rem = 0xff;//����״̬
static uint8_t  Disp_ERR_VHB80_Code = 0xff;

void Error_Base_HeaterWire_DISP_Enable(void)//����ʱ����ر�־,�Ա�����ʱ��ʾ�������ߵ�ͼ��
{
	Disp_ERR_VHB80_Code = 0xff;
	Wire_Mode_Sel_Rem = 0xff;//����״̬
}


void AlarmErrorFunc(void)    //�����¼�
{
	uint8_t color;
	static uint8_t  xdata   Err_Event_Cnt;  //��С��������
	uint8_t  xdata   Err_Event_1;
	uint8_t  xdata   Err_Event_2;
	  
	static uint8_t AlarmInfoRem = 0;//����״̬����
	{
		Err_Event_Cnt++;//��˸���������ٳ�������ʱ��
		if(Err_Event_Cnt > 1)
		{	
			Err_Event_Cnt = 0;//��	
		}
		Err_Event_1 = 0;
		Err_Event_2 = 1;			
	
		//LCD_ShowxNum(POS_RT_RH_X-40,5,16,4,RT_Temp,0x80,BLACK18); //8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ
		//LCD_ShowxNum(POS_RT_RH_X-40,60,16,4,ERR_RT_Temp_Times,0x80,BLACK18); //
	
		if((No_CQKSensor_Times>1000 || CQK_Sensor_Temp_No_Change_Times > 6000) //�������¶ȴ�����������
			||(CQKSensor_Hithen800_Times > 250)//����10S > 80��
			||((No_ReadData_Temp_Times>20))//������ʾ
		  )
		{
			 CQK_Sensor_Err=1;
			 CQK_Temp = 0;	
			 LoTemp_CQK_Count	= 0;//����������,���ɼ����±���
			 Working_Normal = 0;//��������ʱ������	
			 Set_bit(ERR_Kind,Alarm_Const_CQK_Sensor); 				
    }
		else
		{
			 CQK_Sensor_Err=0;
			 Clear_bit(ERR_Kind,Alarm_Const_CQK_Sensor); 				
		}
			
		if(Nochange_Times>900 || No_ReadData_SHIDU_Times>20 || RT_Temp_Not_Updated_10mS > 3000)//�����ʪ�ȴ�����������
		{
			 SHIDU_Sensor_Err=1;
			 RTD_Sensor_Err=1;
			 RT_SHIDU = 0; 
			 HiTemp_Count = 0;//����������,���ɼ����±���
			 LoTemp_Count = 0;//����������,���ɼ����±���
			 LoHumity_Count = 0;//����������,���ɼ���ʪ����
			 Working_Normal = 0;//��������ʱ������		
			 Set_bit(ERR_Kind,Alarm_Const_RTD_Sensor); 
		}		
		else
		{
				SHIDU_Sensor_Err=0;
				Clear_bit(ERR_Kind,Alarm_Const_RTD_Sensor); 
		}
			
		if(Nochange_Times>900 ||  No_ReadData_Temp_Times>20 
			|| ERR_RT_Temp_Times>20 || RT_Temp_Not_Updated_10mS > 3000)//������¶ȴ����������� 2014-07-03
  	{
	  	 RTD_Sensor_Err=1;
			 if(ERR_RT_Temp_Times<10)//�����쳣����ʱ�¶Ȳ�����
				{
					RT_Temp = 0; 		
					Diplay_RTtemp = 0;
				}
			 HiTemp_Count = 0;//����������,���ɼ����±���
			 LoTemp_Count = 0;//����������,���ɼ����±���
			 LoHumity_Count = 0;//����������,���ɼ���ʪ����
			 Working_Normal = 0;//��������ʱ������		
		}
  	else
  		   RTD_Sensor_Err=0;
		
    if( No_HeatSensor_Times>150 || JRP_Sensor_Temp_No_Change_Times > 600)//�����̴�����������	 
		{			
  		JRP_Sensor_Err=1;
			Set_bit(ERR_Kind,Alarm_Const_JRP_Sensor); 
		}
  	else
		{
  			JRP_Sensor_Err=0;
			  Clear_bit(ERR_Kind,Alarm_Const_JRP_Sensor); 
		}
  	if(HiTemp_Count>2)  //���´�����
		{
  	   Set_bit(ERR_Kind,Alarm_Const_HiTemp); 	   
		}
  	else
  		 Clear_bit(ERR_Kind,Alarm_Const_HiTemp);
    if(LoTemp_Count>120)  //���´�����
  		 Set_bit(ERR_Kind,Alarm_Const_LoTemp);
  	else
  	 	 Clear_bit(ERR_Kind,Alarm_Const_LoTemp);

    if(LoHumity_Count>120) //��ʪ�ȴ�����
    {
    	   Set_bit(ERR_Kind,Alarm_Const_LoHumity);//���õ�ʪ�ȱ�����־
    }
    else
    {
			if (Bit_is_one(ERR_Kind,Alarm_Const_LoHumity))
			{
				Clear_bit(ERR_Kind,Alarm_Const_LoHumity);//�����ʪ�ȱ�����־
			}
    }

	//ɫ�鱨��  VHB15A_FUNCTION_ALARM_COLOR_DISPLAY
	/*����̽ͷ����   	---��
	 ������̽ͷ���� 	---��
	 ������̽ͷ���� 	---��
	 ���±��� 			---��
	 ���±��� 			---��
	 ��ʪ�ȱ��� 		---��
	 ˮ��δ��װ�ñ��� 	---��
	 ������δ��װ��·	---��
	 ��ˮ/����		    ---��
	*/
		
	if(Work_State != UI_STATE_SCREENSAVER_MODE)//����ʱ����ʾ
	{
		if((SHIDU_Sensor_Err==1)||(RTD_Sensor_Err))	//�����̽ͷ
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+5,RED18);
		}
		if(CQK_Sensor_Err==1)//������̽ͷ
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+5,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+10,GREEN18);
		}
		if(JRP_Sensor_Err==1)//������̽ͷ
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+10,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+15,BLUE18);
		}
		if(HiTemp_Count>2)//����
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+15,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+20,BLACK18);
		}
		if(LoTemp_Count>120)//����
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+20,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+25,GRAY18);
		}
		if(LoHumity_Count>120)//��ʪ��
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+25,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+30,YELLOW18);
		}
		if(HeaterPlate_State==0)//ˮ��δװ�û����̿�·
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+30,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+35,BRIGHT_BLUE18);
		}
		if(Wire_Mode_Mismatch == 1)//��·����˿ģʽ��ʵ�ʲ�ƥ��	
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+35,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+40,PURPLE18);
		}
		if(Bit_is_one(ERR_Kind,Alarm_Const_NoWater)) //��ˮ(����)����
		{
			Draw_Rectangle_Real(POS_ALARM_COL_X,POS_ALARM_COL_Y+40,POS_ALARM_COL_X+10,POS_ALARM_COL_Y+45,BLACK18);
		} 
	}


	  if(HeaterPlate_State==0)  //û�з������
    {
    	AlarmInfoIndex = 1;
		}
		else if(Bit_is_one(ERR_Kind,Alarm_Const_NoWater)) //��ˮ����
    {
    	AlarmInfoIndex = 2;
    }
		else if(CQK_Sensor_Err || RTD_Sensor_Err || SHIDU_Sensor_Err)
    {
			AlarmInfoIndex = 3;
    } 
		else if(Bit_is_one(ERR_Kind,Alarm_Const_HiTemp)) //���´���
    {
    	AlarmInfoIndex = 4;
    }
		else if(Bit_is_one(ERR_Kind,Alarm_Const_LoTemp)) //���߶˳�������
    {
    	AlarmInfoIndex = 5;
    }
		else if(LoTemp_CQK_Count>120)//�����ڳ�������
	  {
    	AlarmInfoIndex = 6;
    }	
		else if(Bit_is_one(ERR_Kind,Alarm_Const_LoHumity))//������ʪ�ȴ���
    {
			AlarmInfoIndex = 7;
    }
		else if(Wire_Mode_Mismatch == 1)//��·����˿ģʽ��ʵ�ʲ�ƥ��	
    {
      AlarmInfoIndex = 8;
		}	
		else
    {
			AlarmInfoIndex = 0;
			AlarmSoundPauseStatus = 0;//�ޱ���������������ͣ
    }
		
		if(AlarmInfoIndex !=  AlarmInfoRem)//����״̬�ı�
		{
			AlarmSoundPauseStatus = 0;//������ͣȡ��
			Sound_Alarm_Pause_Disable();	
		}	
					
		AlarmInfoRem = AlarmInfoIndex;
		
	
  	//����������==============================================================
   ERR_Kind&=0xFE;  ////�����һλ�Ǵ���������
   if(CQK_Sensor_Err || RTD_Sensor_Err || JRP_Sensor_Err || SHIDU_Sensor_Err)
   {
   	  ERR_Kind|=0x01;//��һλ�Ǵ���������
   } 

   if(Work_State != UI_STATE_SCREENSAVER_MODE)//������ģʽ������Ҫ���˳�����ģʽ
	 {	
		 if(HeaterPlate_State==0)    //�޼����̣���ʾ���е�ͼƬ   Plate_State==0
		 {
			 Working_Normal = 0;//��������ʱ������

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
							if(Err_Event_Cnt == Err_Event_1) //�޼���
							{ 
									Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,GRAY18);  //���������ϲ����²�
									Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,GRAY18); 
							}
						} 

						if((Wire_Mode_Sel==Wire_Sel_In_Only || Wire_Mode_Sel== Wire_Sel_None))
						{
								if(Err_Event_Cnt == Err_Event_1)
								{
									Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,GRAY18);  //���������ϲ����²�
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,GRAY18);   //���������ϲ����²�
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,GRAY18);  //����� 	       	 	 
								}
						}
					}
					Wire_Mode_Sel_Rem = Wire_Mode_Sel;								 
				 }
				 if(Err_Event_Cnt == Err_Event_1)  	 
						 color=RED18;//����ɫ
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
			else  if(Bit_is_one(ERR_Kind,Alarm_Const_NoWater)) //��ˮ(����)����
			// if(0)
		 {
				 //��ʾ��ˮ����================================
				 if(Disp_ERR_VHB80_Code!=2)
				 {
						Disp_ERR_VHB80_Code=2; 
						DISP_VHB80_PIC(BLUE18);
				 }
				 if(Err_Event_Cnt == Err_Event_1)
						color=RED18;          //��ˮ�����Ϊ��ɫ
				 else if(Err_Event_Cnt == Err_Event_2)
						color=WHITE18;
				 Draw_Rectangle_Real(POS_HEAT_X,POS_HEAT_Y+2,POS_HEAT_X+8,POS_HEAT_Y+33,color);
							
		 }
		 else if(CQK_Sensor_Err || RTD_Sensor_Err || SHIDU_Sensor_Err  ||HiTemp_Count>2 || LoTemp_Count>120 ||LoTemp_CQK_Count>120) //̽ͷ�����ߵ��±���
	//	 || Heat_ERR
		 //else if(0)
		 {
			 //��ʾͼ�Σ���˸���󲿷�==========================================
			 if(Disp_ERR_VHB80_Code!=3)
			 {
					Disp_ERR_VHB80_Code=3; 
					DISP_VHB80_PIC(GRAY18);
			 }	   	

			 if(Err_Event_Cnt == Err_Event_1)
			 {
						color=RED18;  //��
			 }
				else if(Err_Event_Cnt == Err_Event_2)
			 {
						color=GRAY18;	//��
			 }


			 {

				 if(CQK_Sensor_Err||LoTemp_CQK_Count>120)//��ʾ�����ڴ���///
				 {
							DISP_CQK25X24(POS_CQK_X,POS_CQK_Y,color);
				 }else
				 {
							DISP_CQK25X24(POS_CQK_X,POS_CQK_Y,GRAY18);
				 } 
				 if(RTD_Sensor_Err||HiTemp_Count>2 || LoTemp_Count>120)  //�����̽ͷ����
				 { 
							DISP_RTD28X24(POS_RTD_X,POS_RTD_Y,color);
				 }else
				 {
							DISP_RTD28X24(POS_RTD_X,POS_RTD_Y,GRAY18);
				 } 
				 if(JRP_Sensor_Err) //������̽ͷ
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
		else if(Wire_Mode_Mismatch == 1)//��·����˿ģʽ��ʵ�ʲ�ƥ��	
			//else if(0)
		{
					Working_Normal = 0;//��������ʱ������	

					 {
						 if(Disp_ERR_VHB80_Code!=4)
						 {
								Disp_ERR_VHB80_Code=4; 
								DISP_VHB80_PIC(BLUE18);
						 }
						 
						 //��In�����߶�
						if(WireIn_State > 0)//��In��·����˿����ʾ��ɫ
						{
							color=BLUE18;
						}
						else
						{
							color=GRAY18;//��In��·����˿����ʾ��ɫ
						}
						if(Err_Event_Cnt <= Err_Event_1)//��ʾ��ǰ״̬���л����ޣ���ɫ���ɫ
						{//���������ɫ��ʾ
						}								
						else if(Err_Event_Cnt <= Err_Event_2)//����ģʽȷ����ʾ��ɫ�������ǵ�ǰ״̬��ɫ
						{
							if(((WireIn_State == 0)&&(Wire_Mode_Sel == Wire_Sel_None))//��In��·����˿����Noneģʽ����Ҫ����
								||((WireIn_State == 1)&&(Wire_Mode_Sel == Wire_Sel_Double))//��In��·����˿����˫֧ģʽ����Ҫ����
								||((WireIn_State == 1)&&(Wire_Mode_Sel == Wire_Sel_In_Only)))//��In��·����˿���ǵ�֧ģʽ����Ҫ����
							
							{
							}
							else//�������������Ҫ����:����Inʱ��/˫֧/Un,��InʱNone/Un
							{
								color=RED18;
							}
						}
						
						Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,color);  //���������ϲ����²�
						Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,color); 			
				
						
						 //��Exp�����߶�
						if(WireOut_State > 0)//��Out��·����˿����ʾ��ɫ
						{
							color=BLUE18;
						}
						else
						{
							color=GRAY18;//��Out��·����˿����ʾ��ɫ
						}
						if(Err_Event_Cnt <= Err_Event_1)//��ʾ��ǰ״̬���л����ޣ���ɫ���ɫ
						{//���������ɫ��ʾ
						}								
						else if(Err_Event_Cnt <= Err_Event_2)//����ģʽȷ����ʾ��ɫ�������ǵ�ǰ״̬��ɫ
						{
							if(((WireOut_State == 0)&&(Wire_Mode_Sel == Wire_Sel_None))//��Out��·����˿����Noneģʽ����Ҫ����
								||((WireOut_State == 0)&&(Wire_Mode_Sel == Wire_Sel_In_Only))//��Out��·����˿���ǵ�֧ģʽ����Ҫ����
								||((WireOut_State == 1)&&(Wire_Mode_Sel == Wire_Sel_Double)))//��In��·����˿����˫֧ģʽ����Ҫ����							
							
							{
							}
							else//�������������Ҫ����:����Outʱ˫֧/Un,��Outʱ��/None/Un
							{
								color=RED18;
							}
						}
						
						Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,color);  //���������ϲ����²�
						Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,color);   //���������ϲ����²�
						Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,color);  //����� 
					}    		 
		 }
		 else if(JRP_Sensor_Err)//������̽ͷ����
		 {
			 Working_Normal = 0;//��������ʱ������		
			 //��ʾͼ�Σ���˸���󲿷�==========================================
			 if(Disp_ERR_VHB80_Code!=5)
			 {
					Disp_ERR_VHB80_Code=5; 
					DISP_VHB80_PIC(GRAY18);
			 }	
			 if(Err_Event_Cnt == Err_Event_1)
			 {
						//color=RED18;  //��
				 color=BLUE18;	//��
			 }
			 //else
				 else if(Err_Event_Cnt == Err_Event_2)
			 {
						color=GRAY18;	//��
			 }
			 DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,color); 	
		 
		 }
		 else if((Wire_Mode_Sel==Wire_Sel_In_Only || Wire_Mode_Sel== Wire_Sel_None))//����ȷ�Ϻ�ĵ�֧���Ȼ��޼��Ȼ�·	
		{	

			 if(Work_State != UI_STATE_SCREENSAVER_MODE)  //û����Ļ����ʱ������ʾ����ģʽ
			 {
						 if(Disp_ERR_VHB80_Code!=6)
						 {
								Disp_ERR_VHB80_Code=6; 
								DISP_VHB80_PIC(BLUE18);
						 }

				 //�������� 
				if(Err_Event_Cnt == Err_Event_1) //		
				{	
				 if(Wire_Mode_Sel_Rem != Wire_Mode_Sel)
				 {				 
					 if(Wire_Mode_Sel == Wire_Sel_None)//�޼���˿
					 { 
									Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,GRAY18);  //���������ϲ����²�
									Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,GRAY18); 
								
									Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,GRAY18);  //���������ϲ����²�
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,GRAY18);   //���������ϲ����²�
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,GRAY18);  //����� 				
						} 
						else if(Wire_Mode_Sel==Wire_Sel_In_Only)
						{
									Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,BLUE18);  //���������ϲ����²�
									Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,BLUE18); 
									
									Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,GRAY18);  //���������ϲ����²�
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,GRAY18);   //���������ϲ����²�
									Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,GRAY18);  //����� 
						}
					}
					Wire_Mode_Sel_Rem = Wire_Mode_Sel;
				 }	
			 }			
		 }
		 else   //�������д�����ʾ
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

//void Event_1s(void)       //1�����¼� �ڲ���ʱ�ж��ñ�־
void LowTempDet1SFunc(void)
{
	uint16_t a,b,c;//�������¶ȴﵽ�趨�¶�-1
	static uint8_t  xdata   No_Water_Times=0;	
	static uint8_t RT_SHIDU_More_then_90_Cnt = 0;//��������90%�ﵽ10��(1��1��)	
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
	Humidity_No_Change_Sec++;//ʪ�Ȳ��ı�ļ�����������600ʱ����̽ͷʧЧ��ʪ�ȿ���ʧЧ
	
  if(HeaterPlate_State)  //
  {	
  	if(Last1s_RThumity==CONTROL_RT_SHIDU && Last1s_RTtemp==RT_Temp)	//������¶Ⱥ�ʪ��ͬʱ���ı�
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
 
	 //��ˮ����,Ӧͨ�������̵ļ��������ٶ��ж�ˮ��======================================
	 //����ˮ����ˮ��������¶�Ӧ�ûή��,���͵ķ��ȹ�����3������
	 //�м���˿����ˮ�жϣ��������¶ȸ���������¶ȣ��������¶���100�����ϣ������ʪ�ȵ���80%��������20%���ҵ��½�
	 //�޼���˿���жϣ����ˮ�ֲ�����������������¶Ƚϵͣ��������¶���100������
   if(HeaterPlate_State)
   {

   }
	 else
   {   	
   	    Clear_bit(ERR_Kind,Alarm_Const_NoWater);   	
   } 
	 
	 if(RT_SHIDU >= 900) //���ڿ��ټ�⣬ֻҪʪ�ȴﵽ90%����10S��Ȼ������2����С��60%�򱨾�
	 {		 
		 RT_SHIDU_More_then_90_Cnt++;
		 if(RT_SHIDU_More_then_90_Cnt >= 10)//����10�볬��90%
		 {
			 RT_SHIDU_More_then_90_Cnt = 10;
		 }
	 }
	 else
	 {
		 if(RT_SHIDU_More_then_90_Cnt < 10)//С��90%����10������������,10������������
		 {
			 RT_SHIDU_More_then_90_Cnt = 0;
		 }
	 }		 

	 //���ձ���	<60% 
   //if(Work_Min>=20||Work_Day>0 ||Work_Hour>0) //|| RT_SHIDU_More_then_90_Cnt >= 10)
	 if(Working_Normal >= 1200)
   {
		   if(RT_SHIDU<600) //����2��������60%����	 ��Ϊ�Ǹ��գ�������������
		   {  		   		
		       if(No_Water_Times<240)  No_Water_Times++;
			   if(RTD_Sensor_Err || SHIDU_Sensor_Err) //���������󲻿���ʪ���������ˮ����
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
	 
	 if(RT_SHIDU>650)//�¶ȴ���65%��ȡ����ˮ����
	 {
		 No_Water_Times=0;
		 Clear_bit(ERR_Kind,Alarm_Const_NoWater);
	 }

   //============================================================
	 //������¶ȴﵽʶ��180S 
	 //if(Work_Mode==Invasive_Mode) //�д�ģʽ
	 if(1) //
	 {
		if(RT_Temp >= Set_RT_Temp - 15) //���ڿ��ټ�⣬ֻҪ�ﵽ�趨�¶�-1.5��10S��Ȼ������2����С��60%�򱨾�
		{		 
			 RT_Temp_Reach_Set_Cnt++;
			 if(RT_Temp_Reach_Set_Cnt >= 180)//����180�볬���趨-1
			 {
				 RT_Temp_Reach_Set_Cnt = 180;
			 }
		}
		else
		{
			 if(RT_Temp_Reach_Set_Cnt < 180)//С���趨����10������������,10������������
			 {
				 RT_Temp_Reach_Set_Cnt = 0;
			 }
		}
	}	
		
	 if(Work_Mode==Noninvasive_Mode)//�޴�ģʽ
	 {
		 a = Const_NoninvasivePatientTemperature_Max+10;//���±�����ֵ
		 if(WireIn_State!=0)//�м���˿��֧��˫֧
		 {
			b = Const_NoninvasivePatientTemperature_Min-10;//�޴����±�����ֵ 29��
		 }
		 else 
		 {
			 b = 260;//�޻�·����˿ʱ�޴����±���Ϊ26��
		 }
		 //c = 700;//��ʪ������ֵ V2.0
		 c = 300;//��ʪ������ֵ V2.02 �޴�ģʽȡ����ʪ�ȱ���
	 }
	 else if(Work_Mode==Invasive_Mode) //�д�ģʽ
	 {
		 a = Const_InvasivePatientTemperature_Max+10;
		 if(WireIn_State!=0)//�м���˿��֧��˫֧
		 {
			 b = Const_InvasivePatientTemperature_Min-10;//�д����±�����ֵ 34��
		 }
		 else
		 {
			 b = 290;//�޻�·����˿ʱ�д�����Ϊ29��
		 }
		
		 c = 800;//��ʪ������ֵ
	 }

		if(Diplay_RTtemp>a)
		{
				if(HiTemp_Count<240)	HiTemp_Count++;
		}else
		{
				 HiTemp_Count=0;
		} 
		
/*		
		ΪӦ���ȱ��ʣ������µĵ��±����߼�����Ҫͬʱ�������������������±���
			1���д�ģʽ
			2����֧��˫֧���Ȼ�·ģʽ
			3�����߶˴ﵽ�趨�¶�-1�� ����180S����ֹ����
			4�����߶˵����趨�¶�-2���һ�·���ȹ���Ϊȫ����(190)����10S*/

//����˵��±���		
		//if(Work_Min>=20||Work_Day>0 ||Work_Hour>0)// || RT_Temp_Reach_Set_Cnt >= 180)	 //������±���
		if(Working_Normal >= 1200)//20����
		{
			 if((RT_Temp_Reach_Set_Cnt >= 180)//�ﵽ�¶�����180S
				//&&(WireIn_State!=0)//�м���˿��֧��˫֧
				&&(Diplay_RTtemp <= Set_RT_Temp - 30))//�¶ȵ����趨�¶�3��	//��Դ�жϻ�̽ͷ����		  
			 {
				 if(((WireIn_State!=0) && (Micro_Temp_In >= 190)))//�м���˿��֧��˫֧
					 //||(WireIn_State==0))//�޷���˿��һ����ȫ���ʼ��� �޷���˿��ʹ�� 
				
				 {
						if(LoTemp_Count<240) LoTemp_Count+= 12;	//���¶ȴﵽ�趨�¶Ⱥ�����10S�½����趨�¶�-2.5�ȣ����±���	
				 }
				 else 
				 {
					 //if(LoTemp_Count<240) LoTemp_Count++;	//���¶ȴﵽ�趨�¶Ⱥ�����10S�½����趨�¶�-2.5�ȣ����±���	
				 }
			 }
			 else if(Diplay_RTtemp<b)//55���ӿ�ʼ����¶ȹ��ͱ���
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

//����˵�ʪ����			
		//if(Work_Min>=20||Work_Day>0 ||Work_Hour>0)// || RT_SHIDU_More_then_90_Cnt >= 10)	 //�����ʪ����
		if(Working_Normal >= 1200)
		{
			 if(RT_SHIDU<c)	  //2014-07-23	  ��ʪ�ȱ���
			 {
					if(LoHumity_Count<250)   LoHumity_Count++;
			 }
			 else
					LoHumity_Count=0;
	
		}else
		{

			 LoHumity_Count=0;
		}	 
	 
//�����ڵ��±���
	 	//if(CQK_Temp >= Set_CQK_Temp - 10) //���ڿ��ټ�⣬ֻҪ�ﵽ�趨�¶�10S��Ȼ������2����С��60%�򱨾�
	  if(CQK_Temp >= 300) //���ڿ��ټ�⣬ֻҪ�ﵽ�趨�¶�10S��Ȼ������2����С��60%�򱨾�
		{		 
			 CQK_Temp_Reach_Set_Cnt++;
			 if(CQK_Temp_Reach_Set_Cnt >= 10)//����10��ﵽ
			 {
				 CQK_Temp_Reach_Set_Cnt = 10;
			 }
		}
		else
		{
			 if(CQK_Temp_Reach_Set_Cnt < 10)//10������������,10������������
			 {
				 CQK_Temp_Reach_Set_Cnt = 0;
			 }
		}	

		if(Working_Normal >= 1200)
		{
			 if((CQK_Temp < Const_NoninvasiveChamberTemperature_Min - 10)
				 &&(Work_Mode==Invasive_Mode))//�д�ģʽ���г����ڵ��±���
			 {
				 if(LoTemp_CQK_Count<240)
				 {
					  if(CQK_Temp_Reach_Set_Cnt>= 10)//���ﵽ���¶���ӿ��ٶ�
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
	 static uint8_t   xdata  Enable_Save_Data=0;  //�������ݼ���
	 uint16_t a,b,c;//
	 unsigned long Adress;	
   if(Enable_Save_Data>=1)
   {
     	Enable_Save_Data++;
     	if(Enable_Save_Data>=10) Enable_Save_Data=0;

   }
   if(Enable_Save_Data==2)
   {
     if((Run_Count-1)%256==0)//һ������Ϊ4K��ÿ�δ洢������Ϊ16�ֽڣ���256�����ݼ�Ϊ4K 256*16=4096;
     {
   	   //�������256������
   	    Adress=Run_Count-1;	 //����Ϊ257-1=256
   	    Adress=(Adress<<4);	 //	 256*16= 4096
   	    Adress=Adress+0x2000;//��ʼ��ַΪ0X2000����8192Ϊ�����ֽ�
   	    SPI_Erase_Sector(Adress);
     }
   }else  if(Enable_Save_Data==3)//�������
   {
		Adress=Run_Count-1;
		Adress=(Adress<<4);
		Adress=Adress+0x2000; 
		 
			{
				 RX8010_GetTime(SaveData);
				
				//LCD_ShowxNum(POS_RT_RH_X-40,5,16,3,SaveData[2],0x80,BLACK18); //
				
				 a = (SaveData[2]&0x0F)%10 + ((SaveData[2]>>4)& 0x03)*10;
				 SaveData[2] = a;//ת��Ϊ24Сʱ��Сʱ
				
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
				 SaveData[2] = ((b << 4) + c) | a;//ʱ��ת��ΪDS1302��ʽ
				 
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
		//SaveData[10]=CONTROL_RT_SHIDU/256;	//2015-06-04 �洢��ʵ��ʪ��
		//SaveData[11]=CONTROL_RT_SHIDU%256; 
		
		SaveData[12]=CQK_Temp/256;   	
		SaveData[13]=CQK_Temp%256;
		SaveData[14]=JEP_Temp/256;
		SaveData[15]=JEP_Temp%256;
		SPI_Write_nBytes(Adress,16,&SaveData[0]);

   }else if(Enable_Save_Data==4)  //������ݵ�ַ
   {
   	   Store_Data();   	 
   }else if(Enable_Save_Data==5)  //��������
   {
       //�����������,���¼���
       if(Run_Count>=44896) 
        SPI_Erase_Sector(0x000);
  
   }else if(Enable_Save_Data==6)  //��������
   {
       //�����������,���¼���
       if(Run_Count>=44896) 
       {	
         SPI_Erase_Sector(0x1000);        
         Run_Count=0;
       }  
   } else if(Enable_Save_Data==9)  //������
   {

   } 
}


void	Init_port(void) //��ʼ���˿�״̬--------------------------------
{
	uint16_t j=0;
	
	P1M1=0x08;  //P13Ϊ�������� Wire_out_FB
	P1M0=0x0;
	P2M1=0x70;//P24-P26ȫ�趨�ɿ�©���
	P2M0=0xF0;
	P3M1=0x18;  //P34,P33Ϊ�������� Wire_In_FB  Plant_FB 
	P3M0=0x64;  //P35,P36,P32Ϊǿ���  Heat_Plant_Out  Heat_Wire_In  Heat_JRP_EN_OUT  
	P4M1=0x00;  //IO Port initialization
	P4M0=0x09;  //P40,P43Ϊǿ���
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
		Work_State = UI_STATE_DATAREADER_MODE;//���ݴ������
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

void	Main_Init(void)	//�������ʼ��
{
	Get_RunCount();	 //�õ�������ݵ�����  

	Diplay_RTtemp = RT_Temp;
	
	CQK_Sensor_Err=0;
	RTD_Sensor_Err=0;
	JRP_Sensor_Err=0;
	SHIDU_Sensor_Err=0;

	Back_Color=BRIGHT_BLUE18 ;

	WireOut_State=0;
	WireIn_State=0;		

	Wire_Mode_Sel = Wire_Sel_Double;//Ĭ��Ϊ˫֧���Ȼ�·
	Wire_Mode_Mismatch = 0;//��·����˿ģʽ��ʵ�ʲ�ƥ���־

	Wire_Warm_Up_Sec = 0; //��<=300ʱ���������ȹ���
	Plate_Warm_Up_Sec = 0;

	Set_CQK_Temp_Comp = 360;//��ʼ��
	Humidity_No_Change_Sec = 0;//ʪ�Ȳ��ı�ļ���	
	
	JEP_Temp_Aim = Set_CQK_Temp + 100;     //������Ŀ���¶� 400 - 530
	PID_temp3.Sum_error = 0;
	PID_temp3.Ek = 0;
	PID_temp3.Ek1 = 0;
	PID_temp3.Ek2 = 0;
}



//����ģʽʱ��ʾ�¶�
//����������ʱ����������ʵʱ��
void ServiceMode_TempHumidy_Disp(void)
{
	uint16_t Disp,color;
	bit Disp_Err;
	Back_Color=WHITE18;	
	Disp_Err=0;		

	  if(CQK_Temp>=0x0FFF || No_CQKSensor_Times>20 ) //�������¶ȴ�����������
	  {
		 CQK_Sensor_Err=1;
		 CQK_Temp = 0;
    }
  	else
  		   CQK_Sensor_Err=0;
		
  	if(No_ReadData_SHIDU_Times>10)//�����ʪ�ȴ�����������
  	 {
	     SHIDU_Sensor_Err=1;
			RT_SHIDU = 0; 
		}		
  	else
  		  SHIDU_Sensor_Err=0;
		
    if(No_ReadData_Temp_Times>20 || ERR_RT_Temp_Times>10)//������¶ȴ����������� 2014-07-03
  	 { 	 
				RTD_Sensor_Err=1;
				RT_Temp = 0; 		
				Diplay_RTtemp = 0;
			}
			else
  		   RTD_Sensor_Err=0;
			
    if( No_HeatSensor_Times>20)//�����̴�����������
		{
					JRP_Sensor_Err=1;
				JEP_Temp = 0;
		}

  	else
  			JRP_Sensor_Err=0;

	//������¶�
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
    Draw_Rectangle_Real(POS_RT_TEMP_X+1-8,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8-8,POS_RT_TEMP_Y+71,WHITE18); //ȥ��С����
  }else
  {
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y,(Disp/100)%10,color);  
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+34,Disp%100/10,color);   
    DISP_TEMP_30X56(POS_RT_TEMP_X-8,POS_RT_TEMP_Y+34*2+5,Disp%10,color);    
    Draw_Rectangle_Real(POS_RT_TEMP_X+1-8,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8-8,POS_RT_TEMP_Y+71,color); //����
  }		  

	//�������¶�
	Disp=CQK_Temp; 
  if(Disp>=1000)
  {
    DISP_RH_17X40(55,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20,(Disp%1000)/100,color);   
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20*2+5,(Disp%100)/10,color);    
    Draw_Rectangle_Real(56,POS_RT_TEMP_Y+40,61,POS_RT_TEMP_Y+43,WHITE18); //ȥ��С����
  }else
  {
    DISP_RH_17X40(55,POS_RT_TEMP_Y,(Disp/100)%10,color);  
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20,Disp%100/10,color);   
    DISP_RH_17X40(55,POS_RT_TEMP_Y+20*2+5,Disp%10,color);    
    Draw_Rectangle_Real(56,POS_RT_TEMP_Y+40,61,POS_RT_TEMP_Y+43,color); //����
  }
   
	//�������¶�
	Disp=JEP_Temp;
	if(Disp>=1000)
	{
	DISP_RH_17X40(5,POS_RT_TEMP_Y,(Disp/1000)%10,color);  
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20,(Disp%1000)/100,color);   
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20*2+5,(Disp%100)/10,color);    
	Draw_Rectangle_Real(51,POS_RT_TEMP_Y+40,56,POS_RT_TEMP_Y+43,WHITE18); //ȥ��С����
	}else
	{
	DISP_RH_17X40(5,POS_RT_TEMP_Y,(Disp/100)%10,color);  
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20,Disp%100/10,color);   
	DISP_RH_17X40(5,POS_RT_TEMP_Y+20*2+5,Disp%10,color);    
	Draw_Rectangle_Real(6,POS_RT_TEMP_Y+40,11,POS_RT_TEMP_Y+43,color); //����
	}	

	//��ʾʪ��
	{	
	 DISP_RH_17X40(POS_RT_RH_X+2,POS_RT_RH_Y,RT_SHIDU%1000/100,color);
	 DISP_RH_17X40(POS_RT_RH_X+2,POS_RT_RH_Y+19,RT_SHIDU%100/10,color);
	}   
}

//���趨�Ĳ���д�뵽STC EEPROM
//ע�⣺����ʱ�����FLASH�����ݶ��뵽RAM��
void Setting_write_to_flash(void)
{ 
		STC_IAP_Sector_Erase(DATA_FLASH_ADDRESS_Set_RT_YCTemp);           //������������
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_First, data_flash.First);//�� DEBUG_DATA д�� EEPROM
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Work_Mode, data_flash.Work_Mode);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_RT_WCTemp, data_flash.Set_RT_WCTemp);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_RT_YCTemp, data_flash.Set_RT_YCTemp);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_In_Exp_Ratio, data_flash.In_Exp_Ratio);//
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_CQK_WCTemp, data_flash.Set_CQK_WCTemp);//
    STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Set_CQK_YCTemp, data_flash.Set_CQK_YCTemp);//
		STC_IAP_Byte_Program(DATA_FLASH_ADDRESS_Language, data_flash.Language);//
}

//��Ĭ�ϵĲ���д�뵽STC EEPROM
void Write_Default_Setting_to_flash(void) //��Ĭ�ϳ����趨д��FLASH
{
				data_flash.First = 'Z';
			data_flash.Work_Mode = Invasive_Mode;//�д�
			data_flash.Set_RT_WCTemp = (340/5);//
			data_flash.Set_RT_YCTemp = (390/5);//
			data_flash.In_Exp_Ratio = 4;//1:1.3
			data_flash.Set_CQK_WCTemp = (310/5);//
			data_flash.Set_CQK_YCTemp = (360/5);//
			//data_flash.Language = Lan_English;

			Setting_write_to_flash();//д�뵽FLASH
}


void Mem_Flash_Recall(void) //��ȡ�ϴεļ��䲢��֤����
{
	data_flash.First = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_First);    //��EEPROM��ֵ
	data_flash.Work_Mode = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Work_Mode);    
	data_flash.Set_RT_WCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_RT_WCTemp);    
	data_flash.Set_RT_YCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_RT_YCTemp);    
	data_flash.In_Exp_Ratio = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_In_Exp_Ratio);  
	data_flash.Set_CQK_WCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_CQK_WCTemp);    
	data_flash.Set_CQK_YCTemp = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Set_CQK_YCTemp); 
	data_flash.Language = STC_IAP_Byte_Read(DATA_FLASH_ADDRESS_Language); 
	
	if( //У�������Ƿ���ȷ
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
			||(data_flash.Language > Lan_French)//���4������
	  )
	  {
			data_flash.Language = Lan_English;
			Write_Default_Setting_to_flash();//��Ĭ�ϳ����趨д��FLASH
	  }
}



void Load_Settings_Before_Choice(void)//��ѡ�񷽿����֮ǰ��Ҫ��������ݣ���ͬʱ��������ѡ�����������
{
	if(defalut_mode == Load_Fac_MODE)  //���ǹ���Ĭ���趨  
	{
		Write_Default_Setting_to_flash();//��Ĭ�ϳ����趨д��FLASH
	}

		Work_Mode = data_flash.Work_Mode;  //����ģʽ
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

/*�͹���ģʽ
����---�������ģʽʱ��a,�����̼���ʱ�����޶���50���¶Ȳ�����50��
											 b,��·����˿����ʱ�����޶���40
Ŀ��---����������˻�����ڵ��±���ʱ��������Դ����·���µ�ԭ���⣬���п�����̽ͷ���䣬
				����ȫ�����ȣ������û�δ��Ԥʱ��ȫ����������ȴ�����£��û����ܻ���Ϊ���������ع���
				��������ģʽ���ȣ�����ܻ�����̽ͷ���䵼�´��ʼ��ȣ������¶��������ա�
�������ģʽ����---����˻�����ڳ��ֵ��±��������б�����ʱ
�˳�����ģʽ����---���±��������������������ʱ�˳�90S
*/
void LowPowerModeFunc(void)	 
{	
	if((LoTemp_Count > 120 || LoTemp_CQK_Count > 120)||(Bit_is_one(ERR_Kind,Alarm_Const_NoWater))) //��ˮ(����)����
	{
			if(		
				   (ERR_Kind & 0x01)!=0//����������
				|| (ERR_Kind & 0x02)!=0//����
				|| Wire_Mode_Mismatch == 1  //����˿δѡ
				|| HeaterPlate_State==0)//ˮ��δװ��
			{	
			}
			else	
			{			
				if(AlarmSoundPauseStatus == 0)//δ���¾�����
				{
					Low_Power_Mode_Flag = 1;//�͹���ģʽ��־
					if(JEP_Temp<=450)
					{				
						Micro_Temp_Val=50;  //ֹͣ����
					}
					else
					{
						Micro_Temp_Val=0;  //ֹͣ����
					}
					Micro_Temp_In=40;
					Micro_Temp_Out=Micro_Temp_In;			
					Micro_Adj_Mode_Test = 4;//����ģʽ����
				}
				else
					Low_Power_Mode_Flag = 0;
			}
	}
	else
		Low_Power_Mode_Flag = 0;
}




//����PID�㷨
//��U = U(k)-U(k-1) = Kp*[e(k)-e(k-1)]+Ki*e(k)+Kd*[e(k)-2*e(k-1)+e(k-2)]
//* ���У�Kp��Ki��Kd�ֱ�Ϊ���������ֺ�΢�ַŴ�ϵ����u(k)��ʾ��k������ʱ��
//�Ŀ�����,e(k)��ʾ��k������ʱ�̵ĺ�������ƫ��.
//PIE_UK = Kp*(Ek-Ek1)+Ki*EK+Kd*(Ek+Ek2-EK1)

//struct PID
//	 	{
//		 	int Uk; //�ܵĿ�����
//		 	int Uk1;//�ϴε��ܿ�����
//		 	int Sum_error;//�������
//		 	int Ek;//��ǰ���������
//		 	int Ek1;//ǰһ�ε������
//		 	int Ek2;//ǰ���ε������       
//       }PID_temp;//

//�з���˿ʱ�Ļ��߶��¶ȿ���PID�㷨---���ݻ��߶�Ŀ���¶Ⱥ�ʵ���¶ȣ���������Ȼ�·����ֵ  ÿ1�빤��һ��
void PID_Calc(void) //�趨�¶����400,���000,ʵ��0-800,  
{

 	int Vlaue1,tmp;
 	static int PID_Kp = 40;//����ϵ�� 5��Ϊ1500;1��Ϊ300;0.1��Ϊ30
 	static int PID_Ki = 8;//����ϵ��
 	static int PID_Kd = 20;//΢��ϵ�� 	

	if(PID_temp.Ek > 100)//10��Ϊ���� 
	{
		PID_temp.Ek = 100;
	}
	if(PID_temp.Ek < -100)
	{
		PID_temp.Ek = -100;
	}
	
 	PID_temp.Sum_error += PID_temp.Ek;//�������
 	if(PID_temp.Sum_error > 3500) //0.1���������250һ����
 	{
 		PID_temp.Sum_error = 3500;
 	}
 	if(PID_temp.Sum_error < -3500)
 	{
		
 	}
	Vlaue1 = PID_Kp * PID_temp.Ek;//���� ���100*30=3000;  ���*����ϵ��(�ٶ����5�ȼ�50*25=1250)
	
	tmp = PID_temp.Sum_error * PID_Ki / 50; //���� ���3000*2/100=60,һ������������250��  250*5/15*3=250;
	Vlaue1 += tmp;
 	tmp = (PID_temp.Ek - (2 * PID_temp.Ek1) + PID_temp.Ek2) * PID_Kd;//΢�� 
	PID_temp.Sum_error = PID_temp.Sum_error + tmp;//2015-06-16 ��΢�ּ�����ֺ�
 	Vlaue1 += tmp;
	PID_temp.Uk = Vlaue1;// PID_temp.Uk1 + Vlaue1;       // ��� /100���Ϊ250��
 	if(PID_temp.Uk > 780) 
 		PID_temp.Uk = 780;
	else if(PID_temp.Uk < 0)
		PID_temp.Uk = 0;//����Ϊ����

	PID_temp.Uk /= 4;//���190; 	
		
 	PID_temp.Ek2 = PID_temp.Ek1;        // �����µ�K-1������ֵ
 	PID_temp.Ek1 = PID_temp.Ek;
}


//������������PID�㷨--- ���ݼ����̵�Ŀ���¶Ⱥ�ʵ���¶ȣ���������Ȼ�·����ֵ  ÿ1�빤��һ��
void HeatPlate_PID_Calc(void) //�趨�¶����400,���000,ʵ��0-800,  
{

 	int Vlaue1,tmp;
//	uint16_t  Val_Calc;
 	static int PID_Kp2 = 38;//����ϵ�� //����ϵ�� 10��Ϊ4000/20=200;1��Ϊ400/20=20;0.1��Ϊ2
 	static int PID_Ki2 = 8;//����ϵ��
 	static int PID_Kd2 = 20;//΢��ϵ�� 	

	if(PID_temp2.Ek > 100)//10��Ϊ���� 
	{
		PID_temp2.Ek = 100;
	}
	if(PID_temp2.Ek < -100)
	{
		PID_temp2.Ek = -100;
	}
	
 	PID_temp2.Sum_error += PID_temp2.Ek;//�������
 	if(PID_temp2.Sum_error > 3500) //0.1���������250һ����
 	{
 		PID_temp2.Sum_error = 3500;
 	}
 	if(PID_temp2.Sum_error < -3500)
 	{
 		PID_temp2.Sum_error = -3500;
 	} 	

	Vlaue1 = PID_Kp2 * PID_temp2.Ek;//���� ���100*30=3000;  ���*����ϵ��(�ٶ����5�ȼ�50*25=1250)
	
	tmp = PID_temp2.Sum_error * PID_Ki2/50; //���� ���3000*2/100=60,һ������������250��  250*5/15*3=250;
	Vlaue1 += tmp;
 	tmp = (PID_temp2.Ek - (2 * PID_temp2.Ek1) + PID_temp2.Ek2) * PID_Kd2;//΢�� 
	PID_temp2.Sum_error = PID_temp2.Sum_error + tmp;//2015-06-16 ��΢�ּ�����ֺ�
 	Vlaue1 += tmp;
	PID_temp2.Uk = Vlaue1;//;       // ��� /100���Ϊ250��
 	if(PID_temp2.Uk > 780) 
 		PID_temp2.Uk = 780;
	else if(PID_temp2.Uk < 0)
		PID_temp2.Uk = 0;//����Ϊ����

	PID_temp2.Uk /= 4;//���195; 		

 	PID_temp2.Ek2 = PID_temp2.Ek1;        // �����µ�K-1������ֵ
 	PID_temp2.Ek1 = PID_temp2.Ek;
}


//������Ŀ���¶ȵ�PID�㷨--- ���ݳ����ڵ�Ŀ���¶Ⱥ�ʵ���¶ȣ������������Ŀ���¶ȵ�ƫ��, ÿ20�빤��һ��
static void HeatPlateTemp_Aim_PID_Calc(void) // 
{
 	int Vlaue1,tmp;
//	uint16_t  Val_Calc;
 	static int PID_Kp3 = 40;//����ϵ�� 10��Ϊ4000/100=40;1��Ϊ400/100=4;0.1��Ϊ0.4
 	static int PID_Ki3 = 1;//����ϵ��
 	static int PID_Kd3 = 80;//΢��ϵ�� 	

	if(PID_temp3.Ek > 100)//10��Ϊ���� 
	{
		PID_temp3.Ek = 100;
	}
	if(PID_temp3.Ek < -100)
	{
		PID_temp3.Ek = -100;
	}
	
 	PID_temp3.Sum_error += PID_temp3.Ek;//�������
 	if(PID_temp3.Sum_error > 1000) //0.1���������250һ����
 	{
 		PID_temp3.Sum_error = 1000;
 	}
 	if(PID_temp3.Sum_error < -1000)
 	{
 		PID_temp3.Sum_error = -1000;
 	} 	

	Vlaue1 = PID_Kp3 * PID_temp3.Ek;//���� ���100*40=4000;  ���*����ϵ��(�ٶ����5�ȼ�50*25=1250)

	
	tmp = PID_temp3.Sum_error * PID_Ki3; //���� ���3500*8/100=280
	Vlaue1 += tmp;
 	//tmp = (PID_temp3.Ek - (2 * PID_temp3.Ek1) + PID_temp3.Ek2) * PID_Kd3;//΢�� 
	tmp = (2 * PID_temp3.Ek - PID_temp3.Ek1 - PID_temp3.Ek2) * PID_Kd3;//΢�� 
	PID_temp3.Sum_error = PID_temp3.Sum_error + tmp;//��΢�ּ�����ֺ�
 	Vlaue1 += tmp;
	PID_temp3.Uk = Vlaue1;// PID_temp.Uk1 + Vlaue1;       // ��� /100���Ϊ250��
 	if(PID_temp3.Uk > 5000) 
 		PID_temp3.Uk = 5000;
	else if(PID_temp3.Uk < -5000)
		PID_temp3.Uk = -5000;//����

	PID_temp3.Uk /= 100;//���50; 		

 	PID_temp3.Ek2 = PID_temp3.Ek1;        // �����µ�K-1������ֵ
 	PID_temp3.Ek1 = PID_temp3.Ek;
}


//�޻�·����˿ʱ�Ļ��߶��¶ȵ���
static uint16_t NoneWire_Warm_Up_Sec=0;
static void PatientTemp_NoneHeatWire_Adj(void)
{
	  static int err_sum=0;
   	static int RT_Temp_Mem = 0;
//	static uint16_t WarmUpstate=0;
	  int Calc_int=0;
		uint16_t Val_Calc;	   	

		{
				if(CQK_Temp < 330)//���������¶ȵ���33����ȫ���ʼ���
				{
					//Micro_Temp_Val = 190;
					//����Ϊ�������¶ȵ�PID����
					
					PID_temp2.Ek = 980 - JEP_Temp;//��ǰ������� �趨�¶�-��ǰʵ���¶�
					HeatPlate_PID_Calc(); 
					Val_Calc = (uint8_t)PID_temp2.Uk;	//���������ʱ��
					Micro_Temp_Val = Val_Calc;//���ڼ��㣬���ⳬ��190�����ж�	
					
					Temp2_Int = JEP_Temp;
					
				}
				else
				{	
					NoneWire_Warm_Up_Sec++;
					if(NoneWire_Warm_Up_Sec > 2000)NoneWire_Warm_Up_Sec = 2000;
					if(NoneWire_Warm_Up_Sec < 3)//00)//10�������趨
					{
						//Temp2_Int = 500 + Set_RT_Temp - 300;//�������¶��趨��50��-60��
						if(Temp2_Int < (550 + (Set_RT_Temp - 300)*2))//�������¶��趨��50��-60��
							Temp2_Int = 550 + (Set_RT_Temp - 300)*2;
						  RT_Temp_Mem = RT_Temp;
					}
					else
					{	
						err_sum = err_sum + Set_RT_Temp - RT_Temp;//����
						if(err_sum > 180)err_sum=180;
						else if(err_sum < -180)err_sum=-180;//ÿ��10��(100)��30000/100=300,��Ϊ5��������
						
						//if((RT_Temp - Set_RT_Temp) >= 1){if(err_sum > 0)err_sum = 0;}//�ﵽ�趨�¶ȣ�����Ϊ��
						//else if((Set_RT_Temp - RT_Temp) >= 1){if(err_sum < 0)err_sum = 0;}//δ�ﵽ�趨�¶ȣ�����Ϊ��
						
						if((Work_State != UI_STATE_SCREENSAVER_MODE)&&(Test_Mode_Dis_Jrp_Ctl == 1))
						{	//8��ʾ��λΪ0Ҳ��ʾ,0��ʾ�ǵ�����ʾ ������
							Back_Color=WHITE18;
							if(err_sum >=0)LCD_ShowxNum(220,10,16,1,1,0x80,BLACK18); //1-��ʾΪ����
											  else LCD_ShowxNum(220,10,16,1,0,0x80,BLACK18); //0-��ʾΪ����
							LCD_ShowxNum(220,20,16,5,abs(err_sum),0x80,BLACK18); //�����̼��ȵ�ģʽ��ʾ
						}						

						NoneWire_Heat_Sec++;
						if(NoneWire_Heat_Sec >= 180)//3���ӵ���һ���¶�
						{
							NoneWire_Heat_Sec = 0;//-Temp2_Int
							
							Calc_int = err_sum;
							//if(Calc_int > 6000)Calc_int=6000;
							//else if(Calc_int< -6000)Calc_int=-6000;//ÿ��5��(50)��2��������Ϊ6000
							
						  Temp2_Int = Temp2_Int + (Set_RT_Temp - RT_Temp)/6 + (RT_Temp_Mem-RT_Temp)/2 + Calc_int/40;//(400-300)/10
							//if(Temp2_Int > 1050)Temp2_Int = 1050;
							if(Temp2_Int > 980)Temp2_Int = 980;

							RT_Temp_Mem = RT_Temp;
						}
					}						

					//����Ϊ�������¶ȵ�PID����
					PID_temp2.Ek = Temp2_Int - JEP_Temp;//��ǰ������� �趨�¶�-��ǰʵ���¶�
					HeatPlate_PID_Calc(); 
					Val_Calc = (uint8_t)PID_temp2.Uk;	//���������ʱ��
					if(RT_Temp > (Set_RT_Temp + 10))
					{
						Micro_Temp_Val = 0;//����1�� 
						
					}
					else	
						Micro_Temp_Val = Val_Calc;//���ڼ��㣬���ⳬ��190�����ж�	
			}				
		}
}

//�������н���,����ȷ�ϼ�ȷ����·��ģʽ
void WireInOut_State_Confirm(void)
{
			if((WireIn_State > 0) && (WireOut_State > 0)) //˫�ܼ���ģʽ
			{
					Wire_Mode_Sel = Wire_Sel_Double;	
			} 
			else if((WireIn_State > 0) && (WireOut_State == 0)) //���ܼ���ģʽ
			{
					Wire_Mode_Sel = Wire_Sel_In_Only;	
			}
			else if((WireIn_State == 0) && (WireOut_State == 0)) //�޼���˿ģʽ
			{
					Wire_Mode_Sel = Wire_Sel_None;	
			}	
}

void HeaterPlateWireDriveFbTask(void)//16
{
	static uint8_t Micro_Temp_CLK = 0;
	static uint8_t xdata   Check_WireInTimes = 0;
	static uint8_t xdata   Check_WireOutTimes =0 ;
	static uint8_t WireIn_FB_Count = 0;     //��������In�����ź�
	static uint8_t WireOut_FB_Count = 0;     //��������Out�����ź�
	static uint8_t NoHpCheck_Times = 0;//�޼����̵Ĵ���.
	
	Micro_Temp_CLK++;
	if(Micro_Temp_CLK > 200)//ʱ��
	{		
			Micro_Temp_CLK = 0;
		
//ÿ�����ڼ��������Ƿ����
			 HP_CNT_Int_End_Rem = HP_CNT_Int;
			if(HP_CNT_Int < 1)//δ��⵽
			{
				NoHpCheck_Times++;
				if(NoHpCheck_Times >=2)
				{
					NoHpCheck_Times = 2;
					HeaterPlate_State=0; //δ��⵽������
				}
			}
			else
			{
				NoHpCheck_Times = 0;
				HeaterPlate_State=1; //�������̴���					
			}
			HP_CNT_Int = 0;//����
	 }
	 
//����������	
	 if (Micro_Temp_Val>Micro_Temp_CLK)
	 {
			Heat_JRP_Port=1;
	 }	 
	 else
	 {
			Heat_JRP_Port=0;
	 }
	
//INSP��֧�����������ͼ��
	if (Micro_Temp_In>Micro_Temp_CLK)
	{
			 Heat_WireIn_Port=1;
			 Heat_WIRE_EN_OUT=0;//��ֹ��һ����	
	}
	else  //������ʱ�ļ��
	{	 	  
		  if(Heat_WireIn_Port!=0)
	 	  {
	  	   Heat_WireIn_Port=0;
				 Heat_WIRE_EN_OUT=1;//��ֹ��һ����	  	
	  	   Check_WireInTimes=0;
	  	   WireIn_FB_Count=0;
	  	}
			else
	  	{
	  		Check_WireInTimes++;
	  		if(WireIn_Exist_Port==0) WireIn_FB_Count++;
	  		if(Check_WireInTimes>4)
	  		{
	  			 if(WireIn_FB_Count<3)   //������⵽��β����ȱ���
	  			 {
						 WireIn_State=0; //δ��⵽����˿						 
	  			 }
	  			 else
	  			 {						 
						 WireIn_State=1; //û���ȣ������⵽����˿
	  			 }
	  			 Check_WireInTimes=0;
	  			 WireIn_FB_Count=0;
	     }
	   }
	}			
	
//EXP��֧�����������ͼ��
	if (Micro_Temp_Out>Micro_Temp_CLK)
	{
			 Heat_WireOut_Port=1;
			 Heat_WIRE_EN_OUT = 0;//��ֹ��һ����	  
	 }
	 else
	 {
	 	  if(Heat_WireOut_Port!=0)
	 	  {
	  	   Heat_WireOut_Port=0;
				 Heat_WIRE_EN_OUT = 1;//��ֹ��һ����	  
	  	   Check_WireOutTimes=0;
	  	   WireOut_FB_Count=0;
	  	}else
	  	{
	  		Check_WireOutTimes++;
	  		if(WireOut_Exist_Port==0) WireOut_FB_Count++;
	  		if(Check_WireOutTimes>4  )
	  		{
	  			 if(WireOut_FB_Count<3)   //������⵽��β����ȱ���
	  			 {
	  			 	  WireOut_State=0; //û���ȣ������⵽����˿
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

			
											
											