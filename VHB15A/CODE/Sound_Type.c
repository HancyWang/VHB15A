#include "Sound_Type.h"
#include "all.h"

static unsigned char Sound_En_Cnt = 0;
static unsigned int SoundPauseCnt = 0;

//2019.03.30
sbit P43 = 0xC0^3;

void Sound_Short(void)//�ٶ���һ��
{	
	if(AlarmInfoIndex)//��ǰ�Ǳ���״̬,������
	{
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
	if(AlarmInfoIndex)//��ǰ�Ǳ���״̬
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
		if(Sound_En_Cnt)
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