#include "stm32f1xx_hal.h"
#include "rcc.h"

void RCC_InitClock(void) {
    RCC_OscInitTypeDef RCC_OscInitStructure = {0};
    RCC_OscInitStructure.HSIState = RCC_HSI_ON;
    RCC_OscInitStructure.HSICalibrationValue = 1;
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStructure.PLL.PLLMUL = RCC_PLL_MUL16;
    RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStructure);

    RCC_ClkInitTypeDef RCC_ClkInitStructure = {0};
    RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStructure.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_2);
}