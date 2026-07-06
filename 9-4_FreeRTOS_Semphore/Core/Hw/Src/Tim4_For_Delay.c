#include "stm32f1xx_hal.h"
#include "tim.h"

TIM_HandleTypeDef htim4;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority){
    __HAL_RCC_TIM4_CLK_ENABLE();

    htim4.Instance = TIM4;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Prescaler = 72 - 1;  // 72MHz -> 1MHz
    htim4.Init.Period = 1000 - 1;   // 1000 个计数 -> 1ms
    htim4.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim4);

    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    HAL_NVIC_SetPriority(TIM4_IRQn, TickPriority, 0);
    
    HAL_TIM_Base_Start_IT(&htim4);
    uwTickPrio = TickPriority;

    return HAL_OK;
}

void HAL_SuspendTick(void) {
    /* Disable SysTick Interrupt */
    __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);
}

void HAL_ResumeTick(void) {
    /* Enable SysTick Interrupt */
    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
}