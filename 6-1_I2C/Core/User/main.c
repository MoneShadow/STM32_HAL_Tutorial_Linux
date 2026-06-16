#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include "adc.h"
#include "i2c.h"
#include <stdint.h>

int16_t arr[6];

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();
    I2C1_Init();
    MPU6050_Init();

    MPU_ReadReg(arr);

    for (uint8_t i = 0; i < 6; i++) {
        u1_printf("%d\r\n", arr[i]);
    }

    while (1) {
        
    }
}