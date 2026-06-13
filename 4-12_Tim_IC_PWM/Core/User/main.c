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
    Timer1_Init(0xFFFF, 7200 - 1, 0); // 0.1ms(计时一个数)
    Timer2_Init(1000 - 1, 36000 - 1); // 0.5ms(计时一个数)

    while (1) {
        if (state > 0) {
            state--;
            u2_printf("period: %d ms\r\n", CCR1 / 10);
            u2_printf("frq: %d Hz\r\n", 10000 / CCR1);
            u2_printf("dutyfactor: %d %%\r\n", CCR2 * 100 / CCR1);
        }
	}
}