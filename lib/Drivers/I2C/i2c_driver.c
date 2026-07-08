#include "stm32f4xx_hal.h"
#include "i2c_driver.h"

static void I2C_EnableClock(I2C_TypeDef *i2c) {
    if (i2c == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
    } else if (i2c == I2C2) {
        __HAL_RCC_I2C2_CLK_ENABLE();
    } else if (i2c == I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();
    }
}

static uint32_t I2C_GetPclk1MHz(void) {
    uint32_t pclk1 = SystemCoreClock;
    uint32_t ppre1 = (RCC->CFGR >> 10U) & 0x7U;

    if (ppre1 == 0x4U) {
        pclk1 /= 2U;
    } else if (ppre1 == 0x5U) {
        pclk1 /= 4U;
    } else if (ppre1 == 0x6U) {
        pclk1 /= 8U;
    } else if (ppre1 == 0x7U) {
        pclk1 /= 16U;
    }

    return pclk1 / 1000000U;
}

static void I2C_Start(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t read) {
    i2c->CR1 |= I2C_CR1_START;
    while ((i2c->SR1 & I2C_SR1_SB) == 0U) {
    }

    (void)i2c->SR1;
    i2c->DR = (uint8_t)((slaveAddress << 1U) | read);

    while ((i2c->SR1 & I2C_SR1_ADDR) == 0U) {
    }
    (void)i2c->SR1;
    (void)i2c->SR2;
}

static void I2C_Stop(I2C_TypeDef *i2c) {
    i2c->CR1 |= I2C_CR1_STOP;
}

void I2C_Init(I2C_TypeDef *i2c, uint32_t clockSpeedHz, uint32_t ownAddress) {
    I2C_EnableClock(i2c);
    uint32_t pclk1MHz = I2C_GetPclk1MHz();
    uint32_t ccr = (pclk1MHz * 1000U) / (clockSpeedHz * 2U);

    if (ccr < 4U) {
        ccr = 4U;
    }

    i2c->CR1 = 0U;
    i2c->CR2 = (pclk1MHz & 0x3FU);
    i2c->OAR1 = (1U << 15U) | (ownAddress << 1U);
    i2c->CCR = ccr;
    i2c->TRISE = pclk1MHz + 1U;
    i2c->CR1 = I2C_CR1_PE;
}

void I2C_WriteBytes(I2C_TypeDef *i2c, uint8_t slaveAddress, const uint8_t *data, uint16_t length) {
    uint16_t index;

    I2C_Start(i2c, slaveAddress, 0U);

    for (index = 0U; index < length; ++index) {
        while ((i2c->SR1 & I2C_SR1_TXE) == 0U) {
        }
        i2c->DR = data[index];
    }

    while ((i2c->SR1 & I2C_SR1_BTF) == 0U) {
    }
    I2C_Stop(i2c);
}

void I2C_ReadBytes(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t *data, uint16_t length) {
    uint16_t index;

    I2C_Start(i2c, slaveAddress, 1U);

    if (length == 1U) {
        i2c->CR1 &= ~I2C_CR1_ACK;
    } else {
        i2c->CR1 |= I2C_CR1_ACK;
    }

    for (index = 0U; index < length; ++index) {
        if (index == (length - 1U)) {
            i2c->CR1 &= ~I2C_CR1_ACK;
        }

        while ((i2c->SR1 & I2C_SR1_RXNE) == 0U) {
        }
        data[index] = (uint8_t)i2c->DR;
    }

    I2C_Stop(i2c);
}

void I2C_WriteRegister(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    I2C_WriteBytes(i2c, slaveAddress, buffer, 2U);
}

uint8_t I2C_ReadRegister(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t reg) {
    uint8_t value = 0U;
    I2C_WriteBytes(i2c, slaveAddress, &reg, 1U);
    I2C_ReadBytes(i2c, slaveAddress, &value, 1U);
    return value;
}
