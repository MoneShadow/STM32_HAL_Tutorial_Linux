#ifndef __ADC_H
#define __ADC_H

extern ADC_HandleTypeDef hadc1;

// extern uint32_t volatile sum;
extern uint8_t volatile hadc1_dma1_tx_state;
extern uint8_t volatile hadc1_gpioa_exti11_state;
extern uint16_t adc1_dmabuffer[];

void ADC_Init(void);

#endif
