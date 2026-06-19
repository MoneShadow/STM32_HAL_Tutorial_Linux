#ifndef __TIM_H
#define __TIM_H

extern TIM_HandleTypeDef Tim_InitStructure;
extern TIM_HandleTypeDef htim3;

extern uint8_t volatile tim1_cc_state;
extern uint8_t volatile htim3_cc_state;
extern uint8_t volatile htim3_update_state;

void Timer1_Init(uint16_t arr, uint16_t psc, uint8_t rep);
void Timer3_Init(uint16_t arr, uint16_t psc);

#endif