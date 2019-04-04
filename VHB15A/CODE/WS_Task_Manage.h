/// @file WS_Task_Manage.h
/// @brief define task management
/// @author Hancy
/// @date 2019.03.26
/// @version 2.0.3a
/// @note 

#ifndef __TASK__MANAGE__
#define __TASK__MANAGE__
#include "data_type.h"
//******************************************************************************
//Maximum number of tasks 确定最大任务数量"TASK_32","TASK_16","TASK_8"
//******************************************************************************
/// @brief This is used to define the maximum number of tasks
#define TASK_32
//#define TASK_16
//#define TASK_8


#ifdef  TASK_32
#define TASK_32

extern uint32_t WS_TaskIfg;
extern const uint32_t WS_TaskMap[32];

#endif

#ifdef  TASK_16
#define TASK_16

extern u16s_t WS_TaskIfg;
extern const u16s_t WS_TaskMap[16];

#endif


#ifdef  TASK_8
#define TASK_8

extern uint8_t WS_TaskIfg;
extern  uint8_t const WS_TaskMap[8];

#endif

extern uint8_t WS_TaskState;

//任务在就绪表中就绪操作
void WS_SetTaskIfg(uint8_t Setprio);
//任务在就绪表中删除操作
void WS_DelTaskIfg(uint8_t DelPrio);
//任务判断是否就绪操作
uint8_t WS_TaskJudge(uint8_t AllPrio);
//自动Task时标
//void WS_TaskTik(void);


#endif