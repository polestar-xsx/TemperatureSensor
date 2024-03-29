#ifndef __SCH_H__
#define __SCH_H__

#ifndef NULL
  #define NULL (void *)0
#endif



#define MAX_TASK_NUM    8
#define SCH_TICK_PERIOD  2 /*Tick period time of scheduler,ms*/
#define SYS_CLK          (24000000)/*System clock*/

typedef void TaskHandle(void);

typedef struct
{
    TaskHandle * pTaskHandler;
    unsigned int u16Period;
    unsigned int u16Delay; 
    unsigned char boRunMe;    
}tTask;

unsigned char boAddTask(TaskHandle *,const unsigned int,const unsigned int);
unsigned char boDeleteTask(const unsigned char u8TaskIndex);
void vStartSch(void);
void vDispatchTask(void);
void vGoToSleep(void);                 
void vUpdateSch(void)  __interrupt (1)  __using (1);
#endif /* __SCH_H__ */
