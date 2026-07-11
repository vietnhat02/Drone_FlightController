#ifndef STM32F4XX_HAL_H
#define STM32F4XX_HAL_H

#include "stm32f4xx.h"

#define __HAL_RCC_GPIOA_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOB_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOC_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOD_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOE_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOF_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOG_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOH_CLK_ENABLE() ((void)0)
#define __HAL_RCC_I2C1_CLK_ENABLE() ((void)0)
#define __HAL_RCC_I2C2_CLK_ENABLE() ((void)0)
#define __HAL_RCC_I2C3_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM1_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM2_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM3_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM4_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM5_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM8_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM9_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM10_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM11_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM12_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM13_CLK_ENABLE() ((void)0)
#define __HAL_RCC_TIM14_CLK_ENABLE() ((void)0)
#define __HAL_RCC_SPI1_CLK_ENABLE() ((void)0)
#define __HAL_RCC_SPI2_CLK_ENABLE() ((void)0)
#define __HAL_RCC_SPI3_CLK_ENABLE() ((void)0)
#define __HAL_RCC_DMA1_CLK_ENABLE() ((void)0)
#define __HAL_RCC_DMA2_CLK_ENABLE() ((void)0)
#define __HAL_RCC_USART1_CLK_ENABLE() ((void)0)
#define __HAL_RCC_USART2_CLK_ENABLE() ((void)0)
#define __HAL_RCC_USART3_CLK_ENABLE() ((void)0)
#define __HAL_RCC_UART4_CLK_ENABLE() ((void)0)
#define __HAL_RCC_UART5_CLK_ENABLE() ((void)0)
#define __HAL_RCC_USART6_CLK_ENABLE() ((void)0)

#define GPIOA ((GPIO_TypeDef *)0x40020000U)
#define GPIOB ((GPIO_TypeDef *)0x40020400U)
#define GPIOC ((GPIO_TypeDef *)0x40020800U)
#define GPIOD ((GPIO_TypeDef *)0x40020C00U)
#define GPIOE ((GPIO_TypeDef *)0x40021000U)
#define GPIOF ((GPIO_TypeDef *)0x40021400U)
#define GPIOG ((GPIO_TypeDef *)0x40021800U)
#define GPIOH ((GPIO_TypeDef *)0x40021C00U)

#define I2C1 ((I2C_TypeDef *)0x40005400U)
#define I2C2 ((I2C_TypeDef *)0x40005800U)
#define I2C3 ((I2C_TypeDef *)0x40005C00U)

#define TIM1 ((TIM_TypeDef *)0x40010000U)
#define TIM2 ((TIM_TypeDef *)0x40000000U)
#define TIM3 ((TIM_TypeDef *)0x40000400U)
#define TIM4 ((TIM_TypeDef *)0x40000800U)
#define TIM5 ((TIM_TypeDef *)0x40000C00U)
#define TIM8 ((TIM_TypeDef *)0x40010400U)
#define TIM9 ((TIM_TypeDef *)0x40014000U)
#define TIM10 ((TIM_TypeDef *)0x40014400U)
#define TIM11 ((TIM_TypeDef *)0x40014800U)
#define TIM12 ((TIM_TypeDef *)0x40001800U)
#define TIM13 ((TIM_TypeDef *)0x40001C00U)
#define TIM14 ((TIM_TypeDef *)0x40002000U)

#define SPI1 ((SPI_TypeDef *)0x40013000U)
#define SPI2 ((SPI_TypeDef *)0x40003800U)
#define SPI3 ((SPI_TypeDef *)0x40003C00U)

#define USART1 ((USART_TypeDef *)0x40011000U)
#define USART2 ((USART_TypeDef *)0x40004400U)
#define USART3 ((USART_TypeDef *)0x40004800U)
#define UART4 ((USART_TypeDef *)0x40004C00U)
#define UART5 ((USART_TypeDef *)0x40005000U)
#define USART6 ((USART_TypeDef *)0x40011400U)

#define DMA1_BASE 0x40026000UL
#define DMA2_BASE 0x40026400UL

typedef struct {
    volatile uint32_t LIFCR;
    volatile uint32_t HIFCR;
} DMA_TypeDef;

#endif
