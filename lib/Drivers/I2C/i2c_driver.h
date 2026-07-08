#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

void I2C_Init(I2C_TypeDef *i2c, uint32_t clockSpeedHz, uint32_t ownAddress);
void I2C_WriteBytes(I2C_TypeDef *i2c, uint8_t slaveAddress, const uint8_t *data, uint16_t length);
void I2C_ReadBytes(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t *data, uint16_t length);
void I2C_WriteRegister(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t reg, uint8_t value);
uint8_t I2C_ReadRegister(I2C_TypeDef *i2c, uint8_t slaveAddress, uint8_t reg);

#endif
