#include "stm32f1xx_hal.h"
#include "LED.h"

void LED_Init(void) {
    /* 开启时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* 配置GPIO口 */
    GPIO_InitTypeDef GPIOA_InitSturcture = {0};
    GPIOA_InitSturcture.Mode = GPIO_MODE_OUTPUT_PP;
    GPIOA_InitSturcture.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIOA_InitSturcture.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIOA_InitSturcture);
    /* 默认输出高电平 */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
}

void LED1_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
}

void LED2_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
}
