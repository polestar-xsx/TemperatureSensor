#ifndef __IIC_H__
#define __IIC_H__

#define IIC_TX_LENGTH    8
#define IIC_RX_LENGTH    8

void vIIC_Write(unsigned char *,unsigned char);

void vIIC_Read(unsigned char *,unsigned char);

void vIIC_ISR_Handler(void) __interrupt (I2C_VECTOR)  __using (1)

#endif /* __IIC_H__ */
