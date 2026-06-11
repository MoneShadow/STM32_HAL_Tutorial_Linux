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
TIM_OC_InitTypeDef Tim_Init_OC_PWM;
TIM_ClearInputConfigTypeDef Tim_Init_IC_BK;

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
    Tim_Init_OC_PWM.Pulse = 200;
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC_PWM, TIM_CHANNEL_1);

    Tim_Init_OC_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    Tim_Init_OC_PWM.OCMode = TIM_OCMODE_PWM1;
    Tim_Init_OC_PWM.OCPolarity = TIM_OCPOLARITY_LOW;
    Tim_Init_OC_PWM.Pulse = 400;
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC_PWM, TIM_CHANNEL_2);

    Tim_Init_OC_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    Tim_Init_OC_PWM.OCMode = TIM_OCMODE_PWM1;
    Tim_Init_OC_PWM.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC_PWM.Pulse = 600;
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC_PWM, TIM_CHANNEL_3);

    Tim_Init_OC_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    Tim_Init_OC_PWM.OCMode = TIM_OCMODE_PWM1;
    Tim_Init_OC_PWM.OCPolarity = TIM_OCPOLARITY_LOW;
    Tim_Init_OC_PWM.Pulse = 800;
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC_PWM, TIM_CHANNEL_4);

    Tim_Init_IC_BK.ClearInputFilter = 0xF;
    Tim_Init_IC_BK.ClearInputSource = TIM_CLEARINPUTSOURCE_ETR;
    Tim_Init_IC_BK.ClearInputState = ENABLE;
    // Tim_Init_IC_BK.ClearInputPolarity = TIM_CLEARINPUTPOLARITY_NONINVERTED; // 高电平触发
    Tim_Init_IC_BK.ClearInputPolarity = TIM_CLEARINPUTPOLARITY_INVERTED;    // 低电平触发
    Tim_Init_IC_BK.ClearInputPrescaler = 0;
    HAL_TIM_ConfigOCrefClear(&Tim_InitStructure, &Tim_Init_IC_BK, TIM_CHANNEL_1);
    HAL_TIM_ConfigOCrefClear(&Tim_InitStructure, &Tim_Init_IC_BK, TIM_CHANNEL_3);

    HAL_TIM_PWM_Start_IT(&Tim_InitStructure, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_IT(&Tim_InitStructure, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start_IT(&Tim_InitStructure, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start_IT(&Tim_InitStructure, TIM_CHANNEL_4);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();

        GPIO_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_ST.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);

        GPIO_Init_ST.Mode = GPIO_MODE_INPUT;
        GPIO_Init_ST.Pin = GPIO_PIN_12;
        GPIO_Init_ST.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);

        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        HAL_NVIC_SetPriority(TIM1_CC_IRQn, 3, 0);
    }
}

/* OC_CompareCallback */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            
        }
    }
}