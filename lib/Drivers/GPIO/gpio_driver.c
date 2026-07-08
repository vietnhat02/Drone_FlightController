#include "stm32f4xx_hal.h"
#include "gpio_driver.h"
/**
 * @brief Enables the clock for the specified GPIO port
 * @param port Pointer to the GPIO port
 */
static void GPIO_EnableClock(GPIO_TypeDef *port) {
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (port == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (port == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    } else if (port == GPIOE) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    } else if (port == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    } else if (port == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    } else if (port == GPIOH) {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
}
/*
    * @brief Configures a GPIO pin with the specified settings
    * @param port Pointer to the GPIO port
    * @param pin GPIO pin number
    * @param mode GPIO mode (input, output, alternate function, analog)
    * @param otype GPIO output type (push-pull, open-drain)
    * @param speed GPIO output speed (low, medium, fast, high)
    * @param pupd GPIO pull-up/pull-down configuration (none, pull-up, pull-down)
    * @param alternate Alternate function number (0-15) for alternate function mode
*/
void GPIO_Config(GPIO_TypeDef *port,
                 uint16_t pin,
                 uint32_t mode,
                 uint32_t otype,
                 uint32_t speed,
                 uint32_t pupd,
                 uint32_t alternate) {
    GPIO_EnableClock(port);
    const uint32_t position = pin * 2U;             //real position in MODER, OSPEEDR, PUPDR registers (2 bits per pin)

    port->MODER &= ~(0x3U << position);             // Clear the mode bits for the specified pin
    port->MODER |= (mode & 0x3U) << position;       // Set the mode bits for the specified pin

    port->OTYPER &= ~(1U << pin);                   // Clear the output type bit for the specified pin
    port->OTYPER |= ((otype & 0x1U) << pin);        // Set the output type bit for the specified pin

    port->OSPEEDR &= ~(0x3U << position);           // Clear the speed bits for the specified pin
    port->OSPEEDR |= (speed & 0x3U) << position;    // Set the speed bits for the specified pin

    port->PUPDR &= ~(0x3U << position);             // Clear the pull-up/pull-down bits for the specified pin
    port->PUPDR |= (pupd & 0x3U) << position;       // Set the pull-up/pull-down bits for the specified pin

    if (mode == GPIODRV_MODE_AF) {
        const uint32_t afrIndex = pin / 8U;         // Determine which AFR register to use (AFR[0] for pins 0-7, AFR[1] for pins 8-15) 
        const uint32_t afrShift = (pin % 8U) * 4U;  // Determine the bit position within the AFR register (4 bits per pin)
        port->AFR[afrIndex] &= ~(0xFU << afrShift); // Clear the alternate function bits for the specified pin
        port->AFR[afrIndex] |= (alternate & 0xFU) << afrShift; // Set the alternate function bits for the specified pin
    }
}
/**
 * @brief Writes a value to the specified GPIO pin
 * @param port Pointer to the GPIO port
 * @param pin GPIO pin number
 * @param value Value to write (0 or 1)
 */
void GPIO_Write(GPIO_TypeDef *port, uint16_t pin, uint8_t value) {
    if (value != 0U) {
        GPIO_Set(port, pin);
    } else {
        GPIO_Reset(port, pin);
    }
}

/**
 * @brief Sets the specified GPIO pin
 * @param port Pointer to the GPIO port
 * @param pin GPIO pin number
 */
void GPIO_Set(GPIO_TypeDef *port, uint16_t pin) {
    port->BSRR = (1U << pin);
}

/**
 * @brief Resets the specified GPIO pin
 * @param port Pointer to the GPIO port
 * @param pin GPIO pin number
 */
void GPIO_Reset(GPIO_TypeDef *port, uint16_t pin) {
    port->BSRR = (1U << (pin + 16U));  
}

/**
 * @brief Toggles the specified GPIO pin
 * @param port Pointer to the GPIO port
 * @param pin GPIO pin number
 */
void GPIO_Toggle(GPIO_TypeDef *port, uint16_t pin) {
    port->ODR ^= (1U << pin);
}

/**
 * @brief Reads the value from the specified GPIO pin
 * @param port Pointer to the GPIO port
 * @param pin GPIO pin number
 * @return Value read from the pin (0 or 1)
 */
uint8_t GPIO_Read(GPIO_TypeDef *port, uint16_t pin) {
    return (uint8_t)((port->IDR >> pin) & 0x1U);
}
