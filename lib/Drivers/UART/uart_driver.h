#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

/* ============ Trạng thái trả về ============ */
typedef enum {
    UART_OK = 0U,
    UART_ERROR_TIMEOUT = 1U,
    UART_ERROR_OVERRUN = 2U,
    UART_ERROR_FRAMING = 3U,
    UART_ERROR_NOISE = 4U,
    UART_ERROR_PARITY = 5U,
} UART_Status_t;

/* ============ Timeout (số vòng lặp polling tối đa) ============ */
#define UART_TIMEOUT_LOOPS   100000UL

/* ============ Mask các cờ lỗi trong SR cần kiểm tra khi nhận ============ */
#define UART_SR_ERROR_MASK   (USART_SR_ORE | USART_SR_FE | USART_SR_NE | USART_SR_PE)

/* ============ Prototype ============ */
void UART_Init(USART_TypeDef *uart, uint32_t baudRate);

UART_Status_t UART_WriteByte(USART_TypeDef *uart, uint8_t data);
UART_Status_t UART_Write(USART_TypeDef *uart, const uint8_t *data, uint16_t length);

UART_Status_t UART_ReadByte(USART_TypeDef *uart, uint8_t *data);
UART_Status_t UART_Read(USART_TypeDef *uart, uint8_t *data, uint16_t length);

#endif