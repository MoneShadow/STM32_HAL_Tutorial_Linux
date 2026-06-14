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
    Timer1_Init(200 - 1, 7200 - 1, 0); // 0.2s/周期

    hadc1.Instance = ADC1;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    HAL_ADC_Init(&hadc1);

    hadc1_channel.Channel = ADC_CHANNEL_0;
    hadc1_channel.Rank = ADC_REGULAR_RANK_1;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_dmabuffer, 10);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;
        hgpioa.Mode = GPIO_MODE_ANALOG;
        hgpioa.Pin = GPIO_PIN_0;
        HAL_GPIO_Init(GPIOA, &hgpioa);

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

uint32_t volatile sum;
uint8_t volatile hadc1_dma1_tx_state = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        sum = 0;
        for (uint16_t i = 0; i < 10; i++) {
            sum += adc1_dmabuffer[i];
        }
        hadc1_dma1_tx_state++;
    }
}