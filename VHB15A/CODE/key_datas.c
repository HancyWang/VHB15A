#include "all.h"

///注意：时标为100mS


KEY_STATE_DEF Key_State_Present;

//==================UP按键属性设置-1============================

static void	KeyUpPressShortAction(void);
static void	KeyUpPressLongFirstAction(void);    //按键长键后的响应函数
static void	KeyUpPressLongRepeatAction(void);  //连续按下持续,连+连-

const struct Key_Multifun_Element Key_UP_Element = 
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

struct Multifun_Key Key_UP =
{
  &KeyToFuncVal,		        //按键码值
  &Key_UP_Tik,	//按键时标
  1,			      //处理状态
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

static void	KeyUpPressLongFirstAction(void) //按键长键后的响应函数
{
	Key_State_Present = KEY_STATE_UP_Long_First;	
}    

static void	KeyUpPressLongRepeatAction(void) //连续按下持续,连+连-
{
	Key_State_Present = KEY_STATE_UP_Long_Repeat;
}

//------------------------end of O define------------------

//==================DOWN按键属性设置-1============================

static void	KeyDownPressShortAction(void);
static void	KeyDownPressLongFirstAction(void);    //按键长键后的响应函数
static void	KeyDownPressLongRepeatAction(void);  //连续按下持续,连+连-

const struct Key_Multifun_Element Key_Down_Element = 
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

struct Multifun_Key Key_Down =
{
  &KeyToFuncVal,		        //按键码值
  &Key_Down_Tik,	//按键时标
  1,			      //处理状态
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

static void	KeyDownPressLongFirstAction(void) //按键长键后的响应函数
{
	Key_State_Present = KEY_STATE_Down_Long_First;	
}    

static void	KeyDownPressLongRepeatAction(void) //连续按下持续,连+连-
{
	Key_State_Present = KEY_STATE_Down_Long_Repeat;
}

//------------------------end of define------------------

//==================OK按键属性设置-1============================

static void	KeyOKPressShortAction(void);
static void	KeyOKPressLongFirstAction(void);    //按键长键后的响应函数
static void	KeyOKPressLongRepeatAction(void);  //连续按下持续,连+连-

const struct Key_Multifun_Element Key_OK_Element = 
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

struct Multifun_Key Key_OK =
{
  &KeyToFuncVal,		        //按键码值
  &Key_OK_Tik,	//按键时标
  1,			      //处理状态
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

static void	KeyOKPressLongFirstAction(void) //按键长键后的响应函数
{
	Key_State_Present = KEY_STATE_OK_Long_First;	
}    

static void	KeyOKPressLongRepeatAction(void) //连续按下持续,连+连-
{
	Key_State_Present = KEY_STATE_OK_Long_Repeat;
}

//------------------------end of O define------------------


//==================UP按键属性设置-1============================

static void	KeyMutePressShortAction(void);
static void	KeyMutePressLongFirstAction(void);    //按键长键后的响应函数
static void	KeyMutePressLongRepeatAction(void);  //连续按下持续,连+连-

const struct Key_Multifun_Element Key_Mute_Element = 
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

struct Multifun_Key Key_Mute =
{
  &KeyToFuncVal,		        //按键码值
  &Key_Mute_Tik,	//按键时标
  1,			      //处理状态
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
	
	if(AlarmInfoIndex)//有报警
	{
		if(AlarmSoundPauseStatus == 0)//正常报警
		{
			AlarmSoundPauseStatus = 1;//声音暂停
			Sound_Alarm_Pause_Enable();
		}
		else//声音暂停
		{
			AlarmSoundPauseStatus = 0;//声音暂停取消
			Sound_Alarm_Pause_Disable();								
		}
	}
}

static void	KeyMutePressLongFirstAction(void) //按键长键后的响应函数
{
	Key_State_Present = KEY_STATE_Mute_Long_First;	
}    

static void	KeyMutePressLongRepeatAction(void) //连续按下持续,连+连-
{
	Key_State_Present = KEY_STATE_Mute_Long_Repeat;
}

//------------------------end of O define------------------
