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
    HAL_I2C_Init(&hi2c1);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    GPIO_InitTypeDef hgpiob;

    if (hi2c->Instance == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        hgpiob.Mode = GPIO_MODE_AF_OD;
        hgpiob.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        hgpiob.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &hgpiob);
    }
}

uint8_t MPU6050_PWR_MGMT_1_data = 0x01;
uint8_t MPU6050_PWR_MGMT_2_data = 0x00;
uint8_t MPU6050_SMPLRT_DIV_data = 0x00;
uint8_t MPU6050_CONFIG_data1 = 0x06;
uint8_t MPU6050_CONFIG_data2 = 0x18;
uint8_t MPU6050_CONFIG_data3 = 0x18;

/* 初始化MPU6050 */
void MPU6050_Init() {
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_PWR_MGMT_1, &MPU6050_PWR_MGMT_1_data, 8, 1000);
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_PWR_MGMT_2, &MPU6050_PWR_MGMT_2_data, 8, 1000);
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_SMPLRT_DIV, &MPU6050_SMPLRT_DIV_data, 8, 1000);
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_CONFIG, &MPU6050_CONFIG_data1, 8, 1000);
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_CONFIG, &MPU6050_CONFIG_data2, 8, 1000);
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_CONFIG, &MPU6050_CONFIG_data3, 8, 1000);
}

/* 读取六轴的值 */
void MPU_ReadReg(int16_t arr[]) {
	uint16_t p = MPU6050_ACCEL_XOUT_H;
	uint8_t Temp[14];
	for (uint8_t i = 0; i < 14; i++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, p, 50, 1000) == HAL_OK) {
            HAL_I2C_Master_Receive(&hi2c1, p++, Temp, 14, 1000);
        }
        else {
            for (uint8_t i = 0; i < 7; i++) {
		        arr[i] = 1;
	        }
            return;
        }
	}
	for (uint8_t i = 0; i < 7; i++) {
		arr[i] = (Temp[2 * i] << 8) | Temp[2 * i + 1];
	}
}