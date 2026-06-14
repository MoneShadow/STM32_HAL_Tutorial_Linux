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

    HAL_ADC_PollForConversion(&hadc1, 1000);
    u2_printf("ADC_IN: %d\r\n", HAL_ADC_GetValue(&hadc1));

    while (1) {
           
    }
}