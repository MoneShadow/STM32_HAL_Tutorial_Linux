#ifndef __TIM_H
#define __TIM_H

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef Tim_Init_DMA_IC1; 
extern DMA_HandleTypeDef Tim_Init_DMA_IC2;

extern uint32_t volatile CCR1;
extern uint32_t volatile CCR2;
extern uint8_t volatile state;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep);
void Timer2_Init(uint16_t arr, uint16_t psc);

#endif