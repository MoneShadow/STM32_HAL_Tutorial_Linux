#include "stm32f1xx_hal.h"

uint32_t HCLKFreq;
uint32_t PCLK1Freq;
uint32_t PCLK2req;

int main(void) {
    HAL_Init();
    SystemCoreClockUpdate();
    HCLKFreq = HAL_RCC_GetHCLKFreq();
    PCLK1Freq = HAL_RCC_GetPCLK1Freq();
    PCLK2req = HAL_RCC_GetPCLK2Freq();
}