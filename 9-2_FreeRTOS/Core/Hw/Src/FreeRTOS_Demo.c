#include "projdefs.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "stm32f1xx_hal_gpio.h"
#include "task.h" 
#include "FreeRTOS_Demo.h" 
#include "LED.h"
#include <stdint.h>

// void static task1(void * arg);
// void static task2(void * arg);
void static LEDBulinktask(void * pvParameters);

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;

typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint16_t time;
} LEDBulinkInfo_TypeDef;

LEDBulinkInfo_TypeDef LED1 = {.GPIOx = GPIOA, .GPIO_Pin = GPIO_PIN_1, .time = 200};
LEDBulinkInfo_TypeDef LED2 = {.GPIOx = GPIOA, .GPIO_Pin = GPIO_PIN_2, .time = 1000};

void FreeRTOS_Start(void) {
    xTaskCreate(LEDBulinktask, "LED1", 128, (void *)&LED1, 4, &task1_handler);
    xTaskCreate(LEDBulinktask, "LED2", 128, (void *)&LED2, 4, &task2_handler);
    vTaskStartScheduler();
}

void static LEDBulinktask(void * pvParameters) {
    LEDBulinkInfo_TypeDef *info = (LEDBulinkInfo_TypeDef *)pvParameters;
    while (1) {
        HAL_GPIO_WritePin(info->GPIOx, info->GPIO_Pin, GPIO_PIN_SET);
        vTaskDelay((pdMS_TO_TICKS(info->time) / 2));
        HAL_GPIO_WritePin(info->GPIOx, info->GPIO_Pin, GPIO_PIN_RESET);
        vTaskDelay((pdMS_TO_TICKS(info->time) / 2));
    }
}

// void static task1(void * arg) {
//     while (1) {
//         LED1_Toggle();
//         vTaskDelay(pdMS_TO_TICKS(500));
//     }
// }

// void static task2(void * arg) {
//     while (1) {
//         LED2_Toggle();
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }