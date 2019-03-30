#include "all.h"

///ע�⣺ʱ��Ϊ100mS


KEY_STATE_DEF Key_State_Present;

//==================UP������������-1============================

static void	KeyUpPressShortAction(void);
static void	KeyUpPressLongFirstAction(void);    //�������������Ӧ����
static void	KeyUpPressLongRepeatAction(void);  //�������³���,��+��-

const struct Key_Multifun_Element Key_UP_Element = 
{
  KEY_STATE_PressValue,       	//PressValue
  KEY_STATE_ReleaseValue,     	//ReleaseValue
  10,                         	//TimOf_Low_High ���µ���������Ӧʱ��
  20,                         	//TimOfSe ���γ�����Ӧʱ��
  5,                          	//TimOfLg ��������������״̬ʱ��
  KeyUpPressShortAction,      	//void (*)(void)�̰����ɵ͵��ߵ���
  KeyUpPressLongFirstAction,    //�������������Ӧ����
  KeyUpPressLongRepeatAction,  //�������³���,��+��-
};

struct Multifun_Key Key_UP =
{
  &KeyToFuncVal,		        //������ֵ
  &Key_UP_Tik,	//����ʱ��
  1,			      //����״̬
};

unsigned char Key_UP_Tik = 0;
void Key_UP_TikFunc(void)
{
    if(Key_UP_Tik < 250)
    {
       Key_UP_Tik++;
    }
}

static void	KeyUpPressShortAction(void)
{
	Key_State_Present = KEY_STATE_UP_Short;	
}

static void	KeyUpPressLongFirstAction(void) //�������������Ӧ����
{
	Key_State_Present = KEY_STATE_UP_Long_First;	
}    

static void	KeyUpPressLongRepeatAction(void) //�������³���,��+��-
{
	Key_State_Present = KEY_STATE_UP_Long_Repeat;
}

//------------------------end of O define------------------

//==================DOWN������������-1============================

static void	KeyDownPressShortAction(void);
static void	KeyDownPressLongFirstAction(void);    //�������������Ӧ����
static void	KeyDownPressLongRepeatAction(void);  //�������³���,��+��-

const struct Key_Multifun_Element Key_Down_Element = 
{
  KEY_STATE_PressValue,       	//PressValue
  KEY_STATE_ReleaseValue,     	//ReleaseValue
  10,                         	//TimOf_Low_High ���µ���������Ӧʱ��
  20,                         	//TimOfSe ���γ�����Ӧʱ��
  5,                          	//TimOfLg ��������������״̬ʱ��
  KeyDownPressShortAction,      	//void (*)(void)�̰����ɵ͵��ߵ���
  KeyDownPressLongFirstAction,    //�������������Ӧ����
  KeyDownPressLongRepeatAction,  //�������³���,��+��-
};

struct Multifun_Key Key_Down =
{
  &KeyToFuncVal,		        //������ֵ
  &Key_Down_Tik,	//����ʱ��
  1,			      //����״̬
};

unsigned char Key_Down_Tik = 0;
void Key_Down_TikFunc(void)
{
    if(Key_Down_Tik < 250)
    {
       Key_Down_Tik++;
    }
}


static void	KeyDownPressShortAction(void)
{
	Key_State_Present = KEY_STATE_Down_Short;	
}

static void	KeyDownPressLongFirstAction(void) //�������������Ӧ����
{
	Key_State_Present = KEY_STATE_Down_Long_First;	
}    

static void	KeyDownPressLongRepeatAction(void) //�������³���,��+��-
{
	Key_State_Present = KEY_STATE_Down_Long_Repeat;
}

//------------------------end of define------------------

//==================OK������������-1============================

static void	KeyOKPressShortAction(void);
static void	KeyOKPressLongFirstAction(void);    //�������������Ӧ����
static void	KeyOKPressLongRepeatAction(void);  //�������³���,��+��-

const struct Key_Multifun_Element Key_OK_Element = 
{
  KEY_STATE_PressValue,       	//PressValue
  KEY_STATE_ReleaseValue,     	//ReleaseValue
  10,                         	//TimOf_Low_High ���µ���������Ӧʱ��
  20,                         	//TimOfSe ���γ�����Ӧʱ��
  5,                          	//TimOfLg ��������������״̬ʱ��
  KeyOKPressShortAction,      	//void (*)(void)�̰����ɵ͵��ߵ���
  KeyOKPressLongFirstAction,    //�������������Ӧ����
  KeyOKPressLongRepeatAction,  //�������³���,��+��-
};

struct Multifun_Key Key_OK =
{
  &KeyToFuncVal,		        //������ֵ
  &Key_OK_Tik,	//����ʱ��
  1,			      //����״̬
};

unsigned char Key_OK_Tik = 0;
void Key_OK_TikFunc(void)
{
    if(Key_OK_Tik < 250)
    {
       Key_OK_Tik++;
    }
}


static void	KeyOKPressShortAction(void)
{
	Key_State_Present = KEY_STATE_OK_Short;	
}

static void	KeyOKPressLongFirstAction(void) //�������������Ӧ����
{
	Key_State_Present = KEY_STATE_OK_Long_First;	
}    

static void	KeyOKPressLongRepeatAction(void) //�������³���,��+��-
{
	Key_State_Present = KEY_STATE_OK_Long_Repeat;
}

//------------------------end of O define------------------


//==================UP������������-1============================

static void	KeyMutePressShortAction(void);
static void	KeyMutePressLongFirstAction(void);    //�������������Ӧ����
static void	KeyMutePressLongRepeatAction(void);  //�������³���,��+��-

const struct Key_Multifun_Element Key_Mute_Element = 
{
  KEY_STATE_PressValue,       	//PressValue
  KEY_STATE_ReleaseValue,     	//ReleaseValue
  10,                         	//TimOf_Low_High ���µ���������Ӧʱ��
  20,                         	//TimOfSe ���γ�����Ӧʱ��
  5,                          	//TimOfLg ��������������״̬ʱ��
  KeyMutePressShortAction,      	//void (*)(void)�̰����ɵ͵��ߵ���
  KeyMutePressLongFirstAction,    //�������������Ӧ����
  KeyMutePressLongRepeatAction,  //�������³���,��+��-
};

struct Multifun_Key Key_Mute =
{
  &KeyToFuncVal,		        //������ֵ
  &Key_Mute_Tik,	//����ʱ��
  1,			      //����״̬
};

unsigned char Key_Mute_Tik = 0;
void Key_Mute_TikFunc(void)
{
    if(Key_Mute_Tik < 250)
    {
       Key_Mute_Tik++;
    }
}


static void	KeyMutePressShortAction(void)
{
	Key_State_Present = KEY_STATE_Mute_Short;	
	
	if(AlarmInfoIndex)//�б���
	{
		if(AlarmSoundPauseStatus == 0)//��������
		{
			AlarmSoundPauseStatus = 1;//������ͣ
			Sound_Alarm_Pause_Enable();
		}
		else//������ͣ
		{
			AlarmSoundPauseStatus = 0;//������ͣȡ��
			Sound_Alarm_Pause_Disable();								
		}
	}
}

static void	KeyMutePressLongFirstAction(void) //�������������Ӧ����
{
	Key_State_Present = KEY_STATE_Mute_Long_First;	
}    

static void	KeyMutePressLongRepeatAction(void) //�������³���,��+��-
{
	Key_State_Present = KEY_STATE_Mute_Long_Repeat;
}

//------------------------end of O define------------------
