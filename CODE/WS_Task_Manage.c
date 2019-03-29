
#include "WS_Task_Manage.h"

#ifdef  TASK_32
#define TASK_32
//******************************************************************************
//任务标志就绪表,最多支持32（0~31）个任务
//******************************************************************************
unsigned long int WS_TaskIfg = 0;                                         //就绪标志位
unsigned char WS_TaskState = 0;                                           //当前任务
const unsigned long int WS_TaskMap[32] =                                  //标志位操作表
{
    0x00000001 , 0x00000002 , 0x00000004 , 0x00000008 , 0x00000010 , 0x00000020 , 0x00000040 , 0x00000080,
    0x00000100 , 0x00000200 , 0x00000400 , 0x00000800 , 0x00001000 , 0x00002000 , 0x00004000 , 0x00008000,
    0x00010000 , 0x00020000 , 0x00040000 , 0x00080000 , 0x00100000 , 0x00200000 , 0x00400000 , 0x00800000,
    0x01000000 , 0x02000000 , 0x04000000 , 0x08000000 , 0x10000000 , 0x20000000 , 0x40000000 , 0x80000000,
};
#endif

#ifdef  TASK_16
#define TASK_16
//******************************************************************************
//任务标志就绪表,最多支持16（0~15）个任务
//******************************************************************************
unsigned short int WS_TaskIfg = 0;                                        //就绪标志位
unsigned char WS_TaskState = 0;                                           //当前任务
const unsigned short int WS_TaskMap[16] =                                 //标志位操作表
{
    0x0001 , 0x0002 , 0x0004 , 0x0008 , 0x0010 , 0x0020 , 0x0040 , 0x0080,
    0x0100 , 0x0200 , 0x0400 , 0x0800 , 0x1000 , 0x2000 , 0x4000 , 0x8000,
};
#endif

#ifdef  TASK_8
#define TASK_8
//******************************************************************************
//任务标志就绪表,最多支持8（0~7）个任务
//******************************************************************************
unsigned char WS_TaskIfg = 0;                                             //就绪标志位
unsigned char WS_TaskState = 0;                                           //当前任务
unsigned char const WS_TaskMap[8] =                                       //标志位操作表
{
    0x01 , 0x02 , 0x04 , 0x08 , 0x10 , 0x20 , 0x40 , 0x80,
};
#endif

//任务在就绪表中就绪操作
void WS_SetTaskIfg(unsigned char Setprio)
{
    WS_TaskIfg |= WS_TaskMap[Setprio];
}

//任务在就绪表中删除操作
void WS_DelTaskIfg(unsigned char DelPrio)
{
    WS_TaskIfg &= ~WS_TaskMap[DelPrio];
}

//任务判断是否就绪操作
unsigned char WS_TaskJudge(unsigned char AllPrio)
{
    if((WS_TaskMap[AllPrio])&WS_TaskIfg)
    {
     return 1;
    }
    return 0;
}



