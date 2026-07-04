#include "projdefs.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_Demo.h" 
#include "LED.h"
#include "uart.h"
#include "Key.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// void static task1(void * arg);
// void static task2(void * arg);
static void LEDBulinktask(void * pvParameters);
// static void Task1(void * pvParameters);
// static void Task2_PrintString(void * pvParameters);
// static void Task3_PrintString(void * pvParameters);
static void PrintString(char *Str);
static void Task1_queue_W(void * pvParameters);
static void Task2_queue_W(void * pvParameters);
static void Task3_queue_R(void * pvParameters);
static void Task4_queue_R(void * pvParameters);

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;

QueueHandle_t queue1;
QueueHandle_t queue2;

typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint16_t time;
} LEDBulinkInfo_TypeDef;

LEDBulinkInfo_TypeDef LED1 = {.GPIOx = GPIOA, .GPIO_Pin = GPIO_PIN_1, .time = 200};
LEDBulinkInfo_TypeDef LED2 = {.GPIOx = GPIOA, .GPIO_Pin = GPIO_PIN_2, .time = 1000};

void FreeRTOS_Start(void) {
    /* Task1_quene_W -> 往队列1存入数据       Task2_queue_W -> 往队列2存入数据 */
    /* Task3_quene_R -> 取出队列1存入的数据   Task4_queue_R -> 取出队列2存入的数据 */
    queue1 = xQueueCreate(2, sizeof(uint8_t));
    queue2 = xQueueCreate(1, sizeof(char *));

    xTaskCreate(LEDBulinktask, "LED1", 128, (void *)&LED1, 4, &task1_handler);
    xTaskCreate(LEDBulinktask, "LED2", 128, (void *)&LED2, 4, &task2_handler);
    xTaskCreate(Task1_queue_W, "Task1_quene_W", 256, NULL, 2, NULL);
    xTaskCreate(Task2_queue_W, "Task2_quene_W", 256, NULL, 2, NULL);
    xTaskCreate(Task3_queue_R, "Task3_quene_R", 256, NULL, 3, NULL);
    xTaskCreate(Task4_queue_R, "Task4_quene_R", 256, NULL, 3, NULL);
    // xTaskCreate(Task1, "Task1", 128, NULL, 2, NULL);
    // xTaskCreate(Task2_PrintString, "Task2_PrintString", 128, NULL, 2, NULL);
    // xTaskCreate(Task3_PrintString, "Task3_PrintString", 128, NULL, 3, NULL); 
    vTaskStartScheduler();
}

static void LEDBulinktask(void * pvParameters) {
    LEDBulinkInfo_TypeDef *info = (LEDBulinkInfo_TypeDef *)pvParameters;
    while (1) {
        HAL_GPIO_WritePin(info->GPIOx, info->GPIO_Pin, GPIO_PIN_RESET);
        vTaskDelay((pdMS_TO_TICKS(info->time) / 2));
        HAL_GPIO_WritePin(info->GPIOx, info->GPIO_Pin, GPIO_PIN_SET);
        vTaskDelay((pdMS_TO_TICKS(info->time) / 2));
    }
}

static void Task1_queue_W(void * pvParameters) {
    /* Key1按下 -> 存入数据 “1” | Key2按下 -> 存入数据 “2” */
    uint8_t Key_NUM1 = 1;
    uint8_t Key_NUM2 = 2;
    while (1) {
        if (Key_ReadStatus(1) == 1) {
            vTaskDelay(pdMS_TO_TICKS(50));
            xQueueSend(queue1, &Key_NUM1, portMAX_DELAY);   // 阻塞发送
            u1_printf("Save1\r\n");
        }
        else if (Key_ReadStatus(2) == 1) {
            vTaskDelay(pdMS_TO_TICKS(50));
            xQueueSend(queue1, &Key_NUM2, portMAX_DELAY);   // 阻塞发送
            u1_printf("Save2\r\n");
        }
    }
}

static void Task2_queue_W(void * pvParameters) {
    /* Key3按下 -> 存入数据 “Hello FreeRTOS” */
    char *Str = "Hello FreeRTOS";
    while (1) {
        if (Key_ReadStatus(3)) {
            xQueueSend(queue2, &Str, portMAX_DELAY);        // 发送指针变量的地址
            u1_printf("Save3\r\n");
        }
    }
}

static void Task3_queue_R(void * pvParameters) {
    /* 取出队列1中的数据 */
    uint8_t Buffer1;
    while (1) {
        // PrintString("Read");
        xQueueReceive(queue1, &Buffer1, portMAX_DELAY); // 阻塞读取
        u1_printf("queue1: %d\r\n", Buffer1);
        
    }
}

static void Task4_queue_R(void * pvParameters) {
    /* 取出队列2中的数据 */
    char *Str;
    while (1) {
        // PrintString("HelloFreeRTOS");
        xQueueReceive(queue2, &Str, portMAX_DELAY);      // 阻塞读取指针值
        u1_printf("queue2: %s\r\n", Str);
        
    }
}

// static void Task1(void * pvParameters) {
//     vTaskDelay(pdMS_TO_TICKS(1000));

//     vTaskSuspend(task1_handler);
//     vTaskDelay(pdMS_TO_TICKS(1000));

//     vTaskResume(task1_handler);
//     vTaskDelay(pdMS_TO_TICKS(1000));

//     vTaskDelete(task1_handler);

//     vTaskDelete(NULL);
// }

static void PrintString(char *Str) {
    uint16_t len = strlen(Str);
    uint16_t i = 0;

    while (i < len) {
        taskENTER_CRITICAL();
        //vTaskSuspendAll();

        while ((huart1.Instance->SR & USART_SR_TXE) == 0);  // 等待TXE置位

        USART1->DR = Str[i++];

        taskEXIT_CRITICAL(); // taskENTER_CRITICAL() 比 vTaskSuspendAll() 更轻量 — 只关闭中断（保护 USART 寄存器访问），不阻止任务切换
        //xTaskResumeAll();
    }
}

// static void Task2_PrintString(void * pvParameters) {
//     while (1) {
//         PrintString("Hello_FreeRTOS");
//     }
// }

// static void Task3_PrintString(void * pvParameters) {
//     while (1) {
//         vTaskDelay(pdMS_TO_TICKS(rand() % 100 + 1));
//         PrintString("0123");
//     }
// }

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