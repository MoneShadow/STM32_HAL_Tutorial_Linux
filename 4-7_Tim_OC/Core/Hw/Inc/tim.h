#ifndef __TIM_H
#define __TIM_H

extern TIM_HandleTypeDef Tim_InitStructure;
extern DMA_HandleTypeDef Tim_DMA;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep);

#endif