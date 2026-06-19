#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "stm32f1xx_hal_rcc.h"
#include "uart.h"
#include <stdint.h>
#include <time.h>

RTC_HandleTypeDef hrtc;

struct tm timedata;
time_t temp;

void RTC_Init(void) {

    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
    HAL_RTC_Init(&hrtc);

    timedata.tm_year = 2026 - 1900;
    timedata.tm_mon = 1;
    timedata.tm_mday = 1;
    timedata.tm_hour = 0;
    timedata.tm_min = 0;
    timedata.tm_sec = 0;

    temp = mktime(&timedata);

    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);           // 关闭写保护
    WRITE_REG(hrtc.Instance->CNTH, temp >> 16);         // 高位寄存器 第16位有效
    WRITE_REG(hrtc.Instance->CNTL, temp & 0x0000FFFF);  // 低位寄存器 第16位有效
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);            // 开启写保护
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc) {
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
}

struct tm *Get_Localtime(void) {
    temp = READ_REG(hrtc.Instance->CNTH << 16) | READ_REG(hrtc.Instance->CNTL & 0x0000FFFF);
    return localtime(&temp);
}