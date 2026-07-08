#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

typedef enum {
    GPIODRV_MODE_INPUT = 0u,
    GPIODRV_MODE_OUTPUT = 1u,
    GPIODRV_MODE_AF = 2u,
    GPIODRV_MODE_ANALOG = 3u,
} GPIO_Mode_t;

typedef enum {
    GPIODRV_OTYPE_PP = 0u,
    GPIODRV_OTYPE_OD = 1u,
} GPIO_OutputType_t;

typedef enum {
    GPIODRV_SPEED_LOW = 0u,
    GPIODRV_SPEED_MEDIUM = 1u,
    GPIODRV_SPEED_FAST = 2u,
    GPIODRV_SPEED_HIGH = 3u,
} GPIO_Speed_t;

typedef enum {
    GPIODRV_PUPD_NONE = 0u,
    GPIODRV_PUPD_UP = 1u,
    GPIODRV_PUPD_DOWN = 2u,
} GPIO_Pull_t;

void GPIO_Config(GPIO_TypeDef *port,
                 uint16_t pin,
                 uint32_t mode,
                 uint32_t otype,
                 uint32_t speed,
                 uint32_t pupd,
                 uint32_t alternate);

void GPIO_Write(GPIO_TypeDef *port, uint16_t pin, uint8_t value);
void GPIO_Set(GPIO_TypeDef *port, uint16_t pin);
void GPIO_Reset(GPIO_TypeDef *port, uint16_t pin);
void GPIO_Toggle(GPIO_TypeDef *port, uint16_t pin);
uint8_t GPIO_Read(GPIO_TypeDef *port, uint16_t pin);

#endif
