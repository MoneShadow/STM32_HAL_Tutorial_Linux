#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_Demo.h"
#include "Key.h"
#include "uart.h"

TaskHandle_t Task1_Handler;
TaskHandle_t Task2_Handler;

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

    xTaskCreate(task1, "task1", 128, NULL, 1, &Task1_Handler);
    xTaskCreate(task2, "task2", 128, NULL, 2, &Task2_Handler);

    taskEXIT_CRITICAL();

    vTaskDelete(NULL);
}

/* task1 按键1按下->释放任务通知模拟。。。 */
void task1(void * pvParameters) {
    uint8_t Key_Num = 0;
    while (1) {
        Key_Num = Key_Scan();
        switch (Key_Num) {
            case 1:
                xTaskNotify(Task2_Handler, Key_Num, eSetValueWithOverwrite);
                u1_printf("Simulate News mailbox\r\n");
                break;
            case 2:
                xTaskNotify(Task2_Handler, Key_Num, eSetValueWithOverwrite);
                u1_printf("Simulate News mailbox\r\n");
                break;
            case 3:
                xTaskNotify(Task2_Handler, Key_Num, eSetValueWithOverwrite);
                u1_printf("Simulate News mailbox...\r\n");
                break;
            default:
                vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

/* task2 接收任务通知模拟。。。 */
void task2(void * pvParameters) {
    uint32_t Value = 0;
    while (1) {
        xTaskNotifyWait(0, 0xFFFFFFFF, &Value, portMAX_DELAY);
        u1_printf("SemaphoreValue[%d]\r\n", Value);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
