#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include <stdint.h>
#include "Key.h"

GPIO_InitTypeDef GPIO_Key_ST;
uint8_t Key1_Status = 0;
uint8_t Key2_Status = 0;
uint8_t Key3_Status = 0;

void Key_Init(void) {
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();   // 禁用 JTAG，保留 SWD (PA13/PA14)

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_Key_ST.Mode = GPIO_MODE_INPUT;
    GPIO_Key_ST.Pin = GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_Key_ST.Pull = GPIO_PULLDOWN;  // 按键按下=高电平, 空闲需下拉
    HAL_GPIO_Init(GPIOB, &GPIO_Key_ST);
}

static uint8_t Key_GetStatus(uint8_t KeyNum) {
    uint16_t Pin = 0;
    uint8_t *StatusPtr = NULL;

    switch (KeyNum) {
        case 1:
            Pin = GPIO_PIN_3;
            StatusPtr = &Key1_Status;
            break;
        case 2:
            Pin = GPIO_PIN_5;
            StatusPtr = &Key2_Status;
            break;
        case 3:
            Pin = GPIO_PIN_7;
            StatusPtr = &Key3_Status;
            break;
        default:
            return 0;
    }

    if (HAL_GPIO_ReadPin(GPIOB, Pin) == GPIO_PIN_SET) {
        for (volatile uint16_t i = 500; i > 1; i--) {
            if (HAL_GPIO_ReadPin(GPIOB, Pin) != GPIO_PIN_SET)
                return 0;
        }
        *StatusPtr = 1;
    }
    return *StatusPtr;
}

uint8_t Key_ReadStatus(uint8_t KeyNum) {
    Key_GetStatus(KeyNum);
    switch (KeyNum) {
        case 1:
            if (Key1_Status == 1) {
                Key1_Status = 0;
                return 1;
            }
            else {
                return Key1_Status;
            }
        case 2:
            if (Key2_Status == 1) {
                Key2_Status = 0;
                return 1;
            }
            else {
                return Key2_Status;
            }
        case 3:
            if (Key3_Status == 1) {
                Key3_Status = 0;
                return 1;
            }
            else {
                return Key3_Status;
            }
        default:
            return 2;
    }
}