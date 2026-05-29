#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "uart.h"

#define SizeMX 100

uint8_t Buffer[256];

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART_Init();

    while (1) {
        switch (HAL_UART_Receive(&uart1, Buffer, SizeMX, 200)) {
        case HAL_OK :
            HAL_UART_Transmit(&uart1, Buffer, SizeMX, 200);
            break;
        case HAL_TIMEOUT :
            if (uart1.RxXferCount != (SizeMX - 1)) {
                HAL_UART_Transmit(&uart1, Buffer, SizeMX - uart1.RxXferCount - 1, 200);
            }
            else {
                HAL_Delay(1);
            }
            break;
        default:
            HAL_Delay(1);
            break;
        }
        
    }
}