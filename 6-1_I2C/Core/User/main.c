#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include "adc.h"
#include "i2c.h"
#include "i2c_sf.h"
#include "MPU6050.h"
#include <stdint.h>

int16_t arr[7];

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();
    I2C1_SF_Init();
    MPU6050_Init_SF();    
    // I2C1_SF_Start();
    // I2C1_SF_Transmit(MPU6050_Address);
    // uint8_t ACK = I2C1_SF_Rceive_ACK();
    // I2C1_SF_Stop();

    // u1_printf("ACK: %d\r\n", ACK);

    MPU6050_ReadReg_SF(arr);

    u1_printf("AX=      %d\r\n", arr[0]);
    u1_printf("AY=      %d\r\n", arr[1]);
    u1_printf("AZ=      %d\r\n", arr[2]);
    // u1_printf("Temp=    %d\r\n", arr[3]);
    u1_printf("GX=      %d\r\n", arr[4]);
    u1_printf("GY=      %d\r\n", arr[5]);
    u1_printf("GZ=      %d\r\n\r\n", arr[6]);

    while (1) {
        
    }
}