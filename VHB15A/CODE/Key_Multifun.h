#ifndef __KEY_MULTIFUN__
#define __KEY_MULTIFUN__

#include "key_datas.h"

//extern uint8_t step_step_times;

//******************************************************************************

struct Key_Multifun_Element //�๦�ܰ���
{
    KEY_STATUS PressValue;                    //��������ֵ
    KEY_STATUS ReleaseValue;                  //�ǰ���ֵ
    uint8_t TimOf_Low_High;             //���µ���������Ӧʱ��
    uint8_t TimOfSe;                    //���γ���״̬����ʱ��
    uint8_t TimOfLg;                    //��������������״̬ʱ��
    void (*Key_Fun_Short_Low_High)(void);     //�̰����ɵ͵��ߵ���
    void (*Key_Fun_Long_High_Low)(void);      //�������ɸߵ��ͳ���TimOfSe
    void (*Key_Fun_Long_Always_Low)(void);    //�������³���TimOfSeTimOfLg,��+��-
};

struct Multifun_Key
{
   KEY_STATUS *KeyValueNow;                   //������ֵ
   uint8_t *KeyTik;                    //����ʱ��
   uint8_t ProcessStep;                //����׶Σ�����״̬������״̬��
};

void Key_Multifun(const struct Key_Multifun_Element Element,struct Multifun_Key *Variate);

#endif