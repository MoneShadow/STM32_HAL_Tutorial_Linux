#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_adc_ex.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_gpio.h"
#include "uart.h"
#include "adc.h"

ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef hadc1_channel;

void ADC_Init(void) {
    hadc1.Instance = ADC1;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    HAL_ADC_Init(&hadc1);

    hadc1_channel.Channel = ADC_CHANNEL_0;
    hadc1_channel.Rank = ADC_REGULAR_RANK_1;
    hadc1_channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &hadc1_channel);

    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_IT(&hadc1);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;
        hgpioa.Mode = GPIO_MODE_ANALOG;
        hgpioa.Pin = GPIO_PIN_0;
        HAL_GPIO_Init(GPIOA, &hgpioa);

        HAL_NVIC_SetPriority(ADC1_2_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // HAL_ADC_PollForConversion(&hadc1, 1000);
        // uint16_t adc_val = HAL_ADC_GetValue(&hadc1);
        // uint16_t mv = adc_val * 3300 / 4095;
        // u2_printf("ADC_IN: %d.%03d v\r\n", mv / 1000, mv % 1000);   // 先放整除部分，后放小数部分，但是本质还是整数运算
    }
}