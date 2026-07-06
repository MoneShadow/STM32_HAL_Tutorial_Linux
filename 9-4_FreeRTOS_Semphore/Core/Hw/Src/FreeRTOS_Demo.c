#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_Demo.h"
#include "Key.h"
#include "uart.h"

QueueHandle_t Semaphre_Handler1;

void task_Start(void * pvParameters);
void task1(void * pvParameters);
void task2(void * pvParameters);
void task3(void * pvParameters);

void FreeRTOS_Start(void) {
    Semaphre_Handler1 = xSemaphoreCreateBinary();   // 创建二值信号量
    if (Semaphre_Handler1 != NULL) {
        u1_printf("Creating Success...\r\n");
    }
    xSemaphoreGive(Semaphre_Handler1);  // 提前释放一次二值信号量
    xTaskCreate(task_Start, "task_Start", 128, NULL, 0, NULL);
    vTaskStartScheduler();      // 启动调度器
}

/* 用于创建任务 —— 临界区保证任务同时就绪，高优先级先跑 */
void task_Start(void * pvParameters) {
    taskENTER_CRITICAL();
    xTaskCreate(task1, "low_task", 128, NULL, 1, NULL);
    xTaskCreate(task2, "midle_task", 128, NULL, 2, NULL);
    xTaskCreate(task3, "high_task", 128, NULL, 3, NULL);
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

/* task1 low_task */
void task1(void * pvParameters) {
    while (1) {
        /* 先获取信号量 */
        u1_printf("low_obtainning...\r\n");
        xSemaphoreTake(Semaphre_Handler1, portMAX_DELAY);
        /* 运行3s */
        u1_printf("low_Running...\r\n");
        HAL_Delay(3000);
        /* 释放信号量 */
        u1_printf("low_releasing...\r\n");
        xSemaphoreGive(Semaphre_Handler1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* task2 midle_task */
void task2(void * pvParameters) {
    while (1) {
        u1_printf("midle_Runnig...\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* task3 high_task */
void task3(void * pvParameters) {
    while (1) {
        /* 先获取信号量 */
        u1_printf("high_obtainning...\r\n");
        xSemaphoreTake(Semaphre_Handler1, portMAX_DELAY);
        /* 运行1s */
        u1_printf("high_Running...\r\n");
        HAL_Delay(1000);
        /* 释放信号量 */
        u1_printf("high_releasing...\r\n");
        xSemaphoreGive(Semaphre_Handler1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}