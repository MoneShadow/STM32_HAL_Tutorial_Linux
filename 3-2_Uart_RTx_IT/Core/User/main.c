#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "myuart.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    MyUart_Init();

    while (1) {
        if (status == 1) {
            status = 0;
            uint8_t vallen = rb.buffer[rb.tail], j = 0, byte = 0;
            rb.tail = NEXT_IDX(rb.tail, (&rb));
            while (vallen-- != 0) {
                R_ReadByte(&rb, &byte);
                txbuffer2[j++] = byte;
            }
            HAL_UART_Transmit_IT(&uart2, txbuffer2, j);
        }
	}
}