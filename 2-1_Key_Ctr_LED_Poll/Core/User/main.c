#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "LED.h"
#include "Key.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    LED_Init();
    Key_Init();

    while (1) {
        if (Key_Val(Up)) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
        }
    }
}