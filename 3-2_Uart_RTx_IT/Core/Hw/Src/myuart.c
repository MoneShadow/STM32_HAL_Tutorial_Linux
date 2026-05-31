#include "stm32f1xx_hal.h"
#include "myuart.h"

UART_HandleTypeDef uart2;
uint8_t rxbuffer2[RX_SIZE2], txbuffer2[TX_SIZE2];
uint8_t Buffer[Buffer_Size];
ring_buffer rb;

__IO uint8_t count = 0;
__IO uint8_t status = 0;

void MyUart_Init(void) {
    /* 串口基地址 8N + 1 */
    uart2.Instance = USART2;
    uart2.Init.BaudRate = 9600;
    uart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart2.Init.Mode = UART_MODE_TX_RX;
    uart2.Init.OverSampling = UART_OVERSAMPLING_16;
    uart2.Init.Parity = UART_PARITY_NONE;
    uart2.Init.StopBits = UART_STOPBITS_1;
    uart2.Init.WordLength = UART_WORDLENGTH_8B;
    HAL_UART_Init(&uart2);

    Rbuffer_Init(&rb, Buffer, Buffer_Size);

    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    /* 开启空闲中断 */
    __HAL_UART_ENABLE_IT(&uart2, UART_IT_IDLE);
    /* 开启接收中断 */
    HAL_UART_Receive_IT(&uart2, rxbuffer2, RX_SIZE2);
}
