#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_rcc.h"
#include "uart.h"
#include <stdint.h>
#include <time.h>
#include "rtc.h"

RTC_HandleTypeDef hrtc;

struct tm timedata;
struct tm arlarmdata;
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

    arlarmdata.tm_year = 2026 - 1900;
    arlarmdata.tm_mon = 1;
    arlarmdata.tm_mday = 1;
    arlarmdata.tm_hour = 0;
    arlarmdata.tm_min = 0;
    arlarmdata.tm_sec = 5;

    temp = mktime(&timedata);

    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);           // 关闭写保护
    WRITE_REG(hrtc.Instance->CNTH, temp >> 16);         // 高位寄存器 第16位有效
    WRITE_REG(hrtc.Instance->CNTL, temp & 0x0000FFFF);  // 低位寄存器 第16位有效

    temp = mktime(&arlarmdata);
    WRITE_REG(hrtc.Instance->ALRH, temp >> 16);         // 高位寄存器 第16位有效
    WRITE_REG(hrtc.Instance->ALRL, temp & 0x0000FFFF);  // 低位寄存器 第16位有效
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);            // 开启写保护

    HAL_Delay(20);

    /* Clear flag alarm A */
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

    /* Configure the Alarm interrupt */
    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_ALRA);

    /* RTC Alarm Interrupt Configuration: EXTI configuration */
    __HAL_RTC_ALARM_EXTI_ENABLE_IT();

    __HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc) {
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
}

struct tm *Get_Localtime(void) {
    temp = READ_REG(hrtc.Instance->CNTH << 16) | READ_REG(hrtc.Instance->CNTL & 0x0000FFFF);
    return localtime(&temp);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
    u1_printf("Time: 5s\r\n");
}