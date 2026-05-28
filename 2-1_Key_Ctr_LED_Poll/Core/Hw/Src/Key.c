#include "stm32f1xx_hal.h"
#include "Key.h"

typedef enum {
    press = 1,
    raise = 0
} Key_Status;

Key_Status sta = raise;

void Key_Init(void) {
    /* 开启时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* 配置GPIO */
    GPIO_InitTypeDef GPIOB_InitStructure = {0};
    GPIOB_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIOB_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_11;
    GPIOB_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIOB_InitStructure);
}

uint8_t Key_Val(Key_Mode Mode) {
    if (!Key1_Val && (sta == raise)) {
        for (uint32_t i = 0xFFFFF; i > 1; i--) {
            if (Key1_Val) {
                return 0;
            }
        }
        sta = press;
        if (Mode == Down) {
            return 1;
        }
    }
    if (Key1_Val && (sta == press)) {
        sta = raise;
        if (Mode == Up) {
            return 1;
        }
    }
    return 0;
}