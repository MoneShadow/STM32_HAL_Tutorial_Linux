#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "FreeRTOS_Demo.h"
#include "uart.h"
#include "Key.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    LED_Init();
    UART_Init();
    Key_Init();

    FreeRTOS_Start();

    while (1) {

    }
}