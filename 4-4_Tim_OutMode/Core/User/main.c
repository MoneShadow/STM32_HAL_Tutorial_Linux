#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(4, 0, 0);
    
    while (1) {
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_TRIGGER)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_TRIGGER);
            u2_prinf("Time: %d\r\n", __HAL_TIM_GET_COUNTER(&Tim_InitStructure));
        }
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);
            u2_prinf("Time\r\n");
        }        
	}
}