#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(0xFFFF, 7200 - 1, 0);
    
    uint16_t count = 0;

    while (1) {
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);
            u2_prinf("Updata: %d\r\n", ++count);
        }
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC1)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC1);
            u2_prinf("ps: %d\r\n", HAL_TIM_ReadCapturedValue(&Tim_InitStructure, TIM_CHANNEL_1) + (count * 65535));
        }
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC2)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC2);

        }
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC3)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC3);

        }
        if (__HAL_TIM_GET_FLAG(&Tim_InitStructure, TIM_FLAG_CC4)) {
            __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_CC4);

        }
	}
}