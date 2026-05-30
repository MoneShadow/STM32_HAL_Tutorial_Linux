#ifndef __UART_H
#define __UART_H

extern UART_HandleTypeDef uart1;
extern uint8_t rxdata[128];
extern uint8_t txdata[128];

void UART_Init(void);

#endif