#ifndef STM32F4XX_H
#define STM32F4XX_H

#include <stdint.h>

#define __IO volatile

extern uint32_t SystemCoreClock;

typedef struct {
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t OAR1;
    __IO uint32_t OAR2;
    __IO uint32_t DR;
    __IO uint32_t SR1;
    __IO uint32_t SR2;
    __IO uint32_t CCR;
    __IO uint32_t TRISE;
} I2C_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SMCR;
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    __IO uint32_t CCMR2;
    __IO uint32_t CCER;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
    __IO uint32_t RCR;      /* ← nên thêm (Repetition Counter, xen giữa ARR và CCR1 theo thứ tự thật) */
    __IO uint32_t CCR1;
    __IO uint32_t CCR2;
    __IO uint32_t CCR3;
    __IO uint32_t CCR4;
    __IO uint32_t BDTR;     /* ← THÊM DÒNG NÀY - bắt buộc cho TIM1/TIM8 MOE */
} TIM_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t CRCPR;
    __IO uint32_t RXCRCR;
    __IO uint32_t TXCRCR;
    __IO uint32_t I2SCFGR;
    __IO uint32_t I2SPR;
} SPI_TypeDef;

typedef struct {
    __IO uint32_t SR;     /* offset 0x00 - Status Register */
    __IO uint32_t DR;     /* offset 0x04 - Data Register */
    __IO uint32_t BRR;    /* offset 0x08 - Baud Rate Register */
    __IO uint32_t CR1;    /* offset 0x0C - Control Register 1 */
    __IO uint32_t CR2;    /* offset 0x10 - Control Register 2 */
    __IO uint32_t CR3;    /* offset 0x14 - Control Register 3 */
    __IO uint32_t GTPR;   /* offset 0x18 - Guard Time and Prescaler */
} USART_TypeDef;

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t NDTR;
    __IO uint32_t PAR;
    __IO uint32_t M0AR;
    __IO uint32_t M1AR;
    __IO uint32_t FCR;
} DMA_Stream_TypeDef;

typedef struct {
    __IO uint32_t CFGR;
} RCC_TypeDef;

extern RCC_TypeDef *RCC;

enum {
    GPIO_MODE_INPUT = 0u,
    GPIO_MODE_OUTPUT = 1u,
    GPIO_MODE_AF = 2u,
    GPIO_MODE_ANALOG = 3u,
    GPIO_OTYPE_PP = 0u,
    GPIO_OTYPE_OD = 1u,
    GPIO_SPEED_LOW = 0u,
    GPIO_SPEED_MEDIUM = 1u,
    GPIO_SPEED_FAST = 2u,
    GPIO_SPEED_HIGH = 3u,
    GPIO_PUPD_NONE = 0u,
    GPIO_PUPD_UP = 1u,
    GPIO_PUPD_DOWN = 2u
};

enum {
    I2C_CR1_PE = 0x0001U,
    I2C_CR1_START = 0x000100U,
    I2C_CR1_STOP = 0x000200U,
    I2C_CR1_ACK = 0x000400U,
    I2C_SR1_SB = 0x0001U,
    I2C_SR1_ADDR = 0x0002U,
    I2C_SR1_TXE = 0x0004U,
    I2C_SR1_BTF = 0x0008U,
    I2C_SR1_RXNE = 0x0010U,
    I2C_SR1_AF = 0x0000400U
};

enum {
    TIM_CR1_CEN = 0x0001U,
    TIM_CR1_ARPE = 0x0080U,
    TIM_EGR_UG = 0x0001U,
    TIM_CCMR1_OC1M = 0x00007000U,
    TIM_CCMR1_OC1PE = 0x00000800U,
    TIM_CCMR1_OC1M_1 = 0x00001000U,
    TIM_CCMR1_OC1M_2 = 0x00002000U,
    TIM_CCMR1_OC2M = 0x00070000U,
    TIM_CCMR1_OC2PE = 0x00080000U,
    TIM_CCMR1_OC2M_1 = 0x00010000U,
    TIM_CCMR1_OC2M_2 = 0x00020000U,
    TIM_CCMR2_OC3M = 0x00007000U,
    TIM_CCMR2_OC3PE = 0x00000800U,
    TIM_CCMR2_OC3M_1 = 0x00001000U,
    TIM_CCMR2_OC3M_2 = 0x00002000U,
    TIM_CCMR2_OC4M = 0x00070000U,
    TIM_CCMR2_OC4PE = 0x00080000U,
    TIM_CCMR2_OC4M_1 = 0x00010000U,
    TIM_CCMR2_OC4M_2 = 0x00020000U,
    TIM_CCER_CC1E = 0x0001U,
    TIM_CCER_CC2E = 0x0004U,
    TIM_CCER_CC3E = 0x0010U,
    TIM_CCER_CC4E = 0x0040U,
    TIM_BDTR_MOE = 0x00008000U 
};

enum {
    SPI_CR1_CPHA = 0x0001U,
    SPI_CR1_CPOL = 0x0002U,
    SPI_CR1_MSTR = 0x0004U,
    SPI_CR1_SSI = 0x0008U,
    SPI_CR1_SSM = 0x0010U,
    SPI_CR1_SPE = 0x0020U,
    SPI_CR1_LSBFIRST = 0x0080U,
    SPI_SR_TXE = 0x0002U,
    SPI_SR_RXNE = 0x0001U
};

enum {
    USART_CR1_UE = 0x2000U,   /* bit 13 - đúng vị trí thật */
    USART_CR1_TE = 0x0008U,   /* bit 3 */
    USART_CR1_RE = 0x0004U,   /* bit 2 */

    USART_SR_PE   = 0x0001U,  /* bit 0 */
    USART_SR_FE   = 0x0002U,  /* bit 1 */
    USART_SR_NE   = 0x0004U,  /* bit 2 */
    USART_SR_ORE  = 0x0008U,  /* bit 3 */
    USART_SR_RXNE = 0x0020U,  /* bit 5 */
    USART_SR_TC   = 0x0040U,  /* bit 6 */
    USART_SR_TXE  = 0x0080U   /* bit 7 */
};

enum {
    DMA_SxCR_EN = 0x00000001U,
    DMA_SxCR_MINC = 0x00000080U
};

#endif
