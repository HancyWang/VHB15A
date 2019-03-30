#ifndef __ACTIVITYFUNCTIONS__
#define __ACTIVITYFUNCTIONS__   


//����ѡ��ʹ����һ���û����趨��Ĭ���趨
#define  Load_User_Pre_MODE      0 //��һ���û����趨
#define  Load_Fac_MODE           1 // ����Ĭ���趨

//�д�ģʽ,�޴�ģʽ����
#define  Noninvasive_Mode      0 //�޴�ģʽ
#define  Invasive_Mode         1 //�д�ģʽ

#define Const_InvasivePatientTemperature_Min       350    //Patient side temperature Minimum limit Invasive mode 
#define Const_InvasivePatientTemperature_Max       400	  //Patient side temperature Maximum limit Invasive mode 
#define Const_NoninvasivePatientTemperature_Min    300    //Patient side temperature Minimum limit Noninvasive mode 
#define Const_NoninvasivePatientTemperature_Max    370    //Patient side temperature Maximum limit Noninvasive mode 

#define Const_InvasiveChamberTemperature_Min       340    //
#define Const_InvasiveChamberTemperature_Max       430	   //
#define Const_NoninvasiveChamberTemperature_Min    300
#define Const_NoninvasiveChamberTemperature_Max    320    //Patient side temperature Maximum limit Noninvasive mode 

#define Alarm_Const_Sensor      		0   //���������� 
#define Alarm_Const_HiTemp      		1   //���±���
#define Alarm_Const_LoTemp     			2   //���±���
#define Alarm_Const_LoHumity   			3   //��ʪ�ȱ���
#define Alarm_Const_NoWater     		4   //��ˮ����
#define Alarm_Const_RTD_Sensor      5   //����˴���������
#define Alarm_Const_CQK_Sensor      6   //�����ڴ���������
#define Alarm_Const_JRP_Sensor      7   //�����̴���������

#define HeatingTemperature_MAX      1050   //��������������¶�

void 	Fun_Null(void);
void	Init_port(void); //��ʼ���˿�״̬--------------------------------
void 	Main_Init(void);//��ʼ��
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
void  Setting_write_to_flash(void);// ���趨�Ĳ���д�뵽STC EEPROM
void  Load_Settings_Before_Choice(void);//��ѡ�񷽿����֮ǰ��Ҫ��������ݣ���ͬʱ��������ѡ�����������
void  ServiceMode_TempHumidy_Disp(void);
void  Mem_Flash_Recall(void);//��ȡ�ϴεļ���
void  WireInOut_State_Confirm(void);//�������н���,����ȷ�ϼ�ȷ����·��ģʽ
void  Error_Base_HeaterWire_DISP_Enable(void);//����ʱ����ر�־,�Ա�����ʱ��ʾ�������ߵ�ͼ��
void  HeaterPlateWireDriveFbTask(void);//

extern struct stc_data_flash data_flash;
extern uint8_t  Work_Mode;		 
extern uint8_t defalut_mode;	//Ԥ��ģʽѡ��
extern uint8_t show_str[16]; 
extern uint8_t  In_Exp_Ratio;	 //In��Exp���ȷ�ʽ�ı���  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
extern uint8_t Micro_Temp_Val;   //���Ϊ201;��СΪ0,1��ʾ10ms����ʱ��
extern uint16_t Micro_Temp_In;
extern uint16_t Micro_Temp_Out;
		 
//ʵʱ����
extern int  xdata  		RT_Temp;            //������¶�
extern int  xdata  		JEP_Temp;           //�������¶�
extern long  xdata  	CQK_Temp;           //�������¶�
		 
extern int  Set_RT_Temp;         //������¶��趨ֵ
extern uint16_t  Set_RT_WCTemp;       //�����޴��¶�
extern uint16_t  Set_RT_YCTemp;       //�����д��¶�
extern uint16_t  Set_CQK_Temp;        //�������¶�
extern uint16_t  Set_CQK_WCTemp;      //�����޴��¶�
extern uint16_t  Set_CQK_YCTemp;      //�����д��¶�
extern uint8_t RT_Temp_Reach_Set_Cnt;//�����¶ȴﵽ�趨�¶�-1.5
extern uint8_t CQK_Temp_Reach_Set_Cnt;//�������¶ȴﵽ30
extern uint8_t In_Exp_Ratio;	 //In��Exp���ȷ�ʽ�ı���  //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
extern uint16_t  xdata Diplay_RTtemp;//��ʾ�¶�
extern uint8_t WireIn_State;      //������In״̬ 1-�� 0-��
extern uint8_t WireOut_State;     //������Out״̬	  1-�� 0-��
extern uint8_t HeaterPlate_State; //������״̬ 1-�� 0-��
extern uint16_t  HP_CNT_Int;				//�������жϼ�����

extern uint8_t  ERR_Kind;
extern bit Wire_Mode_Mismatch; //��·����˿ģʽ��ʵ�ʲ�ƥ��		

extern uint16_t  	HP_CNT_Int_End_Rem;//8S����ʱ�������жϼ�����

extern uint8_t  xdata  SaveData[16];
extern uint8_t  xdata   Test_Mode_Dis_Jrp_Ctl; //�Ӳ���ģʽ��������ģʽ����ʾ�����̿��Ƶ�����	
extern uint8_t AlarmSoundPauseStatus;//����������ͣ 0-�������� 1-����������ͣ(������ʱ��Ч)
extern uint8_t AlarmInfoIndex;//
extern uint8_t No_HeatSensor_Times;//�������¶ȴ���������Ӧ����
extern uint16_t xdata     Working_Normal;//��������������ÿ���1���������ã��ı�����Ҳ����

#define DISPLAY_TEST_DATA_EN_DIS_DEF 0//��ʾ����
		 
 
#endif