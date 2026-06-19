#include "stm32f1xx_hal.h"
#include "rcc.h"

void RCC_InitClock(void) {
    RCC_OscInitTypeDef RCC_OscInitStructure = {0};
    RCC_OscInitStructure.HSEState = RCC_HSE_ON;
    RCC_OscInitStructure.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStructure.PLL.PLLMUL = RCC_PLL_MUL9;
    RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStructure);

    RCC_ClkInitTypeDef RCC_ClkInitStructure = {0};
    RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStructure.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_2);

    RCC_PeriphCLKInitTypeDef PeriphCLKInit;
    PeriphCLKInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphCLKInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphCLKInit);
}