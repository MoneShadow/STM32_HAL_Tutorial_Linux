#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "LED.h"
#include "Key.h"

int main(void) {
    HAL_Init();
    RCC_InitClock();
    LED_Init();
    Key_Init_IT(Up);

    while (1) {
        HAL_Delay(1000);
        __HAL_GPIO_EXTI_GENERATE_SWIT(GPIO_PIN_11);
    }
}