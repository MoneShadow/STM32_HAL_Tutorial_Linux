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
#include "rtc.h"
#include <stdint.h>
#include <time.h>

uint8_t Sec = 0xFF;
struct tm *datatime;

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART1_Init();
    RTC_Init();

    while (1) {
        datatime = Get_Localtime();
        if (Sec != datatime->tm_sec) {
            Sec = datatime->tm_sec;
            u1_printf("%d-%d-%d %d:%d:%d\r\n", 
                    datatime->tm_year + 1900, datatime->tm_mon + 1, datatime->tm_mday,
                    datatime->tm_hour, datatime->tm_min, datatime->tm_sec);
        }
    }
}