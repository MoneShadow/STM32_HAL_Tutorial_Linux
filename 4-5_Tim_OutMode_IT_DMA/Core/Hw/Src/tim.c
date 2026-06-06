#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef Tim_InitStructure;
TIM_ClockConfigTypeDef Tim_Clock;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim_InitStructure.Instance = TIM1;
    Tim_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim_InitStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim_InitStructure.Init.RepetitionCounter = rep;
    Tim_InitStructure.Init.Period = arr;
    Tim_InitStructure.Init.Prescaler = psc;

    Tim_Clock.ClockSource = TIM_CLOCKSOURCE_TI1;
    //Tim_Clock.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
    Tim_Clock.ClockFilter = 0x3;
    Tim_Clock.ClockPolarity = TIM_CLOCKPOLARITY_RISING;
    
    HAL_TIM_Base_Init(&Tim_InitStructure);
    HAL_TIM_ConfigClockSource(&Tim_InitStructure, &Tim_Clock);
    __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);

    //HAL_Delay(50);
    //__HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_TRIGGER);
    //__HAL_TIM_SET_COUNTER(&Tim_InitStructure, 0);

    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

    HAL_NVIC_SetPriority(TIM1_TRG_COM_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);

    HAL_TIM_Base_Start_IT(&Tim_InitStructure);
    __HAL_TIM_ENABLE_IT(&Tim_InitStructure, TIM_IT_TRIGGER);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_Init_ST.Mode = GPIO_MODE_INPUT;
        GPIO_Init_ST.Pin = GPIO_PIN_8;
        GPIO_Init_ST.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        u2_prinf("TimeUpdata\r\n");
    }
    else if (htim->Instance == TIM2) {
    
    }
    else if (htim->Instance == TIM3) {
    
    }
    else if (htim->Instance == TIM4) {
    
    }
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        u2_prinf("Time: %d\r\n", __HAL_TIM_GET_COUNTER(&Tim_InitStructure));        
    }
    else if (htim->Instance == TIM2) {
    
    }
    else if (htim->Instance == TIM3) {
    
    }
    else if (htim->Instance == TIM4) {
    
    }
}