#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

void PWM_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period, uint32_t channel);
void PWM_SetDuty(TIM_TypeDef *tim, uint32_t channel, uint32_t duty);
void PWM_Start(TIM_TypeDef *tim, uint32_t channel);
void PWM_Stop(TIM_TypeDef *tim, uint32_t channel);

#endif
