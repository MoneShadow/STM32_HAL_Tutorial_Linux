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

uint8_t Key_Val(Key_Mode Mode, uint32_t Pdelay, uint32_t Rdelay) {
    if (!Key1_Val && (sta == raise)) {
        for (uint32_t i = Pdelay; i > 1; i--) {
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
        for (uint32_t i = Rdelay; i > 1; i--) {
            if (!Key1_Val) {
                return 0;
            }
        }
        if (Mode == Up) {
            return 1;
        }
    }
    return 0;
}

void Key_Init_IT(Key_Mode Mode) {
	/* 开启时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* 配置GPIO */
    GPIO_InitTypeDef GPIOB_InitStructure = {0};
    GPIOB_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_11;
    GPIOB_InitStructure.Pull = GPIO_PULLUP;
	/* 根据按键模式选择不同的触发条件 */
	if (Mode == Down)
		GPIOB_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	else if (Mode == Up)
		GPIOB_InitStructure.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOB, &GPIOB_InitStructure);

    /* 开启中断 设置中断优先级 */
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 12, 0);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 11, 0);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint32_t i = 0;
	if (GPIO_Pin == GPIO_PIN_1) {
		for ( ; i < 0xFFFFF; i++)
			if (Key1_Val) return;
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
	}
	else if (GPIO_Pin == GPIO_PIN_11) {
		for ( ; i < 0xFFF; i++)
			if (Key11_Val) return;
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
	}
}