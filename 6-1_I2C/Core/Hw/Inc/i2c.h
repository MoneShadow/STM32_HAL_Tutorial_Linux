#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

void I2C1_Init(void);
void MPU6050_Init(void);
void MPU_ReadReg(int16_t arr[]);
uint8_t I2C_IsMPU6050Ready(void);  /* 返回 0=设备未就绪, 非0=已检测到设备 */

#endif
