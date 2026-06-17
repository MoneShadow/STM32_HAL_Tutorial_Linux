#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include "i2c.h"
#include "MPU6050.h"
#include <stdint.h>

I2C_HandleTypeDef hi2c1;

void I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;   /* 必须显式设置！0≠0x4000 */
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    GPIO_InitTypeDef hgpiob = {0};

    if (hi2c->Instance == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        hgpiob.Mode = GPIO_MODE_AF_OD;
        hgpiob.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        hgpiob.Pull = GPIO_PULLUP;         /* 内部上拉，防总线浮空 */
        hgpiob.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &hgpiob);
    }
}

/* 初始化MPU6050 */
void MPU6050_Init() {
    uint8_t Temp[2];

    /* 唤醒 MPU6050（退出睡眠） */
    Temp[0] = MPU6050_PWR_MGMT_1;
    Temp[1] = 0x01;
    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, Temp, 2, 100) != HAL_OK)
        return;

    HAL_Delay(50);  /* 等待MPU6050内部振荡器+PLL稳定（datasheet要求~30ms） */

    Temp[0] = MPU6050_PWR_MGMT_2;
    Temp[1] = 0x00;
    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, Temp, 2, 100) != HAL_OK)
        return;

    Temp[0] = MPU6050_SMPLRT_DIV;
    Temp[1] = 0x09;
    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, Temp, 2, 100) != HAL_OK)
        return;

    Temp[0] = MPU6050_CONFIG;
    Temp[1] = 0x06;
    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, Temp, 2, 100) != HAL_OK)
        return;

    Temp[0] = MPU6050_GYRO_CONFIG;
    Temp[1] = 0x18;
    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, Temp, 2, 100) != HAL_OK)
        return;

    Temp[0] = MPU6050_ACCEL_CONFIG;
    Temp[1] = 0x18;
    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, Temp, 2, 100) != HAL_OK)
        return;
}

/* 读取六轴的值 */
void MPU_ReadReg(int16_t arr[]) {
    uint8_t p = MPU6050_ACCEL_XOUT_H;
    uint8_t Temp[14] = {0};  /* 初始化为0，防止I2C失败时读到垃圾数据 */

    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_Address, &p, 1, 100) != HAL_OK) {
        for (uint8_t i = 0; i < 7; i++) arr[i] = 0;
        return;
    }

    if (HAL_I2C_Master_Receive(&hi2c1, MPU6050_Address, Temp, 14, 100) != HAL_OK) {
        for (uint8_t i = 0; i < 7; i++) arr[i] = 0;
        return;
    }

    for (uint8_t i = 0; i < 7; i++) {
        arr[i] = (Temp[2 * i] << 8) | Temp[2 * i + 1];
    }
}
