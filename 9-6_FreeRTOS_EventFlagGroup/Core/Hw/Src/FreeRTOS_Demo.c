#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_Demo.h"
#include "Key.h"
#include "uart.h"

QueueHandle_t Semaphre_Handler1;
QueueHandle_t Queue_Handler1;
QueueSetHandle_t QueueSet_Handler1;

void task_Start(void * pvParameters);
void task1(void * pvParameters);
void task2(void * pvParameters);

void FreeRTOS_Start(void) {
    xTaskCreate(task_Start, "task_Start", 128, NULL, 0, NULL);
    vTaskStartScheduler();      // 启动调度器
}

/* 用于创建任务等 —— 临界区保证任务同时就绪，高优先级先跑 */
void task_Start(void * pvParameters) {
    taskENTER_CRITICAL();

    Semaphre_Handler1 = xSemaphoreCreateBinary();   // 创建二值信号量
    if (Semaphre_Handler1 != NULL) {
        u1_printf("Semap Created...\r\n");
    }

    Queue_Handler1 = xQueueCreate(1, sizeof(uint8_t));  // 创建队列
    if (Queue_Handler1 != NULL) {
        u1_printf("Queue Created...\r\n");
    }

    QueueSet_Handler1 = xQueueCreateSet(2); // 创建队列集
    if (QueueSet_Handler1 != NULL) {
        u1_printf("QueueSet Created...\r\n");
    }

    xQueueAddToSet(Semaphre_Handler1, QueueSet_Handler1);   // 添加信号量到队列集中
    xQueueAddToSet(Queue_Handler1, QueueSet_Handler1);      // 添加队列到队列集中

    xTaskCreate(task1, "low_task", 128, NULL, 1, NULL);
    xTaskCreate(task2, "midle_task", 128, NULL, 2, NULL);

    taskEXIT_CRITICAL();

    vTaskDelete(NULL);
}

/* task1 按键1按下->写入队列1 按键2按下->释放信号量 */
void task1(void * pvParameters) {
    while (1) {
        uint8_t Key_Num;
        if ((Key_Num = Key_ReadStatus(1)) == 1) {
            if (xQueueSend(Queue_Handler1, &Key_Num, portMAX_DELAY) == pdPASS) {
                u1_printf("QueueSend...\r\n");
            }
        }
        else if (Key_ReadStatus(2) == 2) {
            if (xSemaphoreGive(Semaphre_Handler1) == pdPASS) {
                u1_printf("SemapRelase...\r\n");
            }
        }
    }
}

/* task2 从队列集中获取数据 */
void task2(void * pvParameters) {
    uint8_t QueueBuffer;
    QueueSetMemberHandle_t Member_Handler1;
    while (1) {
        Member_Handler1 = xQueueSelectFromSet(QueueSet_Handler1, portMAX_DELAY);
        if (Member_Handler1 == Queue_Handler1) {
            xQueueReceive(Queue_Handler1, &QueueBuffer, portMAX_DELAY);
            u1_printf("Queue:[%d]\r\n", QueueBuffer);
        }
        else if (Member_Handler1 == Semaphre_Handler1) {
            xSemaphoreTake(Semaphre_Handler1, portMAX_DELAY);
            u1_printf("SemapObtain...\r\n");
        }
    }
}
