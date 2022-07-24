#include "stc8h.h"
#include "iic.h"

typedef enum
{
    enStart = 1,
    enSendData,
    enRecvAck,
    enRecvData,
    enSendAck,
    enSendNck,
    enStop,
    enIdle,
}teniic_state;

typedef struct
{
    unsigned char *pu8Buffer;
    unsigned char u8Length;
    unsigned char u8OpIndex;
    teniic_state enIIC_State;
}tstiic_Data;

tstiic_Data stIIC_Data={0,};

void vIIC_Write(unsigned char * pu8TxBuffer,unsigned char u8Write_len)
{
    stIIC_Data.pu8Buffer = pu8TxBuffer;
    stIIC_Data.u8Length = u8Write_len;
    I2CMSCR = 0x81;                             //发送START命令
    stIIC_Data.enIIC_State = enStart;
    stIIC_Data.u8OpIndex = 0;
}

void vIIC_Read(unsigned char * pu8RxBuffer, unsigned char u8Read_len)
{
    stIIC_Data.pu8Buffer = pu8RxBuffer;
    stIIC_Data.u8Length = u8Read_len;
    I2CMSCR = 0x84;                             //发送RECV命令
    stIIC_Data.enIIC_State = enRecvData;
    stIIC_Data.u8OpIndex = 0;
}



void vIIC_ISR_Handler(void) __interrupt (I2C_VECTOR)  __using (1)
{
    if (I2CMSST & 0x40)
    {
        I2CMSST &= ~0x40;                       //清中断标志
        switch(stIIC_Data.enIIC_State)
        {
            case enStart:
                I2CTXD = stIIC_Data.pu8Buffer[stIIC_Data.u8OpIndex];
                stIIC_Data.u8OpIndex++;
                I2CMSCR = 0x82;                             //发送SEND命令
                stIIC_Data.enIIC_State = enRecvAck;
                break;
            case enSendData:
                if(stIIC_Data.u8OpIndex >= stIIC_Data.u8Length)
                {
                    stIIC_Data.enIIC_State = enStop;
                }
                else
                {
                    I2CTXD = stIIC_Data.pu8Buffer[stIIC_Data.u8OpIndex];
                    stIIC_Data.u8OpIndex++;
                    I2CMSCR = 0x82;                             //发送SEND命令
                    stIIC_Data.enIIC_State = enRecvAck;
                }
                break;
            case enRecvAck:
                I2CMSCR = 0x83;                             //发送读ACK命令
                if(stIIC_Data.u8OpIndex >= stIIC_Data.u8Length)
                {                    
                    stIIC_Data.enIIC_State = enStop;
                }
                else
                {
                    stIIC_Data.enIIC_State = enSendData;
                }
                break;
            case enRecvData:
                stIIC_Data.pu8Buffer[stIIC_Data.u8OpIndex] = I2CRXD;
                if(++stIIC_Data.u8OpIndex >= stIIC_Data.u8Length)
                {
                    I2CMSST = 0x01;                             //设置NACK信号
                    I2CMSCR = 0x85;                             //发送ACK命令
                    stIIC_Data.enIIC_State = enSendNck;
                }
                else
                {
                    I2CMSST = 0x00;                             //设置ACK信号
                    I2CMSCR = 0x85;                             //发送ACK命令
                    stIIC_Data.enIIC_State = enSendAck;
                }
                break;
            case enSendAck:
                I2CMSCR = 0x84;                             //发送RECV命令
                stIIC_Data.enIIC_State = enRecvData;
                break;
            case enSendNck:
                I2CMSCR = 0x86;                             //发送STOP命令
                stIIC_Data.enIIC_State = enIdle;
                break;
            case enStop:
                I2CMSCR = 0x86;                             //发送STOP命令
                stIIC_Data.enIIC_State = enIdle;
                break;
            case enIdle:
                stIIC_Data.u8OpIndex = 0;
                stIIC_Data.u8Length = 0;
                stIIC_Data.pu8Buffer = 0;
                break;
            default:break;
        }
    }
}