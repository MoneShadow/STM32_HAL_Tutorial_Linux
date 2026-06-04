#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(2000 - 1, 36000 - 1, 0);

    uint8_t num = 1;
    
    while (1) {
        if (__HAL_TIM_GET_FLAG(&Tim1_InitStructure, TIM_FLAG_UPDATE)) {
            __HAL_TIM_CLEAR_FLAG(&Tim1_InitStructure, TIM_FLAG_UPDATE);
            u2_prinf("Time: %d\r\n", num++);
            if (num > 5) {
                HAL_TIM_Base_Stop(&Tim1_InitStructure);
                HAL_TIM_Base_DeInit(&Tim1_InitStructure);
            }
        }    
	}
}