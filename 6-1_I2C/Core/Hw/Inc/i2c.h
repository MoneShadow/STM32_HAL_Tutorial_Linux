#ifndef __I2C_H
#define __I2C_H

void I2C1_Init(void);
void MPU6050_Init();
void MPU_ReadReg(int16_t arr[]);

#endif
