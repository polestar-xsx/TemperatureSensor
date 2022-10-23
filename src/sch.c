#include<lint.h>
#include "sch.h"
#include "stc8h.h"

tTask __xdata aTask[MAX_TASK_NUM]={{0, 0, 0, 0}};

unsigned char boAddTask(TaskHandle *pTaskHandle,const unsigned int u16Period,const unsigned int u16Delay)
{
    unsigned char u8Index = 0;
    while((aTask[u8Index].pTaskHandler !=0)&&(u8Index<MAX_TASK_NUM))
    {
        u8Index++;
    }
    if(u8Index == MAX_TASK_NUM)
    {
        return 0xff;
    }
    aTask[u8Index].pTaskHandler = pTaskHandle;
    aTask[u8Index].u16Period = u16Period;
    aTask[u8Index].u16Delay = u16Delay;
    aTask[u8Index].boRunMe = 0;
    return u8Index;
}

unsigned char boDeleteTask(const unsigned char u8TaskIndex)
{
    if(aTask[u8TaskIndex].pTaskHandler != 0x0000)
    {
        aTask[u8TaskIndex].pTaskHandler = 0x0000;
        aTask[u8TaskIndex].boRunMe = 0;
        aTask[u8TaskIndex].u16Delay = 0;
        aTask[u8TaskIndex].u16Period = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}
void vStartSch(void)
{
    TR0 = 1;
    ET0 = 1;
    EA = 1;
}
void vDispatchTask(void)
{
    unsigned char u8Index;
    for (u8Index = 0;u8Index < MAX_TASK_NUM;u8Index++)
    {
        if(aTask[u8Index].boRunMe > 0)
        {
            (*aTask[u8Index].pTaskHandler)();
            aTask[u8Index].boRunMe -= 1;
        }
        if(aTask[u8Index].u16Period == 0)
        {
            boDeleteTask(u8Index);
        }
    }
    vGoToSleep();
}

void vGoToSleep(void)
{
    
}     

void vUpdateSch(void)  __interrupt (1)  __using (1)
{
    unsigned char u8Index;
    TF0 = 0;
    for(u8Index = 0; u8Index < MAX_TASK_NUM;u8Index++)
    {
        if(aTask[u8Index].pTaskHandler != 0)
        {
            if (aTask[u8Index].u16Delay == 0)
            {
                aTask[u8Index].boRunMe += 1;
                if(aTask[u8Index].u16Period != 0)
                {
                    aTask[u8Index].u16Delay = aTask[u8Index].u16Period-1;
                }
            }
            else
            {
                if(aTask[u8Index].u16Delay <= SCH_TICK_PERIOD)
                {
                    aTask[u8Index].u16Delay = 0;
                }
                else
                {
                    aTask[u8Index].u16Delay -= SCH_TICK_PERIOD;
                }
            }
        }
    }
}