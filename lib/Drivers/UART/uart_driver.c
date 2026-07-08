#include "stm32f4xx_hal.h"
#include "uart_driver.h"

static void UART_EnableClock(USART_TypeDef *uart) {
    if (uart == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
    } else if (uart == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();
    } else if (uart == USART3) {
        __HAL_RCC_USART3_CLK_ENABLE();
    } else if (uart == UART4) {
        __HAL_RCC_UART4_CLK_ENABLE();
    } else if (uart == UART5) {
        __HAL_RCC_UART5_CLK_ENABLE();
    } else if (uart == USART6) {
        __HAL_RCC_USART6_CLK_ENABLE();
    }
}

void UART_Init(USART_TypeDef *uart, uint32_t baudRate) {
    UART_EnableClock(uart);
    uart->CR1 &= ~USART_CR1_UE;
    uart->BRR = SystemCoreClock / baudRate;
    uart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void UART_WriteByte(USART_TypeDef *uart, uint8_t data) {
    while ((uart->SR & USART_SR_TXE) == 0U) {
    }
    uart->DR = data;
}

void UART_Write(USART_TypeDef *uart, const uint8_t *data, uint16_t length) {
    uint16_t index;
    for (index = 0U; index < length; ++index) {
        UART_WriteByte(uart, data[index]);
    }
}

uint8_t UART_ReadByte(USART_TypeDef *uart) {
    while ((uart->SR & USART_SR_RXNE) == 0U) {
    }
    return (uint8_t)uart->DR;
}

void UART_Read(USART_TypeDef *uart, uint8_t *data, uint16_t length) {
    uint16_t index;
    for (index = 0U; index < length; ++index) {
        data[index] = UART_ReadByte(uart);
    }
}
