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
TimerHandle_t Timer1_Handler;
TimerHandle_t Timer2_Handler;

void task_Start(void * pvParameters);
void task1(void * pvParameters);
void timer1_calback( TimerHandle_t pxTimer );
void timer2_calback( TimerHandle_t pxTimer );

void FreeRTOS_Start(void) {
    xTaskCreate(task_Start, "task_Start", 128, NULL, 0, NULL);
    vTaskStartScheduler();      // 启动调度器
}

/* 用于创建任务等 —— 临界区保证任务同时就绪，高优先级先跑 */
void task_Start(void * pvParameters) {
    taskENTER_CRITICAL();

    /* 创建软件定时器 */
    Timer1_Handler = xTimerCreate("Timer1", 1000, pdFALSE, (void *)1, timer1_calback);   // 单次
    Timer2_Handler = xTimerCreate("Timer2", 1000, pdTRUE, (void *)2, timer2_calback);    // 周期循环

    xTaskCreate(task1, "task1", 128, NULL, 1, &Task1_Handler);

    taskEXIT_CRITICAL();

    vTaskDelete(NULL);
}

/* task1 按键1按下->开启软件定时器 按键2按下->关闭软件定时器 */
void task1(void * pvParameters) {
    uint8_t Key_Num = 0;
    while (1) {
        Key_Num = Key_Scan();
        switch (Key_Num) {
            case 1:
                xTimerStart(Timer1_Handler, portMAX_DELAY);
                xTimerStart(Timer2_Handler, portMAX_DELAY);
                break;
            case 2:
                xTimerStop(Timer1_Handler, portMAX_DELAY);
                xTimerStop(Timer2_Handler, portMAX_DELAY);
                break;
            default:
                vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void timer1_calback( TimerHandle_t pxTimer ) {
    static uint8_t NUM1 = 0;
    u1_printf("Timer1: %d\r\n", ++NUM1);
 }

void timer2_calback( TimerHandle_t pxTimer ) {
    static uint8_t NUM2 = 0;
    u1_printf("Timer2: %d\r\n", ++NUM2);
 }