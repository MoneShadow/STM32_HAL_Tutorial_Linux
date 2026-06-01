#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include "stm32f1xx_hal_uart.h"

#define RX_SIZE		2048
#define TX_SIZE		2048
#define RX_Data_MAX	256

typedef struct {
	uint8_t *start;
	uint8_t *end;
} LCD;

typedef struct {
	uint8_t txstate;
	uint32_t RXCounter;
	uint32_t TXCounter;
	LCD RXptrData[10];
	LCD TXptrData[10];
	LCD *RXINptr;
	LCD *RXOUTptr;
	LCD *RXENDptr;
	LCD *TXINptr;
	LCD *TXOUTptr;
	LCD *TXENDptr;
} HCD;

extern UART_HandleTypeDef uart1;
extern HCD	hcd1;
extern uint8_t rxbuffer[], txbuffer[];

void UART1_Init(void);
void UART_RXptrInit(void);
void UART_TXData(uint8_t *data, uint32_t data_len);

#include "ring_buffer.h"

#define Buffer_Size 1024
#define RX_SIZE2 256
#define TX_SIZE2 256

extern __IO uint8_t count;
extern __IO uint8_t status;

extern UART_HandleTypeDef uart2;
extern uint8_t rxbuffer2[], txbuffer2[];
extern uint8_t Buffer[];
extern ring_buffer rb;

void UART2_Init(void);

extern DMA_HandleTypeDef uart2_dmarx_st;
extern DMA_HandleTypeDef uart2_dmatx_st;

#endif