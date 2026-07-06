#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_Demo.h"
#include "Key.h"
#include "uart.h"

QueueHandle_t Semaphre_Count_Handler1;

void task_Start(void * pvParameters);
void task1(void * pvParameters);
void task2(void * pvParameters);

void FreeRTOS_Start(void) {
    Semaphre_Count_Handler1 = xSemaphoreCreateCounting(100, 0);   // 创建计数型信号量
    if (Semaphre_Count_Handler1 != NULL) {
        u1_printf("Creating Success...\r\n");
    }
    xTaskCreate(task_Start, "task_Start", 128, NULL, 0, NULL);
    vTaskStartScheduler();      // 启动调度器
}

/* 用于创建任务 */
void task_Start(void * pvParameters) {
    xTaskCreate(task1, "task1", 128, NULL, 1, NULL);
    xTaskCreate(task2, "task2", 128, NULL, 2, NULL);
    vTaskDelete(NULL);
}

/* task1 检测按键按下->释放计数型信号量(即 +1 ) */
void task1(void * pvParameters) {
    while (1) {
        if (Key_ReadStatus(1)) {
            if (xSemaphoreGive(Semaphre_Count_Handler1) == pdTRUE) {
                u1_printf("Release successful...\r\n");
            }
            else {
                u1_printf("Release failed...\r\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms 轮询间隔，让出 CPU 给空闲任务
    }
}

/* task2 每隔1s 获取一次计数型信号量(即 -1 )并打印出剩余值 */
void task2(void * pvParameters) {
    while (1) {
        if (xSemaphoreTake(Semaphre_Count_Handler1, portMAX_DELAY) == pdTRUE) {
            u1_printf("obtain successful... Surplus:[%d]\r\n", uxSemaphoreGetCount(Semaphre_Count_Handler1));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}