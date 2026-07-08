#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

void SPI_Init(SPI_TypeDef *spi, uint32_t baudRateDiv, uint8_t mode, uint8_t lsbFirst);
uint8_t SPI_TransmitByte(SPI_TypeDef *spi, uint8_t data);
void SPI_Transmit(SPI_TypeDef *spi, const uint8_t *data, uint16_t length);
void SPI_Receive(SPI_TypeDef *spi, uint8_t *data, uint16_t length);

#endif
