#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

void TIMER_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period);
void TIMER_Start(TIM_TypeDef *tim);
void TIMER_Stop(TIM_TypeDef *tim);
void TIMER_DelayUs(TIM_TypeDef *tim, uint32_t us);

#endif
