#ifndef __KEY_DATAS__
#define __KEY_DATAS__

//定义按键IO




//定义按键按下和释放的值
typedef enum
{
  KEY_STATE_PressValue = 1,   //按下
  KEY_STATE_ReleaseValue,     //释放 
}KEY_STATUS;

extern uint8_t Key_UP_Tik;//按键计时
extern void Key_UP_TikFunc(void);//按键计时函数
extern const struct Key_Multifun_Element Key_UP_Element;//事件
extern struct Multifun_Key Key_UP;//按键

extern uint8_t Key_Down_Tik;//按键计时
extern void Key_Down_TikFunc(void);//按键计时函数
extern const struct Key_Multifun_Element Key_Down_Element;//事件 
extern struct Multifun_Key Key_Down;//按键

extern uint8_t Key_OK_Tik;//按键计时
extern void Key_OK_TikFunc(void);//按键计时函数
extern const struct Key_Multifun_Element Key_OK_Element;//事件
extern struct Multifun_Key Key_OK;//按键

extern uint8_t Key_Mute_Tik;//按键计时
extern void Key_Mute_TikFunc(void);//按键计时函数
extern const struct Key_Multifun_Element Key_Mute_Element;//事件
extern struct Multifun_Key Key_Mute;//按键

typedef enum
{
  KEY_STATE_NONE=0,    							  //关机模式   未按下任何按键
	KEY_STATE_UP_Short=1,    				    //UP键短按放开
	KEY_STATE_UP_Long_First,    				//UP键长按时间NmS
	KEY_STATE_UP_Long_Repeat,    				//UP键长按连按
	KEY_STATE_Down_Short,
	KEY_STATE_Down_Long_First,
	KEY_STATE_Down_Long_Repeat,
	KEY_STATE_OK_Short,
	KEY_STATE_OK_Long_First,
	KEY_STATE_OK_Long_Repeat,
	KEY_STATE_Mute_Short,
	KEY_STATE_Mute_Long_First,
	KEY_STATE_Mute_Long_Repeat,	
}KEY_STATE_DEF;

extern KEY_STATE_DEF Key_State_Present;




#endif