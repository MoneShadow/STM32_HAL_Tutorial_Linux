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
TIM_OC_InitTypeDef Tim_Init_OC;
DMA_HandleTypeDef Tim_DMA;

uint16_t dmabuffer[6] = {200, 300, 200, 100, 0, 100};
uint16_t dmabuffer4[4] = {900, 700, 500, 300};

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep) {
    Tim_InitStructure.Instance = TIM1;
    Tim_InitStructure.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    Tim_InitStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim_InitStructure.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    /* 
        中央模式1   向下计数触发中断
        中央模式2   向上计数触发中断
        中央模式3   向上向下触发中断
        上到下      先DMA后换向
        下到上      先换向后DMA
    */
    Tim_InitStructure.Init.RepetitionCounter = rep;
    Tim_InitStructure.Init.Period = arr;
    Tim_InitStructure.Init.Prescaler = psc;
    HAL_TIM_OC_Init(&Tim_InitStructure);
    __HAL_TIM_CLEAR_FLAG(&Tim_InitStructure, TIM_FLAG_UPDATE);

    Tim_Init_OC.OCMode = TIM_OCMODE_TOGGLE;
    Tim_Init_OC.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC.Pulse = 200;
    HAL_TIM_OC_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC, TIM_CHANNEL_1);

    Tim_Init_OC.OCMode = TIM_OCMODE_TOGGLE;
    Tim_Init_OC.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC.Pulse = 400;
    HAL_TIM_OC_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC, TIM_CHANNEL_2);

    Tim_Init_OC.OCMode = TIM_OCMODE_TOGGLE;
    Tim_Init_OC.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC.Pulse = 600;
    HAL_TIM_OC_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC, TIM_CHANNEL_3);

    Tim_Init_OC.OCMode = TIM_OCMODE_TOGGLE;
    Tim_Init_OC.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim_Init_OC.Pulse = 800;
    HAL_TIM_OC_ConfigChannel(&Tim_InitStructure, &Tim_Init_OC, TIM_CHANNEL_4);

    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 3, 0);

    HAL_TIM_OC_Start(&Tim_InitStructure, TIM_CHANNEL_1);
    HAL_TIM_OC_Start(&Tim_InitStructure, TIM_CHANNEL_2);
    HAL_TIM_OC_Start(&Tim_InitStructure, TIM_CHANNEL_3);
    HAL_TIM_OC_Start(&Tim_InitStructure, TIM_CHANNEL_4);

    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 3, 0);

    // HAL_TIM_OC_Start_DMA(&Tim_InitStructure, TIM_CHANNEL_1, (uint32_t *)dmabuffer, 6);
    HAL_TIM_DMABurst_WriteStart(&Tim_InitStructure,
                      TIM_DMABASE_CCR1, 
                            TIM_DMA_CC4,
                     (uint32_t *)dmabuffer4,
                                         4);
}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef GPIO_Init_ST;

    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_Init_ST.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_ST.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_Init_ST.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_Init_ST);

        Tim_DMA.Instance = DMA1_Channel4;
        Tim_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        Tim_DMA.Init.MemInc = DMA_MINC_ENABLE;
        Tim_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        Tim_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
        Tim_DMA.Init.Direction = DMA_MEMORY_TO_PERIPH;
        Tim_DMA.Init.Mode = DMA_NORMAL;
        Tim_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_CC4], Tim_DMA);
        HAL_DMA_Init(htim->hdma[TIM_DMA_ID_CC4]);
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
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("DMA_TC: 1\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("DMA_TC: 2\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("DMA_TC: 3\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("DMA_TC: 4\r\n");
        }
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
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("DMA_Half: 1\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("DMA_Half: 2\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("DMA_Half: 3\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("DMA_Half: 4\r\n");
        }
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

/* 输出比较中断回调 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("OC: 1\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("OC: 2\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("OC: 3\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("OC: 4\r\n");
        }
    }
}

/* 输出比较DMA完成回调 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("DMA_TC: 1\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("DMA_TC: 2\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("DMA_TC: 3\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("DMA_TC: 4\r\n");
        }
    }
}

/* 输出比较DMA半完成回调 */
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            u2_printf("DMA_Half: 1\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            u2_printf("DMA_Half: 2\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            u2_printf("DMA_Half: 3\r\n");
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
            u2_printf("DMA_Half: 4\r\n");
        }
    }
}
