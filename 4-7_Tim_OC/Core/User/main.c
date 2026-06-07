#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(0xFFFF, 36000 - 1, 0);

    while (1) {

	}
}