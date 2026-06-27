#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "FreeRTOS_Demo.h"
#include "uart.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    LED_Init();
    UART_Init();

    // u1_printf("Hello FreeRTOS\r\n");

    FreeRTOS_Start();

    while (1) {
    }
}