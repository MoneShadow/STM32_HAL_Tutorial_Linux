#ifndef __MYUART_H
#define __MYUART_H

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

void MyUart_Init(void);

#endif
