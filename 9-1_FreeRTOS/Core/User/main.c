#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rcc.h"
#include "LED.h"

static void vLED_Task(void *pvParameters) {
    while (1) {
        LED_Toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    HAL_Init();
    RCC_InitClock();

    LED_Init();

    xTaskCreate(vLED_Task, "LED", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    while (1) {
    }
}