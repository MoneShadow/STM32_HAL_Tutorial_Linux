#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"
#include "adc.h"
#include "i2c.h"
#include "i2c_sf.h"
#include "MPU6050.h"
#include "spi.h"
#include <stdint.h>

uint8_t TxData[4] = {0x55, 0x77, 0x99, 0xAA};
uint8_t RxData[4];

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();
    SPI1_Init();

    W25Q64_WaitBusy();
    W25Q64_Sector_Erase(0x0000);
    W25Q64_WaitBusy();
    W25Q64_Page_Program(0x0000, TxData, 4);
    W25Q64_Read_Data(0x0000, RxData, 4);

    for (uint8_t i = 0; i < 4; i++) {
        u1_printf("RxData[%d] = %x\r\n", i, RxData[i]);
    }

    u1_printf("\r\n");

    while (1) {
        
    }
}