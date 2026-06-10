#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(1000, 36000 - 1, 0);

    while (1) {
        if (IC1_Status == 1) {
            IC1_Status = 0;
            u2_printf("IC1\r\n");
        }
        if (OC1_Status == 1) {
            OC1_Status = 0;
            u2_printf("OC1\r\n");
        }
	}
}