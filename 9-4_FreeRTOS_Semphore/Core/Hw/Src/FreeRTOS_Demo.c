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

void FreeRTOS_Start(void) {
    Semaphre_Handler1 = xSemaphoreCreateBinary();   // 创建二值信号量
    xTaskCreate(task_Start, "task_Start", 128, NULL, 0, NULL);
    vTaskStartScheduler();      // 启动调度器
}

/* 用于创建任务 */
void task_Start(void * pvParameters) {
    xTaskCreate(task1, "task1", 128, NULL, 1, NULL);
    xTaskCreate(task2, "task2", 128, NULL, 2, NULL);
    vTaskDelete(NULL);
}

/* task1 检测按键按下->释放信号量 */
void task1(void * pvParameters) {
    while (1) {
        if (Key_ReadStatus(1)) {
            if (xSemaphoreGive(Semaphre_Handler1) == pdPASS) {
                u1_printf("Release successful...\r\n");
            }
            else {
                u1_printf("Release failed...\r\n");
            }
        }
        vTaskDelay(10);  // 10ms 轮询间隔，让出 CPU 给空闲任务
    }
}

/* task2 获取信号量 */
void task2(void * pvParameters) {
    while (1) {
        if (xSemaphoreTake(Semaphre_Handler1, portMAX_DELAY) == pdPASS) {
            u1_printf("obtain successful...\r\n");
        }
    }
}