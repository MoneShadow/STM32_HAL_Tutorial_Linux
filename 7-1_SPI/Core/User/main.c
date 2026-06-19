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

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();

    while (1) {
        
    }
}