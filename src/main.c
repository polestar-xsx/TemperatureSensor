#include<lint.h>
#include"stc8h.h"
#include"sch.h"
#include"18b20.h"
#include"iic.h"
#include"clk_sd3078.h"
#include"key.h"
#include"display.h"



void vIOInit(void)
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    DE1 = 0x00;
    DE2 = 0x00;
    DE3 = 0x00;
    DE4 = 0x00;
    DIG = 0xff;
}

void vT0Init(unsigned int u16TaskPeriod)
{
    unsigned int temp;
    TMOD = 0x00;
    temp = 65535 - ((SYS_CLK*u16TaskPeriod)/12/1000);            //65536-Period*Sysclk/12/1000
    TL0 = (unsigned char)(temp & 0xff);                                 
    TH0 = (unsigned char)((temp & 0xff00)>>8);
}

void main()
{
    vIOInit();
    vSetReadTempSeq();
    KeyInit();
    vDisplayInit();
    boAddTask(vDisplayFresh,DIS_FRESH_TASK_PERIOD,3);
    //boAddTask(vGetKey,20,5);
    boAddTask(vReadTemp,3000,8);
    boAddTask(vUpdateClk,1000,15);
    boAddTask(v18b20ServiceTask,1,6);
    boAddTask(vKeyTask,KeyTaskPeriod,20);
    boAddTask(vDisplayMainTask,DIS_MAIN_TASK_PERIOD,30);
    
    vT0Init(SCH_TICK_PERIOD);
    vIIC_Init();
    //vInitClk();
    vStartSch();
    while (1)
    {
        vDispatchTask();
    }
}

