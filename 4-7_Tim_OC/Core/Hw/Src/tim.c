#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef Tim_InitStructure;
TIM_OC_InitTypeDef Tim_Init_OC;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim_InitStructure.Instance = TIM1;
    Tim_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim_InitStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim_InitStructure.Init.RepetitionCounter = rep;
    Tim_InitStructure.Init.Period = arr;
    Tim_InitStructure.Init.Prescaler = psc;
    HAL_TIM_OC_Init(&Tim_InitStructure);
    __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);

    Tim_Init_OC.OCMode = TIM_OCMODE_TOGGLE;
    Tim_Init_OC.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC.Pulse = 100;
    HAL_TIM_OC_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC, TIM_CHANNEL_1);

    HAL_TIM_OC_Start(&Tim_InitStructure, TIM_CHANNEL_1);
}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_ST.Pin = GPIO_PIN_8;
        GPIO_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
}

/* 更新中断回调 DMA完成中断回调 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {

    }
}

/* 触发中断回调 */
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        
    }
}

/* DMA半完成中断回调(溢出更新) */
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        
    }
}

uint32_t tmp = 0;
/* 输入捕获中断回调 DMA完成中断回调 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            
        }
    }
}

/* DMA半完成中断回调(输入捕获) */
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {

        }
    }
}