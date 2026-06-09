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

uint16_t dmabuffer[4] = {10, 40, 70, 100};

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
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_OC_PWM, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_OC_PWM, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&Tim_InitStructure, &Tim_OC_PWM, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&Tim_InitStructure, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&Tim_InitStructure, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&Tim_InitStructure, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&Tim_InitStructure, TIM_CHANNEL_4);

    HAL_TIM_DMABurst_WriteStart(&Tim_InitStructure,
                                TIM_DMABASE_CCR1,
                                TIM_DMA_CC4,
                                (uint32_t *)dmabuffer,
                                TIM_DMABURSTLENGTH_4TRANSFERS);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIOA_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIOA_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIOA_Init_ST.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
        GPIOA_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIOA_Init_ST);

        DMA1_Init_ST.Instance = DMA1_Channel4;
        DMA1_Init_ST.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        DMA1_Init_ST.Init.MemInc = DMA_MINC_ENABLE;
        DMA1_Init_ST.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        DMA1_Init_ST.Init.PeriphInc = DMA_PINC_DISABLE;
        DMA1_Init_ST.Init.Direction = DMA_MEMORY_TO_PERIPH;
        DMA1_Init_ST.Init.Mode = DMA_NORMAL;
        DMA1_Init_ST.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(&Tim_InitStructure, hdma[TIM_DMA_ID_CC4], DMA1_Init_ST);
        HAL_DMA_Init(&DMA1_Init_ST);

        HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
        HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 3, 0);
    }
    
}

/* PWM中断回调(OC) DMA完成回调 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("PWM_DMA: 1TC\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("PWM_DMA: 2TC\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("PWM_DMA: 3TC\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("PWM_DMA: 4TC\r\n");
            u2_printf("PWM_DMA Burst TC: CCR1=%d CCR2=%d CCR3=%d CCR4=%d\r\n",
                    dmabuffer[0], dmabuffer[1], dmabuffer[2], dmabuffer[3]);
        }
    }
}

/* DMA半完成回调 */
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("PWM_DMA: 1Half\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("PWM_DMA: 2Half\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("PWM_DMA: 3Half\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("PWM_DMA: 4Half\r\n");
        }
    }
}