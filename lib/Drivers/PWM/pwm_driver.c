#include "stm32f4xx_hal.h"
#include "pwm_driver.h"
/**
 * @brief Enables the clock for the specified PWM timer
 * @param tim Pointer to the timer
 */
static void PWM_EnableClock(TIM_TypeDef *tim) {
    if (tim == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    } else if (tim == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    } else if (tim == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    } else if (tim == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    } else if (tim == TIM5) {
        __HAL_RCC_TIM5_CLK_ENABLE();
    } else if (tim == TIM8) {
        __HAL_RCC_TIM8_CLK_ENABLE();
    } else if (tim == TIM9) {
        __HAL_RCC_TIM9_CLK_ENABLE();
    } else if (tim == TIM10) {
        __HAL_RCC_TIM10_CLK_ENABLE();
    } else if (tim == TIM11) {
        __HAL_RCC_TIM11_CLK_ENABLE();
    } else if (tim == TIM12) {
        __HAL_RCC_TIM12_CLK_ENABLE();
    } else if (tim == TIM13) {
        __HAL_RCC_TIM13_CLK_ENABLE();
    } else if (tim == TIM14) {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }
}
/**
 * @brief Gets the bit mask for the specified PWM channel
 * @param channel PWM channel number
 * @return Bit mask for the specified channel
 */
static uint32_t PWM_ChannelBit(uint32_t channel) {
    switch (channel) {
        case 1U:
            return TIM_CCER_CC1E;
        case 2U:
            return TIM_CCER_CC2E;
        case 3U:
            return TIM_CCER_CC3E;
        case 4U:
            return TIM_CCER_CC4E;
        default:
            return 0U;
    }
}
/**
 * @brief Initializes the PWM timer
 * @param tim Pointer to the timer
 * @param prescaler Timer prescaler
 * @param period Timer period
 * @param channel PWM channel number
 */
void PWM_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period, uint32_t channel) {
    PWM_EnableClock(tim);
    tim->PSC = prescaler - 1U;
    tim->ARR = period - 1U;
    tim->EGR = TIM_EGR_UG;

    switch (channel) {
        case 1U:
            tim->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE);      // Clear OC1M and OC1PE bits
            tim->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE); // Set OC1M to PWM mode 1 and enable preload
            break;
        case 2U:
            tim->CCMR1 &= ~(TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE);
            tim->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE);
            break;
        case 3U:
            tim->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);
            tim->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE);
            break;
        case 4U:
            tim->CCMR2 &= ~(TIM_CCMR2_OC4M | TIM_CCMR2_OC4PE);
            tim->CCMR2 |= (TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE);
            break;
        default:
            break;
    }

    tim->CCER |= PWM_ChannelBit(channel);
    tim->CR1 = TIM_CR1_ARPE;
}

/**
 * @brief Sets the duty cycle for the specified PWM channel
 * @param tim Pointer to the timer
 * @param channel PWM channel number
 * @param duty Duty cycle value (0 to ARR)
 */
void PWM_SetDuty(TIM_TypeDef *tim, uint32_t channel, uint32_t duty) {
    uint32_t maxDuty = tim->ARR;
    if (duty > maxDuty) {
        duty = maxDuty;
    }
    switch (channel) {
        case 1U:
            tim->CCR1 = duty;
            break;
        case 2U:
            tim->CCR2 = duty;
            break;
        case 3U:
            tim->CCR3 = duty;
            break;
        case 4U:
            tim->CCR4 = duty;
            break;
        default:
            break;
    }
}

/**
 * @brief Checks if the timer is an advanced control timer (TIM1 or TIM8)
 * @param tim Pointer to the timer
 * @return 1 if advanced timer, 0 otherwise
 */
static uint8_t PWM_IsAdvancedTimer(TIM_TypeDef *tim) {
    return (tim == TIM1) || (tim == TIM8);
}

/**
 * @brief Starts the PWM output on the specified channel
 * @param tim Pointer to the timer
 * @param channel PWM channel number
 */
void PWM_Start(TIM_TypeDef *tim, uint32_t channel) {
    tim->CR1 |= TIM_CR1_CEN;
    tim->CCER |= PWM_ChannelBit(channel);

    if (PWM_IsAdvancedTimer(tim)) {
        tim->BDTR |= TIM_BDTR_MOE;
    }
}

/**
 * @brief Stops the PWM output on the specified channel
 * @param tim Pointer to the timer
 * @param channel PWM channel number
 */
void PWM_Stop(TIM_TypeDef *tim, uint32_t channel) {
    tim->CCER &= ~PWM_ChannelBit(channel);
}
