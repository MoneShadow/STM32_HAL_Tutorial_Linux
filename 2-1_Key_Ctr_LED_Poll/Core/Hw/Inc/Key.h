#ifndef __KEY_H
#define __KEY_H

#define Key1_Val HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define Key2_Val HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)

typedef enum {
    Down = 1,
    Up = 0
} Key_Mode;

void Key_Init(void);
uint8_t Key_Val(Key_Mode Mode, uint32_t Pdelay, uint32_t Rdelay);

#endif