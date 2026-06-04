#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"

TIM_HandleTypeDef Tim1_InitStructure;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim1_InitStructure.Instance = TIM1;
    Tim1_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim1_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim1_InitStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim1_InitStructure.Init.RepetitionCounter = rep;
    Tim1_InitStructure.Init.Period = arr;
    Tim1_InitStructure.Init.Prescaler = psc;
    HAL_TIM_Base_Init(&Tim1_InitStructure);
    __HAL_TIM_CLEAR_FLAG(&Tim1_InitStructure, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start(&Tim1_InitStructure);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    else if (htim->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if (htim->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    else if (htim->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    else if (htim->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    else if (htim->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }
    else if (htim->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_DISABLE();
    }
}