#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include "adc.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    ADC_Init();

    while (1) {
        if (hadc1_dma1_tx_state > 0) {
            u2_printf("ADC1_IN1: %d.%03d\r\n", (((sum / 10) * 3300) / 4095) / 1000, (((sum / 10) * 3300) / 4095) % 1000);
        }
    }
}