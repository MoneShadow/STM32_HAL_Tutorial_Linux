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

    u1_printf("Delay Starting...\r\n");
    HAL_Delay(5000);
    u1_printf("Delay Stoped...\r\n");

    FreeRTOS_Start();

    while (1) {

    }
}