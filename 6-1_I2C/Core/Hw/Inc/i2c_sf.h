#ifndef __I2C_SF_H
#define __I2C_SF_H

void I2C1_SF_Init(void);

void I2C1_SF_Start(void);
void I2C1_SF_Stop(void);
void I2C1_SF_Transmit(uint8_t Data);
uint8_t I2C1_SF_Receive(void);
void I2C1_SF_Transmit_ACK(uint8_t Valu);
uint8_t I2C1_SF_Receive_ACK(void);

void MPU6050_Init_SF();
void MPU6050_ReadReg_SF(int16_t *arr);

#endif