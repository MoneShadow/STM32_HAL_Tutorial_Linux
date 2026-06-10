#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(1000, 36000 - 1, 0);

    while (1) {
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC1)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC1);
            u2_printf("CC1\r\n");
        }
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC2)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC2);
            u2_printf("CC2\r\n");
        }
	}
}