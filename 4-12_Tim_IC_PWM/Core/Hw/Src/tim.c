#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "tim.h"
#include "uart.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_IC_InitTypeDef Tim_Init_IC_PWM;
TIM_SlaveConfigTypeDef Tim_Init_Slave;

TIM_OC_InitTypeDef Tim_Init_PWM;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    htim1.Instance = TIM1;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim1.Init.Period = arr;
    htim1.Init.Prescaler = psc;
    htim1.Init.RepetitionCounter = rep;
    HAL_TIM_IC_Init(&htim1);
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);

    Tim_Init_IC_PWM.ICFilter = 0x8;
    Tim_Init_IC_PWM.ICPolarity = TIM_ICPOLARITY_RISING;
    Tim_Init_IC_PWM.ICPrescaler = TIM_ICPSC_DIV1;
    Tim_Init_IC_PWM.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(&htim1, &Tim_Init_IC_PWM, TIM_CHANNEL_1);

    Tim_Init_IC_PWM.ICFilter = 0x8;
    Tim_Init_IC_PWM.ICPolarity = TIM_ICPOLARITY_FALLING;
    Tim_Init_IC_PWM.ICPrescaler = TIM_ICPSC_DIV1;
    Tim_Init_IC_PWM.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    HAL_TIM_IC_ConfigChannel(&htim1, &Tim_Init_IC_PWM, TIM_CHANNEL_2);

    Tim_Init_Slave.SlaveMode = TIM_SLAVEMODE_RESET;
    Tim_Init_Slave.InputTrigger = TIM_TS_TI1FP1;
    Tim_Init_Slave.TriggerFilter = 0x8;
    // Tim_Init_Slave.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED; // 外部触发极性 只对ETR通道生效
    Tim_Init_Slave.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
    HAL_TIM_SlaveConfigSynchro(&htim1, &Tim_Init_Slave);

    HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();

        GPIO_Init_ST.Mode = GPIO_MODE_INPUT;
        GPIO_Init_ST.Pin = GPIO_PIN_8;
        GPIO_Init_ST.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);

        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        HAL_NVIC_SetPriority(TIM1_CC_IRQn, 3, 0);
    }
}

void Timer2_Init(uint16_t arr, uint16_t psc) {
    htim2.Instance = TIM2;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim2.Init.Period = arr;
    htim2.Init.Prescaler = psc;
    HAL_TIM_PWM_Init(&htim2);
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

    Tim_Init_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    Tim_Init_PWM.OCMode = TIM_OCMODE_PWM2;          // PWM1 小于输出有效电平 PWM2 小于输出无效电平
    Tim_Init_PWM.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_PWM.Pulse = 600;
    HAL_TIM_PWM_ConfigChannel(&htim2, &Tim_Init_PWM, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM2) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        GPIO_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_ST.Pin = GPIO_PIN_0;
        GPIO_Init_ST.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);
    }
}

uint32_t volatile CCR1;
uint32_t volatile CCR2;
uint8_t volatile state = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            // 需要捕获CCR1周期 CCR2高电平占时 占空比CCR2/CCR1 周期CCR1*计时一个数所用的时间 频率周期的倒数
            CCR1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            CCR2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            state++;
        }
    }
}