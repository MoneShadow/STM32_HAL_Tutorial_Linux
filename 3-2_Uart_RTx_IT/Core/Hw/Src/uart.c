#include "stm32f1xx_hal.h"
#include "uart.h"
#include <string.h>

UART_HandleTypeDef uart1;
uint8_t rxdata[128];
uint8_t txdata[128];

void UART_Init(void) {
    /* 串口基地址 8N + 1 */
    uart1.Instance = USART1;
    uart1.Init.BaudRate = 9600;
    uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1.Init.Mode = UART_MODE_TX_RX;
    uart1.Init.OverSampling = UART_OVERSAMPLING_16;
    uart1.Init.Parity = UART_PARITY_NONE;
    uart1.Init.StopBits = UART_STOPBITS_1;
    uart1.Init.WordLength = UART_WORDLENGTH_8B;
    HAL_UART_Init(&uart1);

    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(USART1_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* 调用中断接收函数 */
    /*  中断函数的调用必须在HAL_UART_Init(&uart1);
        这个函数完成之后
        不能在HAL_UART_MspInit(UART_HandleTypeDef *huart)这个函数里面调用
        因为中断函数启用依赖(huart->RxState == HAL_UART_STATE_READY)
        而该状态必须在彻底完成HAL_UART_Init(&uart1);这个函数后才会达成
    */
    HAL_UART_Receive_IT(&uart1, rxdata, 20);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {    
    /* 开启时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    /* 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pin = GPIO_PIN_10;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pin = GPIO_PIN_9;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        memcpy(txdata, rxdata, 20);
        HAL_UART_Transmit_IT(&uart1, txdata, 20);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        HAL_UART_Receive_IT(&uart1, rxdata, 20);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        HAL_UART_Receive_IT(&uart1, rxdata, 20);
    }    
}