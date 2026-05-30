#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART_Init();

    while (1) {
        if (hcd1.RXOUTptr != hcd1.RXINptr) {
            UART_TXData(hcd1.RXOUTptr -> start, hcd1.RXOUTptr -> end - hcd1.RXOUTptr -> start + 1);
        }
        if (hcd1.TXOUTptr != hcd1.TXINptr && (hcd1.txstate == 0)) {
            hcd1.txstate = 1;
            HAL_UART_Transmit_IT(&uart1, hcd1.TXOUTptr ->start, hcd1.TXOUTptr -> end - hcd1.TXOUTptr -> start + 1);
            hcd1.TXOUTptr++;
            if (hcd1.TXOUTptr == hcd1.TXENDptr) {
                hcd1.TXOUTptr = &hcd1.TXptrData[0];
            }
        }
	}
}