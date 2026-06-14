#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_adc_ex.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_gpio.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>
#include "adc.h"

ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef hadc1_channel;
DMA_HandleTypeDef hadc1_dma1;

uint16_t adc1_dmabuffer[10];

void ADC_Init(void) {
    // Timer1_Init(200 - 1, 7200 - 1, 0);  // 0.02s/周期
    // Timer3_Init(2000 - 1, 7200 - 1); // 0.2s/周期

    hadc1.Instance = ADC1;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.NbrOfConversion = 10;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.NbrOfDiscConversion = 1;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_EXT_IT11;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    HAL_ADC_Init(&hadc1);

    hadc1_channel.Channel = ADC_CHANNEL_0;
    hadc1_channel.Rank = ADC_REGULAR_RANK_1;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_1;
    hadc1_channel.Rank = ADC_REGULAR_RANK_2;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_2;
    hadc1_channel.Rank = ADC_REGULAR_RANK_3;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_3;
    hadc1_channel.Rank = ADC_REGULAR_RANK_4;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_4;
    hadc1_channel.Rank = ADC_REGULAR_RANK_5;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);
    
    hadc1_channel.Channel = ADC_CHANNEL_5;
    hadc1_channel.Rank = ADC_REGULAR_RANK_6;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_6;
    hadc1_channel.Rank = ADC_REGULAR_RANK_7;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_7;
    hadc1_channel.Rank = ADC_REGULAR_RANK_8;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_8;
    hadc1_channel.Rank = ADC_REGULAR_RANK_9;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    hadc1_channel.Channel = ADC_CHANNEL_9;
    hadc1_channel.Rank = ADC_REGULAR_RANK_10;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_dmabuffer, 10);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;
        hgpioa.Mode = GPIO_MODE_ANALOG;
        hgpioa.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        HAL_GPIO_Init(GPIOA, &hgpioa);

        hgpioa.Mode = GPIO_MODE_ANALOG;
        hgpioa.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        HAL_GPIO_Init(GPIOB, &hgpioa);

        hgpioa.Mode = GPIO_MODE_IT_RISING;
        hgpioa.Pin = GPIO_PIN_11;
        hgpioa.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOA, &hgpioa);

        HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

        hadc1_dma1.Instance = DMA1_Channel1;
        hadc1_dma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hadc1_dma1.Init.MemInc = DMA_MINC_ENABLE;
        hadc1_dma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hadc1_dma1.Init.PeriphInc = DMA_PINC_DISABLE;
        hadc1_dma1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hadc1_dma1.Init.Mode = DMA_CIRCULAR;
        hadc1_dma1.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(hadc, DMA_Handle, hadc1_dma1);
        HAL_DMA_Init(&hadc1_dma1);

        HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    }
}

// uint32_t volatile sum;
uint8_t volatile hadc1_dma1_tx_state = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // hadc1.Instance->CR2 &= ~ADC_CR2_CONT;
        // sum = 0;
        // for (uint16_t i = 0; i < 10; i++) {
        //     sum += adc1_dmabuffer[i];
        // }
        hadc1_dma1_tx_state++;
        // hadc1.Instance->CR2 |= ADC_CR2_CONT;
    }
}

uint8_t volatile hadc1_gpioa_exti11_state = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_11) {
        hadc1_gpioa_exti11_state++;
        // hadc1.Instance->CR2 &= ~ADC_CR2_CONT;
    }
}