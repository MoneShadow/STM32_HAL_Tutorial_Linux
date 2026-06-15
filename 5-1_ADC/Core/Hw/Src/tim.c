#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_gpio_ex.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef Tim_InitStructure;
TIM_HandleTypeDef htim3;
TIM_OC_InitTypeDef Tim_Init_OC_PWM;
TIM_MasterConfigTypeDef htim3_master;
TIM_IC_InitTypeDef htim3_ic;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim_InitStructure.Instance = TIM1;
    Tim_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim_InitStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim_InitStructure.Init.Period = arr;
    Tim_InitStructure.Init.Prescaler = psc;
    Tim_InitStructure.Init.RepetitionCounter = rep;
    HAL_TIM_PWM_Init(&Tim_InitStructure);
    __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);

    Tim_Init_OC_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    Tim_Init_OC_PWM.OCMode = TIM_OCMODE_PWM1;
    Tim_Init_OC_PWM.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC_PWM.Pulse = 100;
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC_PWM, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start_IT(&Tim_InitStructure, TIM_CHANNEL_1);
}

void Timer3_Init(uint16_t arr, uint16_t psc) {
    htim3.Instance = TIM3;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim3.Init.Period = arr;
    htim3.Init.Prescaler = psc;
    HAL_TIM_IC_Init(&htim3);
    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);

    htim3_ic.ICFilter = 0xF;
    htim3_ic.ICPolarity = TIM_ICPOLARITY_RISING;
    htim3_ic.ICPrescaler = TIM_ICPSC_DIV1;
    htim3_ic.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(&htim3, &htim3_ic, TIM_CHANNEL_1);

    htim3_master.MasterOutputTrigger = TIM_TRGO_OC1;
    HAL_TIMEx_MasterConfigSynchronization(&htim3, &htim3_master);

    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();
        
        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        HAL_NVIC_SetPriority(TIM1_CC_IRQn, 3, 0);
    }
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) {
        // __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();        // 开启AFIO时钟
        __HAL_AFIO_REMAP_TIM3_PARTIAL();    // 重映射TIM3
        __HAL_AFIO_REMAP_SWJ_NOJTAG();      // 恢复PB4引脚
        __HAL_RCC_TIM3_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;
        hgpioa.Mode = GPIO_MODE_INPUT;
        hgpioa.Pin = GPIO_PIN_4;
        hgpioa.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOB, &hgpioa);

        HAL_NVIC_EnableIRQ(TIM3_IRQn);
        HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();

        HAL_NVIC_EnableIRQ(TIM3_IRQn);
        HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
    }
}

uint8_t volatile tim1_cc_state = 0;

/* OC_CompareCallback */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            tim1_cc_state++;
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            
        }
    }
}

uint8_t volatile htim3_update_state = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) {
        htim3_update_state++;
    }
}

uint8_t volatile htim3_cc_state = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            htim3_cc_state++;
        }
    }
}