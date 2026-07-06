#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

UART_HandleTypeDef huart1;

void UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.OverSampling = 16;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;
        hgpioa.Mode = GPIO_MODE_INPUT;
        hgpioa.Pin = GPIO_PIN_10;   // Rx
        hgpioa.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOA, &hgpioa);

        hgpioa.Mode = GPIO_MODE_AF_PP;
        hgpioa.Pin = GPIO_PIN_9;    // Tx
        hgpioa.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &hgpioa);
    }
}

void u1_printf(char *fmt, ...) {
    uint8_t tmpbuffer[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf((char *)tmpbuffer, sizeof(tmpbuffer), fmt, ap);
    va_end(ap);
    for (uint16_t i = 0; i < strlen((char *)tmpbuffer); i++) {
        while(!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE));
        huart1.Instance->DR = tmpbuffer[i];
    }
    while(!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC));
}