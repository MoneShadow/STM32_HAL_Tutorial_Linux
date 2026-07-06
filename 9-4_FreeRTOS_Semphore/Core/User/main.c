#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "FreeRTOS_Demo.h"
#include "uart.h"
#include "Key.h"
#include "tim.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    LED_Init();
    UART_Init();
    Key_Init();
    Tim_Init();

    // u1_printf("Hello FreeRTOS\r\n");

    FreeRTOS_Start();

    while (1) {
        HAL_Delay(5000);
        u1_printf("Disable interrupt...\r\n");
        portDISABLE_INTERRUPTS();
        // 关中断后不能用 HAL_Delay（依赖 SysTick），改用忙等待 ~3s
        for (volatile uint32_t i = 0; i < 9000000; i++) {
            __NOP();
        }
        u1_printf("Enable interrupts...\r\n");
        portENABLE_INTERRUPTS();
    }
}