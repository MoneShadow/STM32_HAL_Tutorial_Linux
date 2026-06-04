#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include "stm32f1xx_hal_uart.h"
#include "ring_buffer.h"

#define Buffer_Size 1024
#define RX_SIZE2 256
#define TX_SIZE2 256

extern __IO uint8_t count;
extern __IO uint8_t status;

extern UART_HandleTypeDef uart1;
extern UART_HandleTypeDef uart2;
extern uint8_t rxbuffer2[], txbuffer2[];
extern uint8_t Buffer[];
extern ring_buffer rb;

void UART2_Init(void);

extern DMA_HandleTypeDef uart2_dmarx_st;
extern DMA_HandleTypeDef uart2_dmatx_st;

#endif