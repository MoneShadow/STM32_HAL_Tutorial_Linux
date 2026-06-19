#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "MPU6050.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>

#define  SCK_SET()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define  SCK_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define  SDA_SET()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define  SDA_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)

/*
 * @72MHz, 每个周期约 1/72M ≈ 14ns
 * 180 次循环 × 2周期 ≈ 360周期 ≈ 5μs → SCL 约 100kHz
 */
#define I2C_DELAY()  do { for (volatile uint32_t _d = 0; _d < 180; _d++) { __NOP(); } } while(0)

void I2C1_SF_Init(void) {
    /* 初始化I2C 软件模拟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef hgpiob = {0};
    hgpiob.Mode = GPIO_MODE_OUTPUT_OD;
    hgpiob.Pin = GPIO_PIN_6 | GPIO_PIN_7;   // 6-SCK 7-SDA
    hgpiob.Pull = GPIO_PULLUP;              // 内部上拉（外部4.7kΩ仍推荐）
    hgpiob.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &hgpiob);
}

/* 软件模拟产生开始标志位 */
void I2C1_SF_Start(void) {
    SDA_SET();
    I2C_DELAY();
    SCK_SET();
    I2C_DELAY();
    /* 拉低数据线 产生开始标志 */
    SDA_RESET();
    I2C_DELAY();
    SCK_RESET();
    I2C_DELAY();
}

/* 软件模拟产生结束标志位 */
void I2C1_SF_Stop(void) {
    SDA_RESET();
    I2C_DELAY();
    SCK_SET();
    I2C_DELAY();
    /* 拉高数据线 产生结束标志位 */
    SDA_SET();
    I2C_DELAY();
    SCK_RESET();
    I2C_DELAY();
}

/* 软件模拟发送数据(单次 8字节) */
void I2C1_SF_Transmit(uint8_t Data) {
    /* 发送数据 */
    for (uint8_t i = 0; i < 8; i++) {
        if ((Data & (0x80 >> i)) == 0) {    // 判断高低电平
            SDA_RESET();    // 低电平
        }
        else {
            SDA_SET();      // 高电平
        }
        I2C_DELAY();        // tSU;DAT: 数据建立时间
        SCK_SET();          // 时钟置高电平 从机读取数据
        I2C_DELAY();        // tHIGH: SCL高电平保持
        SCK_RESET();        // 时钟置低电平 准备下一次发送数据
        I2C_DELAY();        // tLOW: SCL低电平保持
    }
}

/* 软件模拟接收数据(单次 8字节) */
uint8_t I2C1_SF_Receive(void) {
    /* 主机释放SDA总线 总线交由从机控制 */
    SDA_SET();
    I2C_DELAY();
    uint8_t Valu = 0;   // 用于接收从机发送的数据
    for (uint8_t i = 0; i < 8; i++) {
        SCK_SET();      // 时钟置高电平 主机开始读取数据
        I2C_DELAY();    // tVD;DAT: 等从机把数据放到SDA上（MPU6050 最大900ns）
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET) {
            Valu |= (0x80 >> i);    // 高位右移(和上面的发送数据一致 高位先行)
        }
        SCK_RESET();    // 时钟置低电平 等待从机准备下一个数据位
        I2C_DELAY();
    }
    return Valu;
}

/* 软件模拟发送应答位 */
void I2C1_SF_Transmit_ACK(uint8_t Valu) {
    if (Valu == 0) {
        SDA_RESET();    // 0 代表有应答
    }
    else {
        SDA_SET();      // 1 代表不应答
    }
    I2C_DELAY();
    SCK_SET();          // 时钟置高电平 等待从机接收应答
    I2C_DELAY();
    SCK_RESET();        // 时钟置低电平 等待下一步操作
    I2C_DELAY();
}

/* 软件模拟接收应答 */
uint8_t I2C1_SF_Receive_ACK(void) {
    uint8_t ACK = 0;    // 默认从机给应答
    /* 主机释放SDA总线 总线交由从机控制 */
    SDA_SET();
    I2C_DELAY();
    SCK_SET();          // 时钟置高电平 主机接收应答
    I2C_DELAY();        // tVD;ACK: 等从机把ACK放到SDA上
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET) {
        ACK = 1;
    }
    SCK_RESET();        // 时钟置低电平 等待下一步操作
    I2C_DELAY();
    return ACK;
}

/*  指定地址写 时序：S -> DiviceAddress_W -> RegAddress -> TransmitData -> P */
void I2C1_SF_Write_Random(uint8_t RegAddress, uint8_t Data) {
    I2C1_SF_Start();
    I2C1_SF_Transmit(MPU6050_Address_W);
    I2C1_SF_Receive_ACK();
    I2C1_SF_Transmit(RegAddress);
    I2C1_SF_Receive_ACK();
    I2C1_SF_Transmit(Data);
    I2C1_SF_Receive_ACK();
    I2C1_SF_Stop();
}

/* 指定地址读 时序：S -> DiviceAddress_W -> RegAddress -> Sr -> DiviceAddress_R -> RecieveData -> P */
uint8_t I2C1_SF_Read_Random(uint8_t RegAddress) {
    uint8_t Valu = 0;
    I2C1_SF_Start();
    I2C1_SF_Transmit(MPU6050_Address_W);
    I2C1_SF_Receive_ACK();
    I2C1_SF_Transmit(RegAddress);
    I2C1_SF_Receive_ACK();
    I2C1_SF_Start();
    I2C1_SF_Transmit(MPU6050_Address_R);
    I2C1_SF_Receive_ACK();
    Valu = I2C1_SF_Receive();
    I2C1_SF_Transmit_ACK(1);
    I2C1_SF_Stop();
    return Valu;
}

/*---------MPU6050---------*/

/* 初始化MPU6050 */
void MPU6050_Init_SF() {
    I2C1_SF_Write_Random(MPU6050_PWR_MGMT_1, 0x01);
    HAL_Delay(50);
    I2C1_SF_Write_Random(MPU6050_PWR_MGMT_2, 0x00);
    I2C1_SF_Write_Random(MPU6050_SMPLRT_DIV, 0x09);
    I2C1_SF_Write_Random(MPU6050_CONFIG, 0x06);
    I2C1_SF_Write_Random(MPU6050_GYRO_CONFIG, 0x18);
    I2C1_SF_Write_Random(MPU6050_ACCEL_CONFIG, 0x18);
}

/* 读取六轴的值(温度只读不用) */
void MPU6050_ReadReg_SF(int16_t *arr) {
    uint8_t Temp[14];
    uint8_t p = MPU6050_ACCEL_XOUT_H;
    for (uint8_t i = 0; i < 14; i++) {
        Temp[i] = I2C1_SF_Read_Random(p++);
    }
    for (uint8_t i = 0; i < 7; i++) {                       //  0   1   2   3   4    5     6
        arr[i] = (Temp[i * 2] << 8) | (Temp[i * 2 + 1]);    // 0/1 2/3 4/5 6/7 8/9 10/11 12/13
    }
}
