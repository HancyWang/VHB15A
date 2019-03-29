/// @file WS_Task_Manage.h
/// @brief define task management
/// @author Hancy
/// @date 2019.03.26
/// @version 2.0.3a
/// @note 

#ifndef __TASK__MANAGE__
#define __TASK__MANAGE__

//******************************************************************************
//Maximum number of tasks ȷ�������������"TASK_32","TASK_16","TASK_8"
//******************************************************************************
/// @brief This is used to define the maximum number of tasks
#define TASK_32
//#define TASK_16
//#define TASK_8


#ifdef  TASK_32
#define TASK_32

extern unsigned long int WS_TaskIfg;
extern const unsigned long int WS_TaskMap[];

#endif

#ifdef  TASK_16
#define TASK_16

extern unsigned short int WS_TaskIfg;
extern const unsigned short int WS_TaskMap[];

#endif


#ifdef  TASK_8
#define TASK_8

extern uint8_t WS_TaskIfg;
extern  unsigned char const WS_TaskMap[];

#endif

extern unsigned char WS_TaskState;

//�����ھ������о�������
void WS_SetTaskIfg(unsigned char Setprio);
//�����ھ�������ɾ������
void WS_DelTaskIfg(unsigned char DelPrio);
//�����ж��Ƿ��������
unsigned char WS_TaskJudge(unsigned char AllPrio);
//�Զ�Taskʱ��
//void WS_TaskTik(void);


#endif