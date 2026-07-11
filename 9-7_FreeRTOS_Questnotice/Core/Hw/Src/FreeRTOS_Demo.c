#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_Demo.h"
#include "Key.h"
#include "uart.h"

EventGroupHandle_t EventGroup_Handler1;

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

    EventGroup_Handler1 = xEventGroupCreate();    // 创建事件标志组
    if (EventGroup_Handler1 != NULL) {
        u1_printf("EventGroup Created...\r\n");
    }

    xTaskCreate(task1, "low_task", 128, NULL, 1, NULL);
    xTaskCreate(task2, "midle_task", 128, NULL, 2, NULL);

    taskEXIT_CRITICAL();

    vTaskDelete(NULL);
}

/* task1 按键1按下->事件标志位1置1 按键2按下->事件标志位2置1 */
void task1(void * pvParameters) {
    while (1) {
        uint8_t Key_Num;
        if ((Key_Num = Key_ReadStatus(1)) == 1) {
            xEventGroupSetBits(EventGroup_Handler1, 1 << 0);
            u1_printf("Bit1...\r\n");
        }
        else if (Key_ReadStatus(2) == 2) {
            xEventGroupSetBits(EventGroup_Handler1, 1 << 1);
            u1_printf("Bit2...\r\n");
        }
    }
}

/* task2 根据事件标志组的状态做出相对应的措施 */
void task2(void * pvParameters) {
    uint8_t Event_Value = 0;
    while (1) {
        Event_Value = xEventGroupWaitBits(EventGroup_Handler1,
                                          (1 << 0) | (1 << 1), 
                                          pdTRUE, pdTRUE, 
                                          portMAX_DELAY);
        u1_printf("EventValue[%x]\r\n", Event_Value);
    }
}
