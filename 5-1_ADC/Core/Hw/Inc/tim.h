#ifndef __TIM_H
#define __TIM_H

extern TIM_HandleTypeDef Tim_InitStructure;

extern uint8_t volatile tim1_cc_state;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep);

#endif