#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

void UART_Init(USART_TypeDef *uart, uint32_t baudRate);
void UART_WriteByte(USART_TypeDef *uart, uint8_t data);
void UART_Write(USART_TypeDef *uart, const uint8_t *data, uint16_t length);
uint8_t UART_ReadByte(USART_TypeDef *uart);
void UART_Read(USART_TypeDef *uart, uint8_t *data, uint16_t length);

#endif
