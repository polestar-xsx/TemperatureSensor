#ifndef __CLK_SD3078_H__
#define __CLK_SD3078_H__

#define Address_Read   0x65
#define Address_Write  0x64

#define RegAddress_Clk 0x00
#define RegAddress_CTR2 0x10
#define RegAddress_CTR1 0x0f

#define CTR2_WriteEn      0x80
#define CTR2_WriteEnClr   0x00
#define CTR1_WriteEn      0x84
#define CTR1_WriteEnClr   0x00
typedef struct
{
    unsigned char u8Sec;
    unsigned char u8Min;
    unsigned char u8Hour;
    unsigned char u8Week;
    unsigned char u8Day;
    unsigned char u8Month;
    unsigned char u8Year;
}tstClock;


void vClk_ReadClk(tstClock * pstClk,vIIC_OpDoneCallback* pCallback);

void vClk_SetClk(tstClock * pstClk,vIIC_OpDoneCallback* pCallback);

#endif /* __CLK_SD3078_H__ */
