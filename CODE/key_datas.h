#ifndef __KEY_DATAS__
#define __KEY_DATAS__

//���尴��IO




//���尴�����º��ͷŵ�ֵ
typedef enum
{
  KEY_STATE_PressValue = 1,   //����
  KEY_STATE_ReleaseValue,     //�ͷ� 
}KEY_STATUS;

extern uint8_t Key_UP_Tik;//������ʱ
extern void Key_UP_TikFunc(void);//������ʱ����
extern const struct Key_Multifun_Element Key_UP_Element;//�¼�
extern struct Multifun_Key Key_UP;//����

extern uint8_t Key_Down_Tik;//������ʱ
extern void Key_Down_TikFunc(void);//������ʱ����
extern const struct Key_Multifun_Element Key_Down_Element;//�¼� 
extern struct Multifun_Key Key_Down;//����

extern uint8_t Key_OK_Tik;//������ʱ
extern void Key_OK_TikFunc(void);//������ʱ����
extern const struct Key_Multifun_Element Key_OK_Element;//�¼�
extern struct Multifun_Key Key_OK;//����

extern uint8_t Key_Mute_Tik;//������ʱ
extern void Key_Mute_TikFunc(void);//������ʱ����
extern const struct Key_Multifun_Element Key_Mute_Element;//�¼�
extern struct Multifun_Key Key_Mute;//����

typedef enum
{
  KEY_STATE_NONE=0,    							  //�ػ�ģʽ   δ�����κΰ���
	KEY_STATE_UP_Short=1,    				    //UP���̰��ſ�
	KEY_STATE_UP_Long_First,    				//UP������ʱ��NmS
	KEY_STATE_UP_Long_Repeat,    				//UP����������
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