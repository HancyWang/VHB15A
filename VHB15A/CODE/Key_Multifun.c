#include "all.h"
//******************************************************************************
//���峤�̰����жϺ���
//******************************************************************************
uint8_t  step_step_times=0;//�ӿ��ٶ�
void Key_Multifun(const struct Key_Multifun_Element Element,struct Multifun_Key *Variate)
{
	switch(Variate->ProcessStep)
	{
		//��ⰴ���Ƿ��£�������½����2�������û�У�����1�����ȴ�������һ�ΰ���
		case 1:
			if(*(Variate->KeyValueNow) == Element.PressValue)//���°���
			{
				Variate->ProcessStep = 2;//�Ѱ��¼�
				*(Variate->KeyTik) = 0;//����ʱ��
			}
			else
			{
				Variate->ProcessStep = 1;//δ���¼�
				*(Variate->KeyTik) = 0;
			}
			break;   
		case 2: 
			if(*(Variate->KeyValueNow) == Element.ReleaseValue)//���𰴼�
			{     
				Variate->ProcessStep = 1;
				*(Variate->KeyTik) = 0;
				(*(Element.Key_Fun_Short_Low_High))();  //���²�����̰�����           
			}
			else if(*(Variate->KeyValueNow) == Element.PressValue)//��������
			{            
				if(*(Variate->KeyTik)>=Element.TimOfSe)
				{
					Variate->ProcessStep = 3;
					*(Variate->KeyTik) = 0;
					(*(Element.Key_Fun_Long_High_Low))();//�ﵽ�趨ʱ�����״���Ӧ����
				}	    
				else
				{
					Variate->ProcessStep = 2;//�ȴ�
				}
			}        
			else
			{
				Variate->ProcessStep = 1;//����
			}		
			break;  
		case 3:       
			if(*(Variate->KeyValueNow) == Element.ReleaseValue) //��������ɿ����ص�1��
			{
				Variate->ProcessStep = 1;
				*(Variate->KeyTik) = 0;
				step_step_times = 0;
			}        
			else if(*(Variate->KeyValueNow) == Element.PressValue)//һֱ�ǰ��£�������3����ִ�г���״̬����
			{            
				Variate->ProcessStep = 3;

				//�������ʱ���־����������ʱ��            
				if(*(Variate->KeyTik) >= (Element.TimOfLg-step_step_times))
				{
					//step_step_times = Element.TimOfLg - 1;
					if(step_step_times<(Element.TimOfLg - 1)){step_step_times++;}
					*(Variate->KeyTik) = 0;
	
					//ִ�г���ʱ�亯��
					(*(Element.Key_Fun_Long_Always_Low))();
				}
			}
			//�����쳣����ص�0��
			else
			{
				Variate->ProcessStep = 1; 
			}		
			break;     
		default:
			break;
	}	
}

