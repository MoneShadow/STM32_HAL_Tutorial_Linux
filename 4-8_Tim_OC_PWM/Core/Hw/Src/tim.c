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
TIM_OC_InitTypeDef Tim_OC_PWM;
TIM_BreakDeadTimeConfigTypeDef Tim_Init_BK;

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

    Tim_OC_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    Tim_OC_PWM.OCMode = TIM_OCMODE_PWM1;
    Tim_OC_PWM.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_OC_PWM.Pulse = 90;   // CCR
    Tim_OC_PWM.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    Tim_OC_PWM.OCIdleState = TIM_OCIDLESTATE_RESET;
    Tim_OC_PWM.OCNIdleState = TIM_OCNIDLESTATE_SET;
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_OC_PWM, TIM_CHANNEL_1);

    Tim_Init_BK.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    Tim_Init_BK.DeadTime = 0xFF;
    Tim_Init_BK.LockLevel = TIM_LOCKLEVEL_OFF;
    Tim_Init_BK.BreakFilter = 0x8;
    Tim_Init_BK.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    Tim_Init_BK.BreakState = TIM_BREAK_ENABLE;
    Tim_Init_BK.OffStateIDLEMode = TIM_OSSI_ENABLE;
    Tim_Init_BK.OffStateRunMode = TIM_OSSR_ENABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&Tim_InitStructure, &Tim_Init_BK);

    HAL_TIM_PWM_Start(&Tim_InitStructure, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&Tim_InitStructure, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIOA_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();

        GPIOA_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIOA_Init_ST.Pin = GPIO_PIN_8;
        GPIOA_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIOA_Init_ST);

        GPIOA_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIOA_Init_ST.Pin = GPIO_PIN_13;
        GPIOA_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIOA_Init_ST);

        GPIOA_Init_ST.Mode = GPIO_MODE_INPUT;
        GPIOA_Init_ST.Pin = GPIO_PIN_12;
        GPIOA_Init_ST.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOB, &GPIOA_Init_ST);
    }
    
}

/* PWM中断回调(OC) DMA完成回调 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("PWM: 1\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("PWM: 2\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("PWM: 3\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("PWM: 4\r\n");
        }
    }
}