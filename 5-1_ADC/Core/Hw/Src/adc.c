#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_gpio.h"
#include "adc.h"

ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef hadc1_channel;

void ADC_Init(void) {
    hadc1.Instance = ADC1;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    HAL_ADC_Init(&hadc1);

    hadc1_channel.Channel = ADC_CHANNEL_0;
    hadc1_channel.Rank = ADC_REGULAR_RANK_1;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    HAL_ADC_Start(&hadc1);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;
        hgpioa.Mode = GPIO_MODE_ANALOG;
        hgpioa.Pin = GPIO_PIN_0;
        HAL_GPIO_Init(GPIOA, &hgpioa);
    }
}