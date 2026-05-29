#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "LED.h"
#include "Key.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    LED_Init();
    Key_Init_IT(Down);

    while (1) {
        
    }
}