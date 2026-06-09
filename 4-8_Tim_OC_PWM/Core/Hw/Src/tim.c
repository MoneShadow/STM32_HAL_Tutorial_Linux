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
DMA_HandleTypeDef DMA1_Init_ST;

uint16_t dmabuffer[5] = {200, 400, 600, 800, 1000};

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
    Tim_OC_PWM.Pulse = 0;   // CCR
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_OC_PWM, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start_DMA(&Tim_InitStructure, TIM_CHANNEL_1, (uint32_t *)dmabuffer, 5);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIOA_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIOA_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIOA_Init_ST.Pin = GPIO_PIN_8;
        GPIOA_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIOA_Init_ST);

        DMA1_Init_ST.Instance = DMA1_Channel2;
        DMA1_Init_ST.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        DMA1_Init_ST.Init.MemInc = DMA_MINC_ENABLE;
        DMA1_Init_ST.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        DMA1_Init_ST.Init.PeriphInc = DMA_PINC_DISABLE;
        DMA1_Init_ST.Init.Direction = DMA_MEMORY_TO_PERIPH;
        DMA1_Init_ST.Init.Mode = DMA_CIRCULAR;
        DMA1_Init_ST.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(&Tim_InitStructure, hdma[TIM_DMA_ID_CC1], DMA1_Init_ST);
        HAL_DMA_Init(&DMA1_Init_ST);

        HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
        HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 3, 0);
    }
    
}

/* PWM中断回调(OC) DMA完成回调 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("PWM_DMA: TC\r\n");
        }
    }
}

/* DMA半完成回调 */
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("PWM_DMA: Half\r\n");
        }
    }
}