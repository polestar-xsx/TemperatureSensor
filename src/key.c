#include "key.h"

typedef struct
{
    unsigned char u8RingHead;
    unsigned char u8RingEnd;
    unsigned char u8Cnt;
    tstKey *pkeyBuffer;
    /* data */
}tstKeyRingBuffer;

tstProcKey __xdata stCurrentProcKey = {{enKey_Null,enKeySts_Idle},0};

tstKey __xdata stKeyBuffer[KeyBuffNum];

tstKeyRingBuffer __xdata KeyRingBuffer = {0,0,0,NULL};


unsigned char AddKeyIntoBuff(tstKey *pstKey)
{
    if(KeyRingBuffer.u8Cnt<KeyBuffNum)
    {
        KeyRingBuffer.pkeyBuffer[KeyRingBuffer.u8RingEnd] = *pstKey;
        if(KeyRingBuffer.u8RingEnd<(KeyBuffNum-1))
        {
            KeyRingBuffer.u8RingEnd++;
        }
        else
        {
            KeyRingBuffer.u8RingEnd=0;
        }
        KeyRingBuffer.u8Cnt++;
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned char GetKeyFromBuffer(tstKey* pstKey)
{
    if(KeyRingBuffer.u8Cnt>0)
    {
        *pstKey = KeyRingBuffer.pkeyBuffer[KeyRingBuffer.u8RingHead];
        if(KeyRingBuffer.u8RingHead < KeyBuffNum-1)
        {
            KeyRingBuffer.u8RingHead++;
        }
        else
        {
            KeyRingBuffer.u8RingHead=0;             
        }
        KeyRingBuffer.u8Cnt--;
        return 1;
    }
    else
    {
        pstKey->enKeyValue = enKey_Null;
        pstKey->enKeySts = enKeySts_Idle;
        return 0;
    }
} 

tenKey tenKeyScan(void)
{
    unsigned char keytemp = 0x00;
    keytemp = KEY1;
    keytemp = (keytemp << 1) | KEY2;
    keytemp = (keytemp << 1) | KEY3;
    keytemp = (keytemp << 1) | KEY4;
    return ((tenKey)keytemp);
}

void KeyInit(void)
{
    stCurrentProcKey.stKey.enKeyValue = enKey_Null;
    stCurrentProcKey.stKey.enKeySts = enKeySts_Idle;
    stCurrentProcKey.u32DebounceCnt = 0;
    KeyRingBuffer.u8RingHead = 0;
    KeyRingBuffer.u8RingEnd = 0;
    KeyRingBuffer.u8Cnt = 0;
    KeyRingBuffer.pkeyBuffer = stKeyBuffer;
    P3M0 = P3M0 & 0xC3;
    P3M1 = P3M1 & 0xC3;
    //P3PU = P3PU | 0x3C;
    //P3IE = P3PU | 0x3C;
    P3 = P3|0x3C;
    _nop_();
    _nop_();
}

void vKeyTask(void)
{
    tenKey enKeyScaned = tenKeyScan();
    tstKey stKey = {enKey_Null,enKeySts_Idle};
    if(enKeyScaned != enKey_Null)
    {
        if(stCurrentProcKey.stKey.enKeyValue != enKeyScaned)
        {
            stCurrentProcKey.stKey.enKeyValue = enKeyScaned;
            stCurrentProcKey.stKey.enKeySts = enKeySts_Detected;
            stCurrentProcKey.u32DebounceCnt = 0;
        }
        else
        {
            switch(stCurrentProcKey.stKey.enKeySts)
            {
                case enKeySts_Detected:
                    if(++stCurrentProcKey.u32DebounceCnt >= Debounce_ShortPress)
                    {
                        stCurrentProcKey.stKey.enKeySts = enKeySts_ShortPressed;
                        stCurrentProcKey.u32DebounceCnt = 0;
                        stKey = stCurrentProcKey.stKey;
                        AddKeyIntoBuff(&stKey);
                    }
                    break;
                case enKeySts_ShortPressed:
                    if(++stCurrentProcKey.u32DebounceCnt >= Debounce_LongPress)
                    {
                        stCurrentProcKey.stKey.enKeySts = enKeySts_LongPressed;
                        stCurrentProcKey.u32DebounceCnt = 0;
                        stKey = stCurrentProcKey.stKey;
                        AddKeyIntoBuff(&stKey);
                    }
                    break;
                case enKeySts_LongPressed:
                case enKeySts_ContinusTrig:
                    if(++stCurrentProcKey.u32DebounceCnt >= Debounce_ContinuePress)
                    {
                        stCurrentProcKey.stKey.enKeySts = enKeySts_ContinusTrig;
                        stCurrentProcKey.u32DebounceCnt = 0;
                        stKey = stCurrentProcKey.stKey;
                        AddKeyIntoBuff(&stKey);
                    }
                    break;
                default:break;
            }
        }
    }
    else
    {
        stCurrentProcKey.stKey.enKeyValue = enKey_Null;
        stCurrentProcKey.stKey.enKeySts = enKeySts_Idle;
    }
}
