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
    Timer1_Init(0xFFFF, 1 - 1, 0);

    int16_t num;

    while (1) {
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC1)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC1);
            if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&Tim_InitStructure)) {
                num = (int16_t)__HAL_TIM_GET_COUNTER(&Tim_InitStructure) / 2;
                u2_printf("Num: %d\r\n", num);
            }
            else {
                num = (int16_t)__HAL_TIM_GET_COUNTER(&Tim_InitStructure) / 2;
                u2_printf("Num: %d\r\n", num);
            }
        }
	}
}