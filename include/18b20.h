#ifndef __18B20_H__
#define __18B20_H__

#include"stc8h.h"

#define DQ_PIN    P23
#define MaxCmdNum  16
#define CMD_SkipROM 0xcc
#define CMD_MatchROM 0x55
#define CMD_CovertT  0x44
#define CMD_ReadScratchPad 0xbe

void v18b20ServiceTask(void);

void vSetReadTempSeq(void);

void vStartCmdSeq(void);



#endif /* __18B20_H__ */
