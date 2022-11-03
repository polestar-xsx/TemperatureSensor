#include "display.h"
#define SET_DIG_BLINK(i,blink)      {stDisBuffer[i].enBlkFreq = blink;}
typedef struct
{
    tenBlinkFreq enBlkFreq;
    unsigned char u8Code;
}tstDisItem;

const unsigned char u8DigtalMap[10] = {0xC0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
const unsigned char u8CharacterMap[26] = {0x88,0x80,0xc6,0xc0,0x86,0x8e,0xc2,0xff,0xf9,0xe1,0xff,0xc7,0xff,0xff,0xc0,0x8c,0xff,0xff,0x92,0xff,0xc1,0xff,0xff,0xff,0xff,0xff};
extern tstClock __xdata stClk;
extern unsigned char au8RecvBuffer[9];
tstDisItem __xdata stDisBuffer[5];

tenDisplayMode enMainDisMode = enDisMode_Sleep;

tenClkDisplayMode enClkDisMode = enClkDisMode_Time;

unsigned char boDisplayOn = 1;

tstClock stClkSetTemp;   //Temp clk date used fro setting

void vDisplayOff(void)
{
    boDisplayOn = 0;
}

void vDisplayOn(void)
{
    boDisplayOn = 1;
}

void vDisplayTime(tstClock *pstClk)
{
    stDisBuffer[3].u8Code = u8DigtalMap[pstClk->u8Min&0xf];
    stDisBuffer[3].enBlkFreq = enBlink_ON;
    stDisBuffer[2].u8Code = u8DigtalMap[pstClk->u8Min>>4];
    stDisBuffer[2].enBlkFreq = enBlink_ON;
    stDisBuffer[1].u8Code = u8DigtalMap[pstClk->u8Hour&0xf];
    stDisBuffer[1].enBlkFreq = enBlink_ON;
    stDisBuffer[0].u8Code = u8DigtalMap[(pstClk->u8Hour>>4)&0x07];
    stDisBuffer[0].enBlkFreq = enBlink_ON;
    stDisBuffer[4].u8Code = 0x7f;                 /*Clock points*/
    stDisBuffer[4].enBlkFreq = enBlink_1HZ;
}

void vDisplayYear(tstClock *pstClk)
{    
    stDisBuffer[3].u8Code = u8DigtalMap[pstClk->u8Year&0xf];
    stDisBuffer[3].enBlkFreq = enBlink_ON;
    stDisBuffer[2].u8Code = u8DigtalMap[pstClk->u8Year>>4];
    stDisBuffer[2].enBlkFreq = enBlink_ON;
    stDisBuffer[1].u8Code = u8DigtalMap[0];
    stDisBuffer[1].enBlkFreq = enBlink_ON;
    stDisBuffer[0].u8Code = u8DigtalMap[2];
    stDisBuffer[0].enBlkFreq = enBlink_ON;
    stDisBuffer[4].enBlkFreq = enBlink_OFF;
}

void vDisplayDate(tstClock *pstClk)
{
    stDisBuffer[3].u8Code = u8DigtalMap[pstClk->u8Day&0xf];
    stDisBuffer[3].enBlkFreq = enBlink_ON;
    stDisBuffer[2].u8Code = u8DigtalMap[pstClk->u8Day>>4];
    stDisBuffer[2].enBlkFreq = enBlink_ON;
    stDisBuffer[1].u8Code = u8DigtalMap[pstClk->u8Month&0xf];
    stDisBuffer[1].enBlkFreq = enBlink_ON;
    stDisBuffer[0].u8Code = u8DigtalMap[pstClk->u8Month>>4];
    stDisBuffer[0].enBlkFreq = enBlink_ON;
    stDisBuffer[4].enBlkFreq = enBlink_ON;
}

void vDisplayDig(unsigned int i16value, unsigned char boPositive)
{
    unsigned char i = 0;
    if(boPositive>0)
    {
        stDisBuffer[3].u8Code = u8DigtalMap[i16value%10];
        stDisBuffer[3].enBlkFreq = enBlink_ON;
        stDisBuffer[2].u8Code = u8DigtalMap[(i16value/10)%10];
        stDisBuffer[2].enBlkFreq = i16value < 10 ?enBlink_OFF:enBlink_ON;
        stDisBuffer[1].u8Code = u8DigtalMap[(i16value/100)%10];
        stDisBuffer[1].enBlkFreq = i16value < 100 ?enBlink_OFF:enBlink_ON;
        stDisBuffer[0].u8Code = u8DigtalMap[(i16value/1000)%10];
        stDisBuffer[0].enBlkFreq = i16value < 1000?enBlink_OFF:enBlink_ON;
        stDisBuffer[4].enBlkFreq = enBlink_OFF;
    }
    else
    {
        stDisBuffer[3].u8Code = u8DigtalMap[i16value%10];
        stDisBuffer[3].enBlkFreq = enBlink_ON;
        stDisBuffer[2].u8Code = u8DigtalMap[(i16value/10)%10];
        stDisBuffer[2].enBlkFreq = i16value < 10 ?enBlink_OFF:enBlink_ON;
        stDisBuffer[1].u8Code = u8DigtalMap[(i16value/100)%10];
        stDisBuffer[1].enBlkFreq = i16value < 100 ?enBlink_OFF:enBlink_ON;
        stDisBuffer[0].enBlkFreq = enBlink_OFF;
        for(i = 0;i<4;i++)
        {
            if(stDisBuffer[3-i].enBlkFreq == enBlink_OFF)
            {
                stDisBuffer[3-i].u8Code = 0xbf; /*Negative symple*/
                stDisBuffer[3-i].enBlkFreq = enBlink_ON;
                break;
            }
        }
    }
}

void vDisplayTemp(unsigned int i16value, unsigned char boPositive)
{
    unsigned char i = 0;
    if(boPositive>0)
    {
        stDisBuffer[3].u8Code = u8CharacterMap['C'-'A'];
        stDisBuffer[3].enBlkFreq = enBlink_ON;
        stDisBuffer[2].u8Code = u8DigtalMap[i16value%10];
        stDisBuffer[2].enBlkFreq = enBlink_ON;
        stDisBuffer[1].u8Code = u8DigtalMap[(i16value/10)%10];
        stDisBuffer[1].enBlkFreq = i16value < 10 ?enBlink_OFF:enBlink_ON;
        stDisBuffer[0].u8Code = u8DigtalMap[(i16value/100)%10];
        stDisBuffer[0].enBlkFreq = i16value < 100?enBlink_OFF:enBlink_ON;
        stDisBuffer[4].enBlkFreq = enBlink_OFF;
    }
    else
    {
        stDisBuffer[3].u8Code = u8CharacterMap['C'-'A'];
        stDisBuffer[3].enBlkFreq = enBlink_ON;
        stDisBuffer[2].u8Code = u8DigtalMap[i16value%10];
        stDisBuffer[2].enBlkFreq = enBlink_ON;
        stDisBuffer[1].u8Code = u8DigtalMap[(i16value/10)%10];
        stDisBuffer[1].enBlkFreq = i16value < 10 ?enBlink_OFF:enBlink_ON;
        stDisBuffer[0].enBlkFreq = enBlink_OFF;
        for(i = 0;i<4;i++)
        {
            if(stDisBuffer[3-i].enBlkFreq == enBlink_OFF)
            {
                stDisBuffer[3-i].u8Code = 0xbf; /*Negative symple*/
                stDisBuffer[3-i].enBlkFreq = enBlink_ON;
                break;
            }
        }
    }
}



void vDisplayInit(void)
{
    unsigned char i = 0;
    enMainDisMode = enDisMode_DisClk;
    vDisplayTime(&stClk);
    vDisplayOn();
    for(i=0;i<5;i++)
    {
        stDisBuffer[i].enBlkFreq = enBlink_OFF;
        stDisBuffer[i].u8Code = 0xff;
    }
}

void vDisplayFresh(void)
{
    static unsigned char u8DigIndex = 0;
    static unsigned int u16Blink05HzCnt = 0;
    static unsigned int u16Blink1HzCnt = 0;
    static unsigned int u16Blink2HzCnt = 0;
    if(boDisplayOn)
    {
        switch(stDisBuffer[4].enBlkFreq)  //DP blink
        {
            case enBlink_OFF:stDisBuffer[4].u8Code=0x80;break;
            case enBlink_ON:stDisBuffer[4].u8Code=0x7f;break;
            case enBlink_05HZ:
                if(u16Blink05HzCnt <= BLK_05HZ_HALF_PERIOD)
                {
                    stDisBuffer[4].u8Code = 0x7f;
                }
                else
                {
                    stDisBuffer[4].u8Code = 0x80;
                }
                break;
            case enBlink_1HZ:
                if(u16Blink1HzCnt <= BLK_1HZ_HALF_PERIOD)
                {
                    stDisBuffer[4].u8Code = 0x7f;
                }
                else
                {
                    stDisBuffer[4].u8Code = 0x80;
                }
                break;
            case enBlink_2HZ:
                if(u16Blink2HzCnt <= BLK_2HZ_HALF_PERIOD)
                {
                    stDisBuffer[4].u8Code = 0x7f;
                }
                else
                {
                    stDisBuffer[4].u8Code = 0x80;
                }
                break;
            default:break;
        }
        DE = (DE&0xf0)|(1<<u8DigIndex);
        switch(stDisBuffer[u8DigIndex].enBlkFreq)
        {
            case enBlink_OFF:DIG = 0xff;break;
            case enBlink_ON:DIG = stDisBuffer[4].u8Code == 0x7f?stDisBuffer[u8DigIndex].u8Code & stDisBuffer[4].u8Code:stDisBuffer[u8DigIndex].u8Code | stDisBuffer[4].u8Code;break;
            case enBlink_05HZ:
                if(u16Blink05HzCnt <= BLK_05HZ_HALF_PERIOD)
                {
                    DIG = stDisBuffer[4].u8Code == 0x7f?stDisBuffer[u8DigIndex].u8Code & stDisBuffer[4].u8Code:stDisBuffer[u8DigIndex].u8Code | stDisBuffer[4].u8Code;
                }
                else
                {
                    DIG = stDisBuffer[4].u8Code == 0x7f?0xff & stDisBuffer[4].u8Code:0xff;
                }
                break;
            case enBlink_1HZ:
                if(u16Blink1HzCnt <= BLK_1HZ_HALF_PERIOD)
                {
                    DIG = stDisBuffer[4].u8Code == 0x7f?stDisBuffer[u8DigIndex].u8Code & stDisBuffer[4].u8Code:stDisBuffer[u8DigIndex].u8Code | stDisBuffer[4].u8Code;
                }
                else
                {
                    DIG = stDisBuffer[4].u8Code == 0x7f?0xff & stDisBuffer[4].u8Code:0xff;
                }
                break;
            case enBlink_2HZ:
                if(u16Blink2HzCnt <= BLK_2HZ_HALF_PERIOD)
                {
                    DIG = stDisBuffer[4].u8Code == 0x7f?stDisBuffer[u8DigIndex].u8Code & stDisBuffer[4].u8Code:stDisBuffer[u8DigIndex].u8Code | stDisBuffer[4].u8Code;
                }
                else
                {
                    DIG = stDisBuffer[4].u8Code == 0x7f?0xff & stDisBuffer[4].u8Code:0xff;
                }
                break;
            default:break;
        }
        
        if(++u8DigIndex >= 4) u8DigIndex = 0;
        u16Blink05HzCnt+=DIS_FRESH_TASK_PERIOD;
        if(u16Blink05HzCnt > BLK_05HZ_FULL_PERIOD)u16Blink05HzCnt = 0;
        u16Blink1HzCnt+=DIS_FRESH_TASK_PERIOD;
        if(u16Blink1HzCnt > BLK_1HZ_FULL_PERIOD)u16Blink1HzCnt = 0;
        u16Blink2HzCnt+=DIS_FRESH_TASK_PERIOD;
        if(u16Blink2HzCnt > BLK_2HZ_FULL_PERIOD)u16Blink2HzCnt = 0;
    }
    else
    {
        DE = DE&0xf0;
    }    
}

void vSleepHandler(tstKey *Key)
{
    if(Key->enKeyValue != enKey_Null)
    {
        switch(Key->enKeyValue)
        {
            case enKey_2:enMainDisMode = enDisMode_DisTemp;break;
            case enKey_3:enMainDisMode = enDisMode_Counter;break;
            default:enMainDisMode = enDisMode_DisClk;vDisplayTime(&stClk);enClkDisMode = enClkDisMode_Time;break;
        }
    }
    else
    {

    }
}

void vClkDisHandler(tstKey *Key)
{
    switch(Key->enKeyValue)
    {
        case enKey_1:enMainDisMode = enDisMode_DisTemp;break;
        case enKey_12:enMainDisMode = enDisMode_Counter;break;
        case enKey_13:
            if(Key->enKeySts == enKeySts_LongPressed)
            {
                enMainDisMode = enDisMode_SetClk;
                stClkSetTemp = stClk;
            }
            break;
        case enKey_2:vDisplayYear(&stClk);enClkDisMode = enClkDisMode_Year;break;
        case enKey_3:vDisplayDate(&stClk);enClkDisMode = enClkDisMode_Date;break;
        case enKey_4:vDisplayTime(&stClk);enClkDisMode = enClkDisMode_Time;break;
        default:break; 
    }
    if(enClkDisMode == enClkDisMode_Time)
    {
        vDisplayTime(&stClk);
    }
    else if(enClkDisMode == enClkDisMode_Date)
    {
        vDisplayDate(&stClk);
    }
    else if(enClkDisMode == enClkDisMode_Year)
    {
        vDisplayYear(&stClk);
    }
}

void vTempDisHandler(tstKey *Key)
{
    unsigned char temp;
    unsigned char boPositive = 1;
    if(au8RecvBuffer[1]&0xf8)/*Negative*/
    {
        temp = (((~au8RecvBuffer[1])&0x07)<<4)|((~au8RecvBuffer[0] + 1)>>4);
        boPositive = 0;
    }
    else
    {
        temp = au8RecvBuffer[1]<<4 | au8RecvBuffer[0]>>4;
        boPositive = 1;
    }
    vDisplayTemp((unsigned int)temp,boPositive);
    switch(Key->enKeyValue)
    {
        case enKey_4:enMainDisMode = enDisMode_DisClk;vDisplayTime(&stClk);enClkDisMode = enClkDisMode_Time;break;
        default:break; 
    }
}

void vCounterHandler(tstKey *Key)
{
    static unsigned int u16Counter = 0;
    switch(Key->enKeyValue)
    {
        case enKey_1:enMainDisMode = enDisMode_DisClk;break;
        case enKey_2:
            if(u16Counter > 0)u16Counter--;break;
        case enKey_3:
            if(++u16Counter > 9999)u16Counter = 0;break;
        case enKey_4:
            if(Key->enKeySts == enKeySts_LongPressed)u16Counter=0;break;
        default:break; 
    }
    vDisplayDig(u16Counter,1);
}

void vSetClkHandler(tstKey *Key)
{
    unsigned char u8SetTemp = 0;
    static unsigned char u8SetIndex = 0;
    unsigned char u8MaxDayofMon = 30;
    unsigned char u8MonthTemp = 0;//Temp month use to calc max day
    unsigned char u8YearTemp = 0; //Used to calc max day
    switch(enClkDisMode)
    {
        case enClkDisMode_Time:
            if(u8SetIndex == 0)
            {
                u8SetTemp = (stClkSetTemp.u8Min>>4)*10;  //BCD to int
                u8SetTemp += stClkSetTemp.u8Min&0x0f;
            }
            else
            {
                u8SetTemp = (stClkSetTemp.u8Hour>>4&0x07)*10; //BCD to int
                u8SetTemp += stClkSetTemp.u8Hour&0x0f;
            }
            switch(Key->enKeyValue)
            {
                case enKey_1:break;
                case enKey_2:
                    if(u8SetIndex == 0)
                    {
                        if(--u8SetTemp > 60)u8SetTemp = 59;
                        stClkSetTemp.u8Min = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }
                    else
                    {
                        if(--u8SetTemp > 23)u8SetTemp = 23;
                        stClkSetTemp.u8Hour = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }                  
                    break;
                case enKey_3:
                    if(u8SetIndex == 0)
                    {
                        if(++u8SetTemp>59)u8SetTemp = 0;
                        stClkSetTemp.u8Min = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }
                    else
                    {
                        if(++u8SetTemp>23)u8SetTemp = 0;
                        stClkSetTemp.u8Hour = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }     
                    break;
                case enKey_4:
                    if(Key->enKeySts == enKeySts_ShortPressed)
                    {
                        if(u8SetIndex == 0)
                        {
                            u8SetIndex = 1;
                        }
                        else
                        {
                            stClkSetTemp.u8Hour = stClkSetTemp.u8Hour | 0x80;//24 hour format
                            vClk_SetClk(&stClkSetTemp,NULL);
                            enMainDisMode = enDisMode_DisClk;
                            u8SetIndex = 0;
                        }
                    }                    
                    break;
                default:break;
            }
            vDisplayTime(&stClkSetTemp);
            if(u8SetIndex == 0)
            {
                SET_DIG_BLINK(3,enBlink_2HZ);
                SET_DIG_BLINK(2,enBlink_2HZ);
            }
            else
            {
                SET_DIG_BLINK(1,enBlink_2HZ);
                SET_DIG_BLINK(0,enBlink_2HZ);
            }     
            SET_DIG_BLINK(4,enBlink_ON); 
            break;
        case enClkDisMode_Year:
            u8SetTemp = (stClkSetTemp.u8Year>>4)*10;  //BCD to int
            u8SetTemp += stClkSetTemp.u8Year&0x0f;
            switch(Key->enKeyValue)
            {
                case enKey_1:break;
                case enKey_2:
                    if(--u8SetTemp > 99)u8SetTemp = 99;
                    stClkSetTemp.u8Year = (u8SetTemp/10)<<4 | (u8SetTemp%10);                
                    break;
                case enKey_3:
                    if(++u8SetTemp>99)u8SetTemp = 0;
                    stClkSetTemp.u8Year = (u8SetTemp/10)<<4 | (u8SetTemp%10);   
                    break;
                case enKey_4:
                    if(Key->enKeySts == enKeySts_ShortPressed)
                    {
                        vClk_SetClk(&stClkSetTemp,NULL);
                        enMainDisMode = enDisMode_DisClk;
                    }                    
                    break;
                default:break;
            }
            vDisplayYear(&stClkSetTemp);
            SET_DIG_BLINK(3,enBlink_2HZ);
            SET_DIG_BLINK(2,enBlink_2HZ);   
            SET_DIG_BLINK(4,enBlink_ON); 
            break;
        case enClkDisMode_Date:
            if(u8SetIndex == 0)
            {
                u8SetTemp = (stClkSetTemp.u8Day>>4)*10;  //BCD to int
                u8SetTemp += stClkSetTemp.u8Day&0x0f;
                u8MonthTemp = (stClkSetTemp.u8Month>>4)*10; //BCD to int
                u8MonthTemp += stClkSetTemp.u8Month&0x0f;
                if(u8MonthTemp == 1 || u8MonthTemp == 3 || u8MonthTemp == 5 || u8MonthTemp == 7 || u8MonthTemp == 8\
                    || u8MonthTemp == 10 || u8MonthTemp == 12)
                {
                    u8MaxDayofMon = 31;
                }
                else if(u8MonthTemp == 4 || u8MonthTemp == 6 || u8MonthTemp == 9 || u8MonthTemp == 11)
                {
                    u8MaxDayofMon = 30;
                }
                else
                {
                    u8YearTemp = (stClkSetTemp.u8Year>>4)*10;  //BCD to int
                    u8YearTemp += stClkSetTemp.u8Year&0x0f;
                    if((u8YearTemp%4 == 0 && u8YearTemp%100 != 0)||u8YearTemp%400 == 0)
                    {
                        u8MaxDayofMon = 29;
                    }
                    else
                    {
                        u8MaxDayofMon = 28;
                    }
                }
            }
            else
            {
                u8SetTemp = (stClkSetTemp.u8Month>>4)*10; //BCD to int
                u8SetTemp += stClkSetTemp.u8Month&0x0f;
            }
            switch(Key->enKeyValue)
            {
                case enKey_1:break;
                case enKey_2:
                    if(u8SetIndex == 0)
                    {
                        if(--u8SetTemp > u8MaxDayofMon)u8SetTemp = u8MaxDayofMon;
                        stClkSetTemp.u8Day = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }
                    else
                    {
                        if(--u8SetTemp > 12)u8SetTemp = 12;
                        stClkSetTemp.u8Month = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }                  
                    break;
                case enKey_3:
                    if(u8SetIndex == 0)
                    {
                        if(++u8SetTemp>u8MaxDayofMon)u8SetTemp = 0;
                        stClkSetTemp.u8Day = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }
                    else
                    {
                        if(++u8SetTemp>12)u8SetTemp = 0;
                        stClkSetTemp.u8Month = (u8SetTemp/10)<<4 | (u8SetTemp%10);
                    }     
                    break;
                case enKey_4:
                    if(Key->enKeySts == enKeySts_ShortPressed)
                    {
                        if(u8SetIndex == 0)
                        {
                            u8SetIndex = 1;
                        }
                        else
                        {
                            vClk_SetClk(&stClkSetTemp,NULL);
                            enMainDisMode = enDisMode_DisClk;
                            u8SetIndex = 0;
                        }
                    }                    
                    break;
                default:break;
            }
            vDisplayDate(&stClkSetTemp);
            if(u8SetIndex == 0)
            {
                SET_DIG_BLINK(3,enBlink_2HZ);
                SET_DIG_BLINK(2,enBlink_2HZ);
            }
            else
            {
                SET_DIG_BLINK(1,enBlink_2HZ);
                SET_DIG_BLINK(0,enBlink_2HZ);
            }     
            SET_DIG_BLINK(4,enBlink_ON); 
            break;
        default:break;
    }

}

void vDisplayMainTask(void)
{
    static unsigned int u16ActiveCnt = 0;
    tstKey KeyTemp = {enKey_Null,enKeySts_Idle};
    GetKeyFromBuffer(&KeyTemp);
    switch(enMainDisMode)
    {
        case enDisMode_Sleep:
            vSleepHandler(&KeyTemp);
            break;
        case enDisMode_DisClk:
            vClkDisHandler(&KeyTemp);
            break;
        case enDisMode_DisTemp:
            vTempDisHandler(&KeyTemp);
            break;
        case enDisMode_SetClk:
            vSetClkHandler(&KeyTemp);
            break;
        case enDisMode_Timer:break;
        case enDisMode_StopWatch:break;
        case enDisMode_Counter:
            vCounterHandler(&KeyTemp);break;
        default:break;
    }
    if(enMainDisMode != enDisMode_Sleep)
    {
        if(KeyTemp.enKeyValue != enKey_Null)
        {
            u16ActiveCnt = 0;
        }
        else
        {
            u16ActiveCnt += DIS_MAIN_TASK_PERIOD;
            if(u16ActiveCnt > DIS_CLK_ACTIVE_TIME)
            {
                vDisplayOff();
                enMainDisMode = enDisMode_Sleep;
                u16ActiveCnt = 0;
            }
            else
            {
                vDisplayOn();            
            } 
        }
    }
    
}