#include "stm32f4xx_hal.h"
#include "timer_driver.h"
/**
 * @brief Enables the clock for the specified timer
 * @param tim Pointer to the timer
 */
static void TIMER_EnableClock(TIM_TypeDef *tim) {
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
/*
 * @brief Initializes the timer
 * @param tim Pointer to the timer
 * @param prescaler Timer prescaler
 * @param period Timer period
 */
void TIMER_Init(TIM_TypeDef *tim, uint32_t prescaler, uint32_t period) {
    TIMER_EnableClock(tim);
    tim->CR1 = 0U;
    tim->PSC = prescaler - 1U;
    tim->ARR = period - 1U;
    tim->EGR = TIM_EGR_UG;
}
/**
 * @brief Starts the timer
 * @param tim Pointer to the timer
 */
void TIMER_Start(TIM_TypeDef *tim) {
    tim->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Stops the timer
 * @param tim Pointer to the timer
 */
void TIMER_Stop(TIM_TypeDef *tim) {
    tim->CR1 &= ~TIM_CR1_CEN;
}

/**
 * @brief Delays for a specified number of microseconds
 * @param tim Pointer to the timer
 * @param us Number of microseconds to delay
 */
void TIMER_DelayUs(TIM_TypeDef *tim, uint32_t us) {
    tim->CNT = 0U;
    tim->CR1 |= TIM_CR1_CEN;

    while (tim->CNT < us) {
    }

    tim->CR1 &= ~TIM_CR1_CEN;
}
