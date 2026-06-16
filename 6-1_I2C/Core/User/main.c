#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include "adc.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();
    ADC_Init();

    while (1) {
        if (hadc1_dma1_tx_state > 0) {
            hadc1_dma1_tx_state--;
            // u1_printf("%d\r\n", hadc1_Dis_count);
            u1_printf("ADC1_IN%d: %d.%03d v\r\n", (hadc1_Dis_count % 5) * 2, ((adc1_dmabuffer[0] * 3300) / 4095) / 1000, ((adc1_dmabuffer[0] * 3300) / 4095) % 1000);
            u1_printf("ADC1_IN%d: %d.%03d v\r\n\r\n", ((hadc1_Dis_count % 5) * 2) + 1, ((adc1_dmabuffer[1] * 3300) / 4095) / 1000, ((adc1_dmabuffer[1] * 3300) / 4095) % 1000);
            hadc1_Dis_count++;
        }
        if (tim1_cc_state > 0) {
            tim1_cc_state--;
            u1_printf("Transfer!!!\r\n");
        }
        if (htim3_update_state > 0) {
            htim3_update_state--;
            u1_printf("Tim3: Update!!!\r\n");
        }
        if (htim3_cc_state > 0) {
            htim3_cc_state--;
            u1_printf("Tim3: IC1!!!\r\n");
        }
        if (hadc1_gpioa_exti11_state > 0) {
            hadc1_gpioa_exti11_state--;
            u1_printf("EXTI11!!!\r\n");
        }
    }
}