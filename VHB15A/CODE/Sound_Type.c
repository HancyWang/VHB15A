#include "Sound_Type.h"
#include "all.h"

static uint8_t Sound_En_Cnt = 0;
static uint16_t SoundPauseCnt = 0;

//2019.03.30
sbit P43 = (uint8_t)0xC0^(uint8_t)3;

void Sound_Short(void)//�ٶ���һ��
{	
	if(AlarmInfoIndex!=(uint8_t)0)//��ǰ�Ǳ���״̬,������
	{
		//do nothing
	}
	else
	{
		Sound_En_Cnt = 10;		
	}
}

void Sound_Alarm_Pause_Enable(void)//������ͣ������ʱ������
{	
	SoundPauseCnt = (90 * 50);//90S��ʱ
}

void Sound_Alarm_Pause_Disable(void)//������ͣȡ��
{	
	SoundPauseCnt = 0;//90S��ʱ
}

void Sound_Type_Drive(void)//���붨ʱ�ж�
{
	if(AlarmInfoIndex!=(uint8_t)0)//��ǰ�Ǳ���״̬
	{
		if(SoundPauseCnt == 0)//������ͣȡ��
		{
			Buzzer_Port=1;
			AlarmSoundPauseStatus = 0;//������ͣȡ��
		}
		else//������ͣ
		{
			SoundPauseCnt--;	
			Buzzer_Port=0;			
		}
	}
	else//�Ǳ���״̬
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


