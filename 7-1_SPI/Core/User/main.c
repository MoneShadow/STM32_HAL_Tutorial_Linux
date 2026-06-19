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
#include "spi_sf.h"
#include <stdint.h>

uint8_t TxData[8] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xAA, 0xCC, 0xEE};
uint8_t RxData[8];

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();
    SPI1_SF_Init();

    W25Q64_WaitBusy_SF();
    W25Q64_Sector_Erase_SF(0x0000);
    W25Q64_WaitBusy_SF();
    W25Q64_Page_Program_SF(0x0000, TxData, 8);
    W25Q64_Read_Data_SF(0x0000, RxData, 8);

    for (uint8_t i = 0; i < 8; i++) {
        u1_printf("RxData[%d] = %x\r\n", i, RxData[i]);
    }

    u1_printf("\r\n");

    while (1) {
        
    }
}