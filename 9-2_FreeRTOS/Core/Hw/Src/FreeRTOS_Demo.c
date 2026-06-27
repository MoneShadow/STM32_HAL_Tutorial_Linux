#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"

void static task1(void * arg);
void static task2(void * arg);

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;

void FreeRTOS_Start(void) {
    xTaskCreate(task1, "task1", 128, NULL, 4, &task1_handler);
    xTaskCreate(task2, "task2", 128, NULL, 4, &task2_handler);
    vTaskStartScheduler();
}

void static task1(void * arg) {
    while (1) {
        LED1_Toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void static task2(void * arg) {
    while (1) {
        LED2_Toggle();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}