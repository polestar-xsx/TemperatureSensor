#include "18b20.h"
#include "stc8h.h"
typedef enum 
{
    enIdle = 0,
    enReset,
    enPresence,
    enSendCmd,
    enRecvData,
}tenState_18b20;

typedef struct
{
    tenState_18b20 SeqState;
    unsigned char u8CmdLength;
    unsigned int u16Skiploops;
    unsigned char au8CmdData[4];    
}tstCmdSeq;

tstCmdSeq __xdata astCmdSeq[MaxCmdNum]={{enIdle,0,0,{0,0,0,0}},};
unsigned char au8RecvBuffer[9] = {0,0,0,0,0,0,0,0,0};
unsigned char boRunCmdSeq = 0;

void Delay15us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 90;
	while (--i);
}

void Delay5us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 27;
	while (--i);
}

void Delay1us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 3;
	while (--i);
}

void v18b20SendCmd(unsigned char* cmdbuffer,unsigned char length)
{
    unsigned char sendcnt;
    for(sendcnt = 0;sendcnt < 8 * length; sendcnt++)
    {
        DQ_PIN = 0;
        Delay1us();
        DQ_PIN = (cmdbuffer[sendcnt>>3]>>(sendcnt&0x07))&0x01;
        Delay15us();
        Delay15us();
        Delay15us();
        Delay15us();
        DQ_PIN = 1;
    }
    
}

void v18b20ReadData(unsigned char* revbuffer, unsigned char length)
{
    unsigned char rcvcnt;
    for(rcvcnt = 0;rcvcnt < 8 * length; rcvcnt++)
    {
        DQ_PIN = 0;
        Delay1us();
        DQ_PIN = 1;
        Delay5us();
        Delay5us();
        if(DQ_PIN)
        {
            revbuffer[rcvcnt>>3] = revbuffer[rcvcnt>>3] | (1<<(rcvcnt&0x07));
        }
        Delay15us();
        Delay15us();
        Delay15us();
        Delay5us();
    }
}

void v18b20ServiceTask(void)
{
    static unsigned char u8CmdSeqIndex = 0;
    static unsigned int u16SkipCnt = 0;
    if(boRunCmdSeq == 1)
    {
        //P20 = 0;
        if(u16SkipCnt == 0)
        {
            switch(astCmdSeq[u8CmdSeqIndex].SeqState)
            {
                case enIdle:            //sequence finished
                    boRunCmdSeq = 0;
                    DQ_PIN = 1;
                    //u8CmdSeqIndex = 0;
                    //u16SkipCnt = 0;
                    break;
                case enReset:
                    DQ_PIN = 0;
                    break;
                case enPresence:
                    DQ_PIN = 1;
                    break;
                case enSendCmd:
                    v18b20SendCmd(astCmdSeq[u8CmdSeqIndex].au8CmdData,astCmdSeq[u8CmdSeqIndex].u8CmdLength);
                    break;
                case enRecvData:
                    au8RecvBuffer[0] = 0;
                    au8RecvBuffer[1] = 0;
                    v18b20ReadData(au8RecvBuffer,2);
                    DQ_PIN = 0;
                    break;
                default:break;
            }
            if(boRunCmdSeq == 0)
            {
                u8CmdSeqIndex = 0;
                u16SkipCnt = 0;
            }
            else
            {
                if(0 == astCmdSeq[u8CmdSeqIndex].u16Skiploops)
                {
                    u8CmdSeqIndex++;
                    u16SkipCnt = 0;
                }
                else
                {
                    u16SkipCnt++;
                }
            }
            
        }
        else
        {
            if(u16SkipCnt >= astCmdSeq[u8CmdSeqIndex].u16Skiploops)
            {
                u8CmdSeqIndex++;
                u16SkipCnt = 0;
            }
            else
            {
                u16SkipCnt++;
            }
        }        
    }
}

void vSetReadTempSeq(void)
{
    astCmdSeq[0].SeqState = enReset;
    astCmdSeq[0].u16Skiploops = 0;
    astCmdSeq[1].SeqState = enPresence;
    astCmdSeq[1].u16Skiploops = 0;
    astCmdSeq[2].SeqState = enSendCmd;
    astCmdSeq[2].u16Skiploops = 380;
    astCmdSeq[2].au8CmdData[0] = CMD_SkipROM;
    astCmdSeq[2].au8CmdData[1] = CMD_CovertT;
    astCmdSeq[2].u8CmdLength = 2;

    astCmdSeq[3].SeqState = enReset;
    astCmdSeq[3].u16Skiploops = 0;
    astCmdSeq[4].SeqState = enPresence;
    astCmdSeq[4].u16Skiploops = 0;
    astCmdSeq[5].SeqState = enSendCmd;
    astCmdSeq[5].u16Skiploops = 0;
    astCmdSeq[5].au8CmdData[0] = CMD_SkipROM;
    astCmdSeq[5].au8CmdData[1] = CMD_ReadScratchPad;
    astCmdSeq[5].u8CmdLength = 2;
    astCmdSeq[6].SeqState = enRecvData;
    astCmdSeq[6].u16Skiploops = 0;

    astCmdSeq[7].SeqState = enIdle;
    astCmdSeq[7].u16Skiploops = 0;
}

void vStartCmdSeq(void)
{
    boRunCmdSeq = 1;
}


