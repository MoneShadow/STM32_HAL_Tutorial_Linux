#ifndef __TIM_H
#define __TIM_H

extern TIM_HandleTypeDef Tim_InitStructure;

extern volatile uint8_t IC1_Status;
extern volatile uint8_t OC1_Status;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep);

#endif