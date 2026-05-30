#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART_Init();

    while (1) {

    }
}