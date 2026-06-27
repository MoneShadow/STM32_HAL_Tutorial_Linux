#include "LED.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rcc.h"
#include "FreeRTOS_Demo.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();

    LED_Init();

    FreeRTOS_Start();

    while (1) {
    }
}