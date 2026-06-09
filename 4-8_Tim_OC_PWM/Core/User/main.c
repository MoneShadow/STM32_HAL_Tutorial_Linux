#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(100, 7200 - 1, 0);

    while (1) {
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC1)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC1);
            u2_printf("PWM: 1\r\n");
        }
	}
}