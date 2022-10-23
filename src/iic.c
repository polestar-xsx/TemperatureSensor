#include "stc8h.h"
#include "sch.h"
#include "iic.h"

typedef enum
{
    enIdle = 0,
    enSendDevAddress,
    enSendData,
    enWaitAddressAck,
    enWaitDataAck,
    enRecvData,
    enSendNck,
    enSendAck,
    enStop,    
}teniic_state;

typedef enum 
{
    enNone = 0,
    enRead = 1,
    enWrite = 2,
}teniic_op;

typedef struct
{
    unsigned char *pu8Buffer;
    vIIC_OpDoneCallback *pCallback;
    unsigned char u8Length;
    unsigned char u8BufferIndex;
    unsigned char u8Address;
    unsigned char boSendStop;
    teniic_op u8Operation;
    teniic_state enIIC_State;
}tstiic_Data;

tstiic_Data stIIC_Data={0,0,0,0,0,0,0,0};

void vIIC_Init(void)
{
    P_SW2 = 0x90;
    I2CCFG = 0xf8; //speed:24M 100K
    I2CMSST = 0x00;
    P_SW2 = 0x10;
    __sbit SDA = P24;
    __sbit SCL = P25;
}

void vIIC_Write(unsigned char u8address,unsigned char * pu8TxBuffer,unsigned char u8Write_len,vIIC_OpDoneCallback* pCallback,unsigned char boSendStop)
{
    stIIC_Data.pu8Buffer = pu8TxBuffer;
    stIIC_Data.u8Length = u8Write_len;
    stIIC_Data.u8Address = u8address;
    stIIC_Data.pCallback = pCallback;
    stIIC_Data.u8Operation = enWrite;
    stIIC_Data.enIIC_State = enSendDevAddress;
    stIIC_Data.u8BufferIndex = 0;
    stIIC_Data.boSendStop = boSendStop;
    P_SW2 |= 0x80;
    I2CMSCR = 0x81;                             //发送START命令
    P_SW2 &= 0x7f;
    
}

void vIIC_Read(unsigned char u8address,unsigned char * pu8RxBuffer, unsigned char u8Read_len,vIIC_OpDoneCallback* pCallback)
{
    stIIC_Data.pu8Buffer = pu8RxBuffer;
    stIIC_Data.u8Length = u8Read_len;
    stIIC_Data.u8Address = u8address;
    stIIC_Data.pCallback = pCallback;
    stIIC_Data.u8Operation = enRead;
    stIIC_Data.enIIC_State = enSendDevAddress;
    stIIC_Data.u8BufferIndex = 0;
    stIIC_Data.boSendStop = 1;
    P_SW2 |= 0x80;
    I2CMSCR = 0x81;                             //发送START命令
    P_SW2 &= 0x7f;
}



void vIIC_ISR_Handler(void) __interrupt (I2C_VECTOR)  __using (1)
{
    P_SW2 |= 0x80;
    if (I2CMSST & 0x40)
    {
        I2CMSST &= ~0x40;                       //清中断标志
        switch(stIIC_Data.enIIC_State)
        {
            case enSendDevAddress:
                I2CTXD = stIIC_Data.u8Address;
                I2CMSCR = 0x82;                             //发送SEND命令
                stIIC_Data.enIIC_State = enWaitAddressAck;
                break;
            case enWaitAddressAck:
                I2CMSCR = 0x83;                             //发送读ACK命令
                if(enWrite == stIIC_Data.u8Operation)
                {
                    stIIC_Data.enIIC_State = enSendData;
                }
                else
                {
                    stIIC_Data.enIIC_State = enRecvData;
                }
                break;
            case enSendData:
                I2CTXD = stIIC_Data.pu8Buffer[stIIC_Data.u8BufferIndex];
                I2CMSCR = 0x82;                             //发送SEND命令
                stIIC_Data.enIIC_State = enWaitDataAck;
                ++stIIC_Data.u8BufferIndex;
                break;
            case enWaitDataAck:
                I2CMSCR = 0x83;
                if(stIIC_Data.u8BufferIndex >= stIIC_Data.u8Length)
                {
                    if(1 == stIIC_Data.boSendStop)
                    {
                        stIIC_Data.enIIC_State = enStop;
                    }
                    else
                    {
                        stIIC_Data.enIIC_State = enIdle;
                    }
                }
                else
                {
                    stIIC_Data.enIIC_State = enSendData;
                }
                break;
            case enRecvData:
                I2CMSCR = 0x84;//发送RECV命令
                if(stIIC_Data.u8BufferIndex + 1 >= stIIC_Data.u8Length)
                {
                    stIIC_Data.enIIC_State = enSendNck;
                }
                else
                {
                    stIIC_Data.enIIC_State = enSendAck;
                }
                break;
            case enSendAck:
                stIIC_Data.pu8Buffer[stIIC_Data.u8BufferIndex] = I2CRXD;
                I2CMSST = 0x00;                             //设置ACK信号
                I2CMSCR = 0x85;                             //发送ACK命令
                stIIC_Data.enIIC_State = enRecvData;
                stIIC_Data.u8BufferIndex++;
                break;
            case enSendNck:
                stIIC_Data.pu8Buffer[stIIC_Data.u8BufferIndex] = I2CRXD;
                I2CMSST = 0x01;                             //设置NACK信号
                I2CMSCR = 0x85;                             //发送ACK命令
                stIIC_Data.enIIC_State = enStop;
                break;
            case enStop:
                I2CMSCR = 0x86;                             //发送STOP命令
                stIIC_Data.enIIC_State = enIdle;
                break;
            case enIdle:
                stIIC_Data.u8BufferIndex = 0;
                stIIC_Data.u8Length = 0;
                stIIC_Data.pu8Buffer = 0;
                if(NULL != stIIC_Data.pCallback)
                {
                    (*stIIC_Data.pCallback)();
                }
                break;
            default:break;
        }
    }
    P_SW2 &= 0x7f;
}