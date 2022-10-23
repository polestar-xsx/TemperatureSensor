#include"clk_sd3078.h"
#include <string.h>

typedef enum
{
    enOp_Idle = 0,
    enOp_SetClk,
}tenClkOp;

unsigned char __xdata au8WriteBuffer[8] = {0,};
tstClock __xdata stSetClk = {0,};
vIIC_OpDoneCallback * pCallbackTemp = 0;
tenClkOp __xdata enCurrentOp = enOp_Idle;
tstClock __xdata stClk = {0,};
unsigned char boClockValid = 0;

void vClk_ReadClk(tstClock * pstClk,vIIC_OpDoneCallback* pCallback)
{
    vIIC_Read(Address_Read,(unsigned char *)pstClk,sizeof(tstClock),pCallback);
}

void vClrCTR2(void)
{
    au8WriteBuffer[0] = RegAddress_CTR2;
    au8WriteBuffer[1] = CTR2_WriteEnClr;
    vIIC_Write(Address_Write,au8WriteBuffer,2,pCallbackTemp,1);
}

void vClrCTR1(void)
{
    au8WriteBuffer[0] = RegAddress_CTR1;
    au8WriteBuffer[1] = CTR1_WriteEnClr;
    vIIC_Write(Address_Write,au8WriteBuffer,2,vClrCTR2,1);
}

void vWriteOp(void)
{    
    if(enCurrentOp == enOp_SetClk)
    {
        au8WriteBuffer[0] = RegAddress_Clk;
        memcpy(&au8WriteBuffer[1],&stSetClk,sizeof(tstClock));
        vIIC_Write(Address_Write,au8WriteBuffer,8,vClrCTR1,1);
    }
    
}

void vSetCTR1(void)
{
    au8WriteBuffer[0] = RegAddress_CTR1;
    au8WriteBuffer[1] = CTR1_WriteEn;
    vIIC_Write(Address_Write,au8WriteBuffer,2,vWriteOp,1);
}

void vSetCTR2(void)
{
    au8WriteBuffer[0] = RegAddress_CTR2;
    au8WriteBuffer[1] = CTR2_WriteEn;
    vIIC_Write(Address_Write,au8WriteBuffer,2,vSetCTR1,1);
}

void vClk_SetClk(tstClock * pstClk,vIIC_OpDoneCallback* pCallback)
{
    vSetCTR2();
    memcpy(&stSetClk,pstClk,sizeof(tstClock));
    pCallbackTemp = pCallback;
    enCurrentOp = enOp_SetClk;
}

void vSetClkValid(void)
{
    boClockValid = 1;
}

unsigned char boGetClkValid(void)
{
    return boClockValid;
}


void vUpdateClk(void)
{
    vClk_ReadClk(&stClk,vSetClkValid);
}


void vInitClk(void)
{
    tstClock stClk = {0x50,0x19,0x96,0x05,0x30,0x07,0x22};
    vClk_SetClk(&stClk,0);
}