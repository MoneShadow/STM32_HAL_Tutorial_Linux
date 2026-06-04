#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef Tim1_InitStructure;
DMA_HandleTypeDef DMA1InitStructure;

uint16_t dmabuffer[4] = {3999, 5999, 7999, 9999};

uint8_t num = 1;

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
    HAL_TIM_Base_Start_DMA(&Tim1_InitStructure, (uint32_t *)dmabuffer, 4);
    __HAL_TIM_ENABLE_IT(&Tim1_InitStructure, TIM_FLAG_UPDATE);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        DMA1InitStructure.Instance = DMA1_Channel5;
        DMA1InitStructure.Init.Direction = DMA_MEMORY_TO_PERIPH;
        DMA1InitStructure.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        DMA1InitStructure.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        DMA1InitStructure.Init.PeriphInc = DMA_PINC_DISABLE;
        DMA1InitStructure.Init.MemInc = DMA_MINC_ENABLE;
        DMA1InitStructure.Init.Mode = DMA_CIRCULAR;
        DMA1InitStructure.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(&Tim1_InitStructure, hdma[TIM_DMA_ID_UPDATE], DMA1InitStructure);
        HAL_DMA_Init(&DMA1InitStructure);

        HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

        HAL_NVIC_SetPriority(TIM1_UP_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
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
        if (htim->State == HAL_TIM_STATE_READY) {
            u2_prinf("DMA_TC\r\n");
            htim->State = HAL_TIM_STATE_BUSY;
        }
        else {
            u2_prinf("Time: %d\r\n", num++);
        }
    }
}

void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        u2_prinf("DMA_Half\r\n");
        htim->State = HAL_TIM_STATE_BUSY;
    }
}

/* Half 7s TC */