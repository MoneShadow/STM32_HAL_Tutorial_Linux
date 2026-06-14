#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include "stm32f1xx_hal_uart.h"
#include "ring_buffer.h"

#define Buffer_Size 1024
#define RX_SIZE1 256
#define TX_SIZE1 256
#define RX_SIZE2 256
#define TX_SIZE2 256

/* ---- UART1 ---- */
extern __IO uint8_t count1;
extern __IO uint8_t status1;

extern uint8_t rxbuffer1[], txbuffer1[];
extern uint8_t Buffer1[];
extern ring_buffer rb1;

extern DMA_HandleTypeDef uart1_dmarx_st;
extern DMA_HandleTypeDef uart1_dmatx_st;

void UART1_Init(void);
void u1_printf(char *fmt, ...);

/* ---- UART2 ---- */
extern __IO uint8_t count2;
extern __IO uint8_t status2;

extern uint8_t rxbuffer2[], txbuffer2[];
extern uint8_t Buffer2[];
extern ring_buffer rb2;

extern DMA_HandleTypeDef uart2_dmarx_st;
extern DMA_HandleTypeDef uart2_dmatx_st;

void UART2_Init(void);
void u2_printf(char *fmt, ...);

/* ---- 共用 handle ---- */
extern UART_HandleTypeDef uart1;
extern UART_HandleTypeDef uart2;

#endif