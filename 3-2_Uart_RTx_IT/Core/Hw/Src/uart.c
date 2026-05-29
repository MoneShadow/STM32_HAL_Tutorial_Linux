#include "stm32f1xx_hal.h"
#include "uart.h"

UART_HandleTypeDef uart1;

void UART_Init(void) {
    /* 串口基地址 8N + 1 */
    uart1.Instance = USART1;
    uart1.Init.BaudRate = 9600;
    uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1.Init.Mode = UART_MODE_TX_RX;
    uart1.Init.OverSampling = 0;
    uart1.Init.Parity = UART_PARITY_NONE;
    uart1.Init.StopBits = UART_STOPBITS_1;
    uart1.Init.WordLength = UART_WORDLENGTH_8B;
    HAL_UART_Init(&uart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    /* 开启时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    /* 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStructure.Pin = GPIO_PIN_10;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pin = GPIO_PIN_9;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}