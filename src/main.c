#include<lint.h>
#include"stc8h.h"
#include"sch.h"
#include"18b20.h"

#define LED_R P20
#define LED_G P21
#define LED_B P22
#define DE1   P00
#define DE2   P01
#define DE3   P02
#define DE4   P03
#define DIG   P1


const unsigned char u8DigtalMap[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char u8DisBuffer[5]={0,0,0,0,0x80};
extern unsigned char au8RecvBuffer[9];

void vDisplay(void)
{
    static unsigned char u8DigIndex = 0;
    switch(u8DigIndex)
    {
        case 0:DE1 = 0;DE4 = 1;DIG = ~u8DigtalMap[u8DisBuffer[0]];break;
        case 1:DE4 = 0;DE3 = 1;DIG = ~u8DigtalMap[u8DisBuffer[1]];break;
        case 2:DE3 = 0;DE2 = 1;DIG = ~(u8DigtalMap[u8DisBuffer[2]]|u8DisBuffer[4]);break;
        case 3:DE2 = 0;DE1 = 1;DIG = ~u8DigtalMap[u8DisBuffer[3]];break;
        default:u8DigIndex = 0;break;
    }
    if(++u8DigIndex >= 4) u8DigIndex = 0;
}

void vUpdateCnt(void)
{
    static unsigned int u16Count = 0;
    if(u16Count++ >= 10000) u16Count = 0;
    u8DisBuffer[0] = u16Count % 10;
    u8DisBuffer[1] = (u16Count / 10) % 10;
    u8DisBuffer[2] = (u16Count / 100) % 10;
    u8DisBuffer[3] = (u16Count / 1000) % 10;
    u8DisBuffer[4] = u8DisBuffer[4]^0x80;
}

void vUpdateTemp(void) 
{
    unsigned char temp = au8RecvBuffer[1]<<4 | au8RecvBuffer[0]>>4;
    u8DisBuffer[0] = temp % 10;
    u8DisBuffer[1] = (temp / 10) % 10;
    u8DisBuffer[2] = (temp / 100) % 10;
    u8DisBuffer[3] = (temp / 1000) % 10;
}

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
    boAddTask(vDisplay,1,3);
    //boAddTask(vUpdateCnt,1000,0);
    boAddTask(vStartCmdSeq,3000,8);
    boAddTask(vUpdateTemp,3000,15);
    boAddTask(v18b20ServiceTask,1,6);
    vT0Init(SCH_TICK_PERIOD);
    vStartSch();
    while (1)
    {
        //LED_R = 1;
        vDispatchTask();
        //LED_R = 0;
    }
}

