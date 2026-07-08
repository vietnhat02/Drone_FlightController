#include "stm32f4xx_hal.h"
#include "spi_driver.h"

static void SPI_EnableClock(SPI_TypeDef *spi) {
    if (spi == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
    } else if (spi == SPI2) {
        __HAL_RCC_SPI2_CLK_ENABLE();
    } else if (spi == SPI3) {
        __HAL_RCC_SPI3_CLK_ENABLE();
    }
}

void SPI_Init(SPI_TypeDef *spi, uint32_t baudRateDiv, uint8_t mode, uint8_t lsbFirst) {
    SPI_EnableClock(spi);
    uint32_t cr1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | (baudRateDiv << 3U);

    if (mode == 1U) {
        cr1 |= SPI_CR1_CPHA;
    } else if (mode == 2U) {
        cr1 |= SPI_CR1_CPOL;
    } else if (mode == 3U) {
        cr1 |= (SPI_CR1_CPHA | SPI_CR1_CPOL);
    }

    if (lsbFirst != 0U) {
        cr1 |= SPI_CR1_LSBFIRST;
    }

    spi->CR1 = cr1;
    spi->CR2 = 0U;
    spi->CR1 |= SPI_CR1_SPE;
}

uint8_t SPI_TransmitByte(SPI_TypeDef *spi, uint8_t data) {
    while ((spi->SR & SPI_SR_TXE) == 0U) {
    }

    spi->DR = data;

    while ((spi->SR & SPI_SR_RXNE) == 0U) {
    }

    return (uint8_t)spi->DR;
}

void SPI_Transmit(SPI_TypeDef *spi, const uint8_t *data, uint16_t length) {
    uint16_t index;
    for (index = 0U; index < length; ++index) {
        (void)SPI_TransmitByte(spi, data[index]);
    }
}

void SPI_Receive(SPI_TypeDef *spi, uint8_t *data, uint16_t length) {
    uint16_t index;
    for (index = 0U; index < length; ++index) {
        data[index] = SPI_TransmitByte(spi, 0xFFU);
    }
}
