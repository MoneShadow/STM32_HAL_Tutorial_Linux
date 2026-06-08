#include "stm32f1xx_hal.h"
#include "rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "uart.h"
#include "tim.h"
#include <stdint.h>

int main(void) {
    HAL_Init();
    RCC_InitClock();
    UART2_Init();
    Timer1_Init(1000 - 1, 36000 - 1, 0); // 周期 1s 频率 1Hz
    /*
        定时器时钟频率 CLK = 72MHz
        计数器的计时频率 TIMCLK = CLK/PSC = 72 000 000 / 36 000 = 2 000 Hz
        计数器的计时周期 1 / TIMCLK = 1 / 2 000 Hz = 0.0005 s
        计数器的更新(溢出)周期 ARR * 1/TIMCLK = 1 000 * 0.0005s = 0.5s
        计数器的更新(溢出)频率 周期取倒数
        OC引脚电平变化周期 2 * 计数器的更新(溢出)周期 = 2 * 0.5s = 1s
        OC引脚电平变化频率 上面取倒数 = 1s/1 = 1Hz
        占空比 高电平时间/周期 * 100% 因为这里选用中央计数模式 所以占空比跟CCR有关
    */

    while (1) {
        
	}
}