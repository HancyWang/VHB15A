#ifndef __KEY_MULTIFUN__
#define __KEY_MULTIFUN__

#include "key_datas.h"

//extern uint8_t step_step_times;

//******************************************************************************

struct Key_Multifun_Element //多功能按键
{
    KEY_STATUS PressValue;                    //按键按下值
    KEY_STATUS ReleaseValue;                  //非按下值
    uint8_t TimOf_Low_High;             //按下弹起的最大响应时间
    uint8_t TimOfSe;                    //初次长按状态条件时间
    uint8_t TimOfLg;                    //后续长连续按下状态时间
    void (*Key_Fun_Short_Low_High)(void);     //短按键由低到高弹起
    void (*Key_Fun_Long_High_Low)(void);      //长按键由高到低持续TimOfSe
    void (*Key_Fun_Long_Always_Low)(void);    //连续按下持续TimOfSeTimOfLg,连+连-
};

struct Multifun_Key
{
   KEY_STATUS *KeyValueNow;                   //按键码值
   uint8_t *KeyTik;                    //按键时标
   uint8_t ProcessStep;                //处理阶段（用于状态机更换状态）
};

void Key_Multifun(const struct Key_Multifun_Element Element,struct Multifun_Key *Variate);

#endif