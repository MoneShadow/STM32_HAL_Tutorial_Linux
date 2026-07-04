#include "projdefs.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_Demo.h" 
#include "LED.h"
#include "uart.h"
#include "Key.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void FreeRTOS_Start(void) {

}