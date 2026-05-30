#ifndef __UART_H
#define __UART_H

#include "string.h"

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

void UART_Init(void);
void UART_RXptrInit(void);
void UART_TXData(uint8_t *data, uint32_t data_len);

#endif