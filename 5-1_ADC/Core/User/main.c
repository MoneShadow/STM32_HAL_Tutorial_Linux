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
        HAL_ADC_PollForConversion(&hadc1, 1000);
        uint16_t adc_val = HAL_ADC_GetValue(&hadc1);
        uint16_t mv = adc_val * 3300 / 4095;
        u2_printf("ADC_IN: %d.%03d v\r\n", mv / 1000, mv % 1000);   // 先放整除部分，后放小数部分，但是本质还是整数运算
        HAL_Delay(1000);
    }
}