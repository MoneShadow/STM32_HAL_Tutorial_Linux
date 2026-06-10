#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef Tim_InitStructure;
TIM_OnePulse_InitTypeDef Tim_Init_OP;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim_InitStructure.Instance = TIM1;
    Tim_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim_InitStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim_InitStructure.Init.Period = arr;
    Tim_InitStructure.Init.Prescaler = psc;
    Tim_InitStructure.Init.RepetitionCounter = rep;
    HAL_TIM_OnePulse_Init(&Tim_InitStructure, TIM_OPMODE_SINGLE);
    __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);

    Tim_Init_OP.ICFilter = 0x8;
    Tim_Init_OP.ICPolarity = TIM_ICPOLARITY_RISING;
    Tim_Init_OP.OCPolarity = TIM_OCPOLARITY_LOW;
    Tim_Init_OP.ICSelection = TIM_ICSELECTION_DIRECTTI;
    Tim_Init_OP.OCMode = TIM_OCMODE_PWM1;
    Tim_Init_OP.Pulse = 400;  // CCR
    HAL_TIM_OnePulse_ConfigChannel(&Tim_InitStructure, &Tim_Init_OP, TIM_CHANNEL_2, TIM_CHANNEL_1);

    HAL_TIM_OnePulse_Start_IT(&Tim_InitStructure, TIM_CHANNEL_2);
}

void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIOA_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();

        GPIOA_Init_ST.Mode = GPIO_MODE_INPUT;
        GPIOA_Init_ST.Pin = GPIO_PIN_8;
        GPIOA_Init_ST.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOA, &GPIOA_Init_ST);

        GPIOA_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIOA_Init_ST.Pin = GPIO_PIN_9;
        GPIOA_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIOA_Init_ST);

        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        HAL_NVIC_SetPriority(TIM1_CC_IRQn, 3, 0);
    }
}

volatile uint8_t IC1_Status = 0;
/* IC_CaptureCallback */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            IC1_Status = 1;
        }
    }
}

volatile uint8_t OC1_Status = 0;
/* OC_CompareCallback */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            OC1_Status = 1;
        }
    }
}