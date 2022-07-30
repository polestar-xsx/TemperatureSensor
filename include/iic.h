#ifndef __IIC_H__
#define __IIC_H__
#include "stc8h.h"
#define IIC_TX_LENGTH    8
#define IIC_RX_LENGTH    8

typedef void vIIC_OpDoneCallback(void);

void vIIC_Init(void);

void vIIC_Write(unsigned char u8address,unsigned char * pu8TxBuffer,unsigned char u8Write_len,vIIC_OpDoneCallback* pCallback,unsigned char boSendStop);

void vIIC_Read(unsigned char u8address,unsigned char * pu8RxBuffer, unsigned char u8Read_len,vIIC_OpDoneCallback* pCallback);

void vIIC_ISR_Handler(void) __interrupt (I2C_VECTOR)  __using (1);

#endif /* __IIC_H__ */
