#ifndef __GUI__
#define __GUI__

		#define Lan_English 				0 //Ӣ��
		#define Lan_Portuguese 			1 //��������
		#define Lan_Spanish  			  2 //��������
		#define Lan_French   			  3 //����
		
		
typedef enum
{
  DISPLAY_Temperature_Patient = 0,
	DISPLAY_Temperature_Chamber,
}DISPLAY_Temp_Kind;

extern DISPLAY_Temp_Kind Display_Temp_Kind;//��ʾ�¶ȶ���



void	HmiEnterToWorkStateFunc(WORK_STATUS State);//����ĳ������ģʽ����
void  HmiServiceModeFunc(void);//ServiceMode����
void	HmiPostFunc(void);//POST���� 
void  HmiPostFunc_Tik_Cnt(void);
void	HmiFactoryDefaultFunc(void);//����ѡ�����
void  HmiFactoryDefault_Tik_Cnt(void);
void	HmiLanguageFunc(void);//����ѡ�����
void	HmiNon_InvasiveSelFunc(void);//�д��޴�ѡ�����
void  HmiNon_InvasiveSel_Tik_Cnt(void);
void	HmiRunningFunc(void);//�д��޴�ѡ�����
void	HmiSetTempPatientFunc(void);//���߶��¶����ý���
void	HmiSetTempChamberFunc(void);//�����ڶ��¶����ý���
void	HmiSetInExpFunc(void);//InExp���ý���
void	HmiSetTimeFunc(void);//ʱ���趨
void	HmiScreenSaverModeFunc(void);//����ģʽ
void  HmiScreenSaverMode_Tik_Cnt(void);
//void  HmiScreenSaverModeFunc_TimeRestFunc(void);//ע�������ģʽʱҪ��һ��
void  DisPlayTime(unsigned char TempTime[7],unsigned char TimeKind);//��ʾʱ��




#endif