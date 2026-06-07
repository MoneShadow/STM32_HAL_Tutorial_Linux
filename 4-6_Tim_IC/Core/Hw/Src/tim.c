#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef Tim_InitStructure;
TIM_IC_InitTypeDef Tim_InitIC;
DMA_HandleTypeDef DMA_ST;

uint16_t updatacount = 0;
uint16_t dmabuffer[4] = {6, 7, 8, 9};

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim_InitStructure.Instance = TIM1;
    Tim_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim_InitStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim_InitStructure.Init.RepetitionCounter = rep;
    Tim_InitStructure.Init.Period = arr;
    Tim_InitStructure.Init.Prescaler = psc;
    HAL_TIM_IC_Init(&Tim_InitStructure);
    
    Tim_InitIC.ICPrescaler = TIM_ICPSC_DIV1;
    Tim_InitIC.ICFilter = 0x8;
    Tim_InitIC.ICPolarity = TIM_ICPOLARITY_RISING;
    Tim_InitIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(&Tim_InitStructure, &Tim_InitIC, TIM_CHANNEL_1);

    Tim_InitIC.ICPrescaler = TIM_ICPSC_DIV1;
    Tim_InitIC.ICFilter = 0x8;
    Tim_InitIC.ICPolarity = TIM_ICPOLARITY_FALLING;
    Tim_InitIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    HAL_TIM_IC_ConfigChannel(&Tim_InitStructure, &Tim_InitIC, TIM_CHANNEL_2);

    __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);

    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

    __HAL_TIM_ENABLE_IT(&Tim_InitStructure, TIM_FLAG_UPDATE);

    HAL_TIM_IC_Start_IT(&Tim_InitStructure, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&Tim_InitStructure, TIM_CHANNEL_2);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_Init_ST.Mode = GPIO_MODE_INPUT;
        GPIO_Init_ST.Pin = GPIO_PIN_8 | GPIO_PIN_9;
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

/* 更新中断回调 DMA全完成中断完成回调 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        u2_prinf("Tim1Updata: %d\r\n", ++updatacount);
    }
    else if (htim->Instance == TIM2) {

    }
    else if (htim->Instance == TIM3) {
    
    }
    else if (htim->Instance == TIM4) {
    
    }
}

/* 触发中断回调 */
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

/* DMA半完成中断回调 */
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        u2_prinf("DMA_Half\r\n");
    }
    else if (htim->Instance == TIM2) {
    
    }
    else if (htim->Instance == TIM3) {
    
    }
    else if (htim->Instance == TIM4) {
    
    }
}

uint32_t tmp = 0;
/* 输入捕获中断回调 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_prinf("IC1: %d\r\n", (tmp = HAL_TIM_ReadCapturedValue(&Tim_InitStructure, TIM_CHANNEL_1) + (updatacount * 65535)));
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_prinf("IC2: %d\r\n", HAL_TIM_ReadCapturedValue(&Tim_InitStructure, TIM_CHANNEL_2) + (updatacount * 65535) - tmp);
        }
    }
    else if (htim->Instance == TIM2) {

    }
    else if (htim->Instance == TIM3) {

    }
    else if (htim->Instance == TIM4) {

    }
}