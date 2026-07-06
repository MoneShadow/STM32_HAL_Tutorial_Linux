#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_tim.h"
#include "uart.h"
#include "tim.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

void Tim_Init(void) {
    htim1.Instance = TIM1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 7200 - 1;
    htim1.Init.Prescaler = 10000 - 1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim1);

    htim4.Instance = TIM4;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 7200 - 1;
    htim4.Init.Prescaler = 10000 - 1;
    htim4.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim4);

    HAL_TIM_Base_Start_IT(&htim1);
    HAL_TIM_Base_Start_IT(&htim4);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
        HAL_NVIC_SetPriority(TIM1_UP_IRQn, 4, 0);
    }
    else if (htim->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(TIM4_IRQn);
        HAL_NVIC_SetPriority(TIM4_IRQn, 6, 0);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        u1_printf("TIM1:4\r\n");    // 正常来讲中断中不该出现处理带有延时的任务，这里仅作为学习使用，故而不再深究
    }
    else if (htim->Instance == TIM4) {
        u1_printf("TIM4:6\r\n");
    }
}