#ifndef __KEY_H__
#define __KEY_H__
#include"stc8h.h"
#include "sch.h"
#define KEY1  P32
#define KEY2  P33
#define KEY3  P34
#define KEY4  P35
#define KeyTaskPeriod  50
#define Debounce_ShortPress  (100/KeyTaskPeriod) /*short press debouce time, 100ms/KeyTaskPeriod*/
#define Debounce_LongPress   (2000/KeyTaskPeriod) /*long press debouce time, 2s/KeyTaskPeriod*/
#define Debounce_ContinuePress  (500/KeyTaskPeriod) /*Continue press debounce time, 500ms/KeyTaskPeriod*/
#define KeyBuffNum       3

typedef enum
{
    enKey_Null = 0x0f,
    enKey_1 = 0b0111,
    enKey_2 = 0b1011,
    enKey_3 = 0b1101,
    enKey_4 = 0b1110,
    enKey_12 = 0b0011,
    enKey_13 = 0b0101,
    enKey_14 = 0b0110,
    enKey_23 = 0b1001,
    enKey_24 = 0b1010,
    enKey_34 = 0b1100,
    enKey_123 = 0b0001,
    enKey_124 = 0b0010,
    enKey_134 = 0b0100,
    enKey_234 = 0b1000,
    enKey1234 = 0b0000,
}tenKey;

typedef enum 
{
    enKeySts_Idle = 0,
    enKeySts_Detected,
    enKeySts_ShortPressed,
    enKeySts_LongPressed,
    enKeySts_ContinusTrig,
}tenKeyState;

typedef struct
{
    tenKey enKeyValue;
    tenKeyState enKeySts;
}tstKey;

typedef struct
{
    tstKey stKey;
    unsigned int u32DebounceCnt;
}tstProcKey;

void vKeyTask(void);

void KeyInit(void);

unsigned char GetKeyFromBuffer(tstKey* pstKey);

#endif /* __KEY_H__ */
