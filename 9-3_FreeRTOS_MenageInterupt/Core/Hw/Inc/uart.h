#ifndef __UART_H
#define __UART_H

extern UART_HandleTypeDef huart1;

void UART_Init(void);
void u1_printf(char *fmt, ...);

#endif