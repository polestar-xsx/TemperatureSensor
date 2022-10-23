#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include "sch.h"
#include "key.h"
#include "stc8h.h"
#include "18b20.h"
#include "clk_sd3078.h"

#define LED_R P20
#define LED_G P21
#define LED_B P22
#define DE    P0
#define DE1   P00
#define DE2   P01
#define DE3   P02
#define DE4   P03
#define DIG   P1

#define DIS_MAIN_TASK_PERIOD         50  /*main task period*/
#define DIS_FRESH_TASK_PERIOD        2   /*fresh task period*/
#define BLK_05HZ_HALF_PERIOD         1000
#define BLK_05HZ_FULL_PERIOD         2000
#define BLK_1HZ_HALF_PERIOD          500
#define BLK_1HZ_FULL_PERIOD          1000
#define BLK_2HZ_HALF_PERIOD          250
#define BLK_2HZ_FULL_PERIOD          500

#define DIS_CLK_ACTIVE_TIME          10000  //Clock display time without any key input

typedef enum 
{
    enDisMode_Sleep = 0,
    enDisMode_DisClk,
    enDisMode_SetClk,
    enDisMode_DisTemp,
    enDisMode_StopWatch,
    enDisMode_Timer,
    enDisMode_Counter,
}tenDisplayMode;

typedef enum 
{
    enClkDisMode_Time = 0,
    enClkDisMode_Year,
    enClkDisMode_Date,
    enClkDisMode_Set,
}tenClkDisplayMode;

typedef enum 
{
    enBlink_OFF = 0,
    enBlink_ON = 1,
    enBlink_05HZ = 2,
    enBlink_1HZ = 3,
    enBlink_2HZ = 4,
}tenBlinkFreq;

void vDisplayFresh(void);

void vDisplayMainTask(void);

void vDisplayInit(void);

#endif /* __DISPLAY_H__ */
