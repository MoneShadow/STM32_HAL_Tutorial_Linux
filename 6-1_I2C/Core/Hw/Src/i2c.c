#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include "i2c.h"
#include "MPU6050.h"
#include <stdint.h>

/*
 * MPU6050 7位地址:
 *   AD0=GND → 0x68, 8位地址 = 0x68<<1 = 0xD0
 *   AD0=VCC → 0x69, 8位地址 = 0x69<<1 = 0xD2
 * STM32F1 HAL 的 DevAddress 参数使用左移1位后的8位地址格式。
 */
#define MPU6050_ADDR_LO  0xD0
#define MPU6050_ADDR_HI  0xD2

I2C_HandleTypeDef hi2c1;
static uint8_t mpu6050_devaddr = 0;  /* 0=设备未就绪 */

/*
 * I2C 总线恢复：当I2C从机锁死总线时（SDA被拉低，BUSY标志卡死），
 * 通过GPIO模拟时钟脉冲来释放从机，并发送STOP条件清除BUSY标志。
 * 注意：此函数要求 GPIOB 和 I2C1 时钟已开启。
 */
static void I2C_BusRecovery(void) {
    GPIO_InitTypeDef gpio = {0};

    /* 确保必要的时钟已开启（HAL_I2C_MspInit 也会开，此处做双重保险） */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* 禁用 I2C 外设 */
    CLEAR_BIT(hi2c1.Instance->CR1, I2C_CR1_PE);

    /* 将 PB6(SCL) 和 PB7(SDA) 临时切换为 GPIO 开漏输出 */
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* 先将 SDA 拉高 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

    /*
     * 发送最多 9 个 SCL 时钟脉冲。
     * 如果从机在输出数据的过程中被复位，它可能正在将 SDA 拉低。
     * 通过提供 SCL 脉冲，可以让从机完成当前字节传输并释放 SDA。
     */
    for (uint8_t i = 0; i < 9; i++) {
        /* 检查 SDA 是否已被释放 */
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET) {
            break;  /* SDA 已经为高，无需继续时钟脉冲 */
        }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    /* 产生 STOP 条件: SDA 从低变高 且 SCL 为高 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1);

    /* 恢复 I2C 复用功能，开启内部上拉防止总线浮空 */
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* 重新使能 I2C 外设 */
    SET_BIT(hi2c1.Instance->CR1, I2C_CR1_PE);
}

void I2C1_Init(void) {
    /* 显式设置所有初始化字段，避免依赖零初始化的默认值 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.OwnAddress2 = 0;
    HAL_I2C_Init(&hi2c1);

    /* 执行 I2C 总线恢复，清除可能卡死的 BUSY 标志 */
    I2C_BusRecovery();

    /*
     * 自动检测 MPU6050 的 I2C 地址。
     * HAL_I2C_IsDeviceReady 发送 START + 设备地址，检查从机是否 ACK。
     * 大多数模块 AD0 接 GND（地址 0xD0），部分模块 AD0 接 VCC（地址 0xD2）。
     */
    if (HAL_I2C_IsDeviceReady(&hi2c1, MPU6050_ADDR_LO, 3, 100) == HAL_OK) {
        mpu6050_devaddr = MPU6050_ADDR_LO;
    } else if (HAL_I2C_IsDeviceReady(&hi2c1, MPU6050_ADDR_HI, 3, 100) == HAL_OK) {
        mpu6050_devaddr = MPU6050_ADDR_HI;
    }
    /* 若两个地址都无应答，mpu6050_devaddr 保持 0，后续操作将跳过 */
}

uint8_t I2C_IsMPU6050Ready(void) {
    return mpu6050_devaddr != 0;
}

/* 初始化MPU6050 */
void MPU6050_Init(void) {
    uint8_t data;

    if (mpu6050_devaddr == 0) return;  /* 设备未就绪 */

    /* PWR_MGMT_1 (0x6B): 唤醒MPU6050 */
    data = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, mpu6050_devaddr, MPU6050_PWR_MGMT_1,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* PWR_MGMT_2 (0x6C): 所有传感器使能 */
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, mpu6050_devaddr, MPU6050_PWR_MGMT_2,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* SMPLRT_DIV (0x19): 采样率不分频，陀螺仪 1kHz */
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, mpu6050_devaddr, MPU6050_SMPLRT_DIV,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* CONFIG (0x1A): DLPF 带宽 ~5Hz */
    data = 0x06;
    HAL_I2C_Mem_Write(&hi2c1, mpu6050_devaddr, MPU6050_CONFIG,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* GYRO_CONFIG (0x1B): 陀螺仪 ±2000°/s */
    data = 0x18;
    HAL_I2C_Mem_Write(&hi2c1, mpu6050_devaddr, MPU6050_GYRO_CONFIG,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    /* ACCEL_CONFIG (0x1C): 加速度计 ±16g */
    data = 0x18;
    HAL_I2C_Mem_Write(&hi2c1, mpu6050_devaddr, MPU6050_ACCEL_CONFIG,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

/* 读取六轴+温度数据（14字节→7个int16_t） */
void MPU_ReadReg(int16_t arr[]) {
    uint8_t Temp[14];

    if (mpu6050_devaddr == 0) {
        for (uint8_t i = 0; i < 7; i++) arr[i] = 0;
        return;
    }

    /*
     * HAL_I2C_Mem_Read 内部使用 Repeated START:
     *   S + DevAddr(W) + MemAddr + Sr + DevAddr(R) + Data[0..13] + P
     * 比分开调用 Transmit+Receive 更可靠（单次事务，不会被其他主设备打断）。
     */
    if (HAL_I2C_Mem_Read(&hi2c1, mpu6050_devaddr, MPU6050_ACCEL_XOUT_H,
                         I2C_MEMADD_SIZE_8BIT, Temp, 14, 100) != HAL_OK) {
        for (uint8_t i = 0; i < 7; i++) arr[i] = 0;
        return;
    }

    /*
     * MPU6050 数据为大端字节序:
     * arr[0]=AX, arr[1]=AY, arr[2]=AZ,
     * arr[3]=Temp, arr[4]=GX, arr[5]=GY, arr[6]=GZ
     */
    for (uint8_t i = 0; i < 7; i++) {
        arr[i] = (Temp[2 * i] << 8) | Temp[2 * i + 1];
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    GPIO_InitTypeDef gpio = {0};

    if (hi2c->Instance == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /*
         * PB6=SCL, PB7=SDA: 开漏复用输出 + 内部上拉。
         * 内部上拉(~40kΩ)很弱，仅防止总线浮空。I2C通信仍需外部4.7kΩ上拉电阻！
         */
        gpio.Mode = GPIO_MODE_AF_OD;
        gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        gpio.Pull = GPIO_PULLUP;
        gpio.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &gpio);
    }
}
