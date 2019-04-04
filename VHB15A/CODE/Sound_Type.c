#include "Sound_Type.h"
#include "all.h"

static uint8_t Sound_En_Cnt = 0;
static uint16_t SoundPauseCnt = 0;

//2019.03.30
sbit P43 = (uint8_t)0xC0^(uint8_t)3;

void Sound_Short(void)//哔短响一声
{	
	if(AlarmInfoIndex!=(uint8_t)0)//当前是报警状态,无声音
	{
		//do nothing
	}
	else
	{
		Sound_En_Cnt = 10;		
	}
}

void Sound_Alarm_Pause_Enable(void)//声音暂停开启，时间重置
{	
	SoundPauseCnt = (90 * 50);//90S计时
}

void Sound_Alarm_Pause_Disable(void)//声音暂停取消
{	
	SoundPauseCnt = 0;//90S计时
}

void Sound_Type_Drive(void)//放入定时中断
{
	if(AlarmInfoIndex!=(uint8_t)0)//当前是报警状态
	{
		if(SoundPauseCnt == 0)//声音暂停取消
		{
			Buzzer_Port=1;
			AlarmSoundPauseStatus = 0;//声音暂停取消
		}
		else//声音暂停
		{
			SoundPauseCnt--;	
			Buzzer_Port=0;			
		}
	}
	else//非报警状态
	{
		if(Sound_En_Cnt!=(uint8_t)0)
		{
			Buzzer_Port=1;
			Sound_En_Cnt--;
		}
		else
		{
			Buzzer_Port=0;
		}
	}
}


