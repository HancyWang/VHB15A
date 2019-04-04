#include "all.h"
//******************************************************************************
//定义长短按键判断函数
//******************************************************************************
uint8_t  step_step_times=0;//加快速度
void Key_Multifun(const struct Key_Multifun_Element Element,struct Multifun_Key *Variate)
{
	switch(Variate->ProcessStep)
	{
		//检测按键是否按下，如果按下进入第2步，如果没有，继续1步，等待按键第一次按下
		case 1:
			if(*(Variate->KeyValueNow) == Element.PressValue)//按下按键
			{
				Variate->ProcessStep = 2;//已按下键
				*(Variate->KeyTik) = 0;//按键时标
			}
			else
			{
				Variate->ProcessStep = 1;//未按下键
				*(Variate->KeyTik) = 0;
			}
			break;   
		case 2: 
			if(*(Variate->KeyValueNow) == Element.ReleaseValue)//弹起按键
			{     
				Variate->ProcessStep = 1;
				*(Variate->KeyTik) = 0;
				(*(Element.Key_Fun_Short_Low_High))();  //按下并弹起短按函数           
			}
			else if(*(Variate->KeyValueNow) == Element.PressValue)//持续按下
			{            
				if(*(Variate->KeyTik)>=Element.TimOfSe)
				{
					Variate->ProcessStep = 3;
					*(Variate->KeyTik) = 0;
					(*(Element.Key_Fun_Long_High_Low))();//达到设定时长的首次响应函数
				}	    
				else
				{
					Variate->ProcessStep = 2;//等待
				}
			}        
			else
			{
				Variate->ProcessStep = 1;//出错
			}		
			break;  
		case 3:       
			if(*(Variate->KeyValueNow) == Element.ReleaseValue) //如果按键松开，回到1步
			{
				Variate->ProcessStep = 1;
				*(Variate->KeyTik) = 0;
				step_step_times = 0;
			}        
			else if(*(Variate->KeyValueNow) == Element.PressValue)//一直是按下，继续第3步，执行长按状态函数
			{            
				Variate->ProcessStep = 3;

				//如果按下时间标志大于连续按时间            
				if(*(Variate->KeyTik) >= (Element.TimOfLg-step_step_times))
				{
					//step_step_times = Element.TimOfLg - 1;
					if(step_step_times<(Element.TimOfLg - 1)){step_step_times++;}
					*(Variate->KeyTik) = 0;
	
					//执行长按时间函数
					(*(Element.Key_Fun_Long_Always_Low))();
				}
			}
			//所有异常情况回到0步
			else
			{
				Variate->ProcessStep = 1; 
			}		
			break;     
		default:
			break;
	}	
}

