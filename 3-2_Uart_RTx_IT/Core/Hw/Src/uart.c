#include "stm32f1xx_hal.h"
#include "uart.h"
#include "myuart.h"
#include "ring_buffer.h"
#include <string.h>

UART_HandleTypeDef uart1;
HCD	hcd1;
uint8_t rxbuffer[RX_SIZE], txbuffer[TX_SIZE];

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
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    UART_RXptrInit();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {    
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

void UART_RXptrInit(void) {
	hcd1.RXINptr = &hcd1.RXptrData[0];
	hcd1.RXOUTptr = &hcd1.RXptrData[0];
	hcd1.RXENDptr =  &hcd1.RXptrData[9];
	hcd1.RXINptr -> start = &rxbuffer[0];
	hcd1.TXINptr = &hcd1.TXptrData[0];
	hcd1.TXOUTptr = &hcd1.TXptrData[0];
	hcd1.TXENDptr =  &hcd1.TXptrData[9];
	hcd1.TXINptr -> start = &txbuffer[0];
	__HAL_UART_ENABLE_IT(&uart1, UART_IT_IDLE);
	HAL_UART_Receive_IT(&uart1, hcd1.RXINptr -> start, RX_Data_MAX);
}

void UART_TXData(uint8_t *data, uint32_t data_len) {
	if (data_len >= (TX_SIZE - hcd1.TXCounter)) {
		hcd1.TXINptr -> start = &txbuffer[0];
		hcd1.TXCounter = 0;
	}
	else {
		hcd1.TXINptr -> start = &txbuffer[hcd1.TXCounter];
	}
	memcpy(hcd1.TXINptr -> start, data, data_len);
	hcd1.TXCounter += data_len;
	hcd1.TXINptr -> end = &txbuffer[hcd1.TXCounter - 1];
	hcd1.RXOUTptr++;
	if (hcd1.RXOUTptr == hcd1.RXENDptr) {
		hcd1.RXOUTptr = &hcd1.RXptrData[0];
	}
	hcd1.TXINptr++;
	if (hcd1.TXINptr == hcd1.TXENDptr) {
		hcd1.TXINptr = &hcd1.TXptrData[0];
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART1) {
		
	}
	if (huart -> Instance == USART2) {
		
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART1) {
		hcd1.txstate = 0;
	}
	if (huart -> Instance == USART2) {
		HAL_UART_Receive_IT(&uart2, rxbuffer2, RX_SIZE2);
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
		hcd1.RXINptr -> end = &rxbuffer[hcd1.RXCounter - 1];
		hcd1.RXINptr++;
		if (hcd1.RXINptr == hcd1.RXENDptr) {
			hcd1.RXINptr = &hcd1.RXptrData[0];
		}
		if ((RX_SIZE - hcd1.RXCounter) < RX_Data_MAX) {
			hcd1.RXINptr -> start = &rxbuffer[0];
			hcd1.RXCounter = 0;
		}
		else {
			hcd1.RXINptr -> start = &rxbuffer[hcd1.RXCounter];
		}
		HAL_UART_Receive_IT(&uart1, hcd1.RXINptr -> start, RX_Data_MAX);
	}
	if (huart -> Instance == USART2) {
		if (R_Free(&rb) >= count) {
			R_WriteByte(&rb, count);
			uint8_t byte;
			for (uint8_t j = 0; count != 0; count--, j++) {
				byte = rxbuffer2[j];
				R_WriteByte(&rb, byte);
			}
			status = 1;
		}
		else {
			HAL_UART_Receive_IT(&uart2, rxbuffer2, RX_SIZE2);
		}
	}
}