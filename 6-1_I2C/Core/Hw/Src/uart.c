#include "stm32f1xx_hal.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

UART_HandleTypeDef uart1;
uint8_t rxbuffer1[RX_SIZE1], txbuffer1[TX_SIZE1];
uint8_t Buffer1[Buffer_Size];
ring_buffer rb1;
__IO uint8_t count1 = 0;
__IO uint8_t status1 = 0;
DMA_HandleTypeDef uart1_dmarx_st;
DMA_HandleTypeDef uart1_dmatx_st;

UART_HandleTypeDef uart2;
uint8_t rxbuffer2[RX_SIZE2], txbuffer2[TX_SIZE2];
uint8_t Buffer2[Buffer_Size];
ring_buffer rb2;
__IO uint8_t count2 = 0;
__IO uint8_t status2 = 0;
DMA_HandleTypeDef uart2_dmarx_st;
DMA_HandleTypeDef uart2_dmatx_st;

void UART1_Init(void) {
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

    Rbuffer_Init(&rb1, Buffer1, Buffer_Size);

    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* 开启DMA时钟 */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 配置DMA 接收方向 */
    uart1_dmarx_st.Instance = DMA1_Channel5; // 通道地址
    uart1_dmarx_st.Init.Direction = DMA_PERIPH_TO_MEMORY;
    uart1_dmarx_st.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart1_dmarx_st.Init.MemInc = DMA_MINC_ENABLE;
    uart1_dmarx_st.Init.Mode = DMA_NORMAL;
    uart1_dmarx_st.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    uart1_dmarx_st.Init.PeriphInc = DMA_PINC_DISABLE;
    uart1_dmarx_st.Init.Priority = DMA_PRIORITY_MEDIUM;
    __HAL_LINKDMA(&uart1, hdmarx, uart1_dmarx_st);	// 父指针互指
    HAL_DMA_Init(&uart1_dmarx_st);
    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    /* 配置DMA 发送方向 */
    uart1_dmatx_st.Instance = DMA1_Channel4; // 通道地址
    uart1_dmatx_st.Init.Direction = DMA_MEMORY_TO_PERIPH;
    uart1_dmatx_st.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart1_dmatx_st.Init.MemInc = DMA_MINC_ENABLE;
    uart1_dmatx_st.Init.Mode = DMA_NORMAL;
    uart1_dmatx_st.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    uart1_dmatx_st.Init.PeriphInc = DMA_PINC_DISABLE;
    uart1_dmatx_st.Init.Priority = DMA_PRIORITY_MEDIUM;
    __HAL_LINKDMA(&uart1, hdmatx, uart1_dmatx_st);	// 父指针互指
    HAL_DMA_Init(&uart1_dmatx_st);
    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    /* 开启空闲中断 */
    __HAL_UART_ENABLE_IT(&uart1, UART_IT_IDLE);
    /* 开启DMA接收 */
    HAL_UART_Receive_DMA(&uart1, rxbuffer1, RX_SIZE1);
}

void UART2_Init(void) {
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

    Rbuffer_Init(&rb2, Buffer2, Buffer_Size);

    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

	/* 开启DMA时钟 */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* 配置DMA 接收方向 */
	uart2_dmarx_st.Instance = DMA1_Channel6; // 通道地址
	uart2_dmarx_st.Init.Direction = DMA_PERIPH_TO_MEMORY;
	uart2_dmarx_st.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	uart2_dmarx_st.Init.MemInc = DMA_MINC_ENABLE;
	uart2_dmarx_st.Init.Mode = DMA_NORMAL;
	uart2_dmarx_st.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	uart2_dmarx_st.Init.PeriphInc = DMA_PINC_DISABLE;
	uart2_dmarx_st.Init.Priority = DMA_PRIORITY_MEDIUM;
	__HAL_LINKDMA(&uart2, hdmarx, uart2_dmarx_st);	// 父指针互指
	HAL_DMA_Init(&uart2_dmarx_st);
    /* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

	/* 配置DMA 发送方向 */
	uart2_dmatx_st.Instance = DMA1_Channel7; // 通道地址
	uart2_dmatx_st.Init.Direction = DMA_MEMORY_TO_PERIPH;
	uart2_dmatx_st.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	uart2_dmatx_st.Init.MemInc = DMA_MINC_ENABLE;
	uart2_dmatx_st.Init.Mode = DMA_NORMAL;
	uart2_dmatx_st.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	uart2_dmatx_st.Init.PeriphInc = DMA_PINC_DISABLE;
	uart2_dmatx_st.Init.Priority = DMA_PRIORITY_MEDIUM;
	__HAL_LINKDMA(&uart2, hdmatx, uart2_dmatx_st);	// 父指针互指
	HAL_DMA_Init(&uart2_dmatx_st);
	/* 设置优先级 开启中断 */
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    /* 开启空闲中断 */
    __HAL_UART_ENABLE_IT(&uart2, UART_IT_IDLE);
    /* 开启DMA接收 */
	HAL_UART_Receive_DMA(&uart2, rxbuffer2, RX_SIZE2);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {    
		/* 开启时钟 */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
		/* 配置GPIO */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pin = GPIO_PIN_10;	// RX
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pin = GPIO_PIN_9;	// TX
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	if (huart->Instance == USART2) {
		/* 开启时钟 */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART2_CLK_ENABLE();
		/* 配置GPIO */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pin = GPIO_PIN_3; // RX
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pin = GPIO_PIN_2; // TX
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}

void u1_printf(char *fmt, ...) {
    uint8_t tmpbuffer[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf((char *)tmpbuffer, sizeof(tmpbuffer), fmt, ap);
    va_end(ap);
    for (uint16_t i = 0; i < strlen((char *)tmpbuffer); i++) {
        while(!__HAL_UART_GET_FLAG(&uart1, UART_FLAG_TXE));
        uart1.Instance->DR = tmpbuffer[i];
    }
    while(!__HAL_UART_GET_FLAG(&uart1, UART_FLAG_TC));
}

void u2_printf(char *fmt, ...) {
	uint8_t tmpbuffer[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf((char *)tmpbuffer, sizeof(tmpbuffer), fmt, ap);
	va_end(ap);
	for (uint16_t i = 0; i < strlen((char *)tmpbuffer); i++) {
		while(!__HAL_UART_GET_FLAG(&uart2, UART_FLAG_TXE));
		uart2.Instance->DR = tmpbuffer[i];
	}
	while(!__HAL_UART_GET_FLAG(&uart2, UART_FLAG_TC));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART1) {
		
	}
	if (huart -> Instance == USART2) {
		
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART1) {
		if (R_Avilable(&rb1) != 0) {
			status1 = 1;
		}
	}
	if (huart -> Instance == USART2) {
		if (R_Avilable(&rb2) != 0) {
			status2 = 1;
		}
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART1) {
		
	}
	if (huart -> Instance == USART2) {
		
	}
}

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART1) {
		if (R_Free(&rb1) >= count1) {
			R_WriteByte(&rb1, count1);
			uint8_t byte;
			for (uint8_t j = 0; count1 != 0; count1--, j++) {
				byte = rxbuffer1[j];
				R_WriteByte(&rb1, byte);
			}
			status1 = 1;
		}
		HAL_UART_Receive_DMA(&uart1, rxbuffer1, RX_SIZE1);
	}
	if (huart -> Instance == USART2) {
		if (R_Free(&rb2) >= count2) {
			R_WriteByte(&rb2, count2);
			uint8_t byte;
			for (uint8_t j = 0; count2 != 0; count2--, j++) {
				byte = rxbuffer2[j];
				R_WriteByte(&rb2, byte);
			}
			status2 = 1;
		}
		HAL_UART_Receive_DMA(&uart2, rxbuffer2, RX_SIZE2);
	}
}