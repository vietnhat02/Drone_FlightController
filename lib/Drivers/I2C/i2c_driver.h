#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

/* ============ Status ============ */
typedef enum {
    I2C_OK = 0U,
    I2C_ERROR_TIMEOUT = 1U,
    I2C_ERROR_NACK = 2U,
} I2C_Status_t;

/* ============ Address Direction ============ */
#define I2C_DIR_WRITE           0U
#define I2C_DIR_READ            1U

/* ============ Threshold ============ */
#define I2C_STANDARD_MODE_MAX_HZ    100000U
#define I2C_FAST_MODE_RISE_TIME_NS  300U
#define I2C_STANDARD_MODE_RISE_TIME_NS  1000U

/* ============ Timeout ============ */
#define I2C_TIMEOUT_LOOPS       100000UL

/* ============ Cases of OAR1 ============ */
#define I2C_OAR1_ALWAYS1_POS    14U   /* Bit 14 của OAR1 luôn phải = 1 theo RM0090 */
#define I2C_OAR1_ADDMODE7_POS   15U   /* 0 = 7-bit addressing mode */

/* ============ CCR ============ */
#define I2C_CCR_FS_POS          15U   /* Fast/Standard mode select */
#define I2C_CCR_VALUE_MASK      0x0FFFU

/* ============ Prototype ============ */
void I2C_Init(I2C_TypeDef *i2c, uint32_t clockSpeedHz, uint32_t ownAddress);

I2C_Status_t I2C_WriteBytes(I2C_TypeDef *i2c, uint8_t slaveAddress,
                             const uint8_t *data, uint16_t length);

I2C_Status_t I2C_ReadBytes(I2C_TypeDef *i2c, uint8_t slaveAddress,
                            uint8_t *data, uint16_t length);

I2C_Status_t I2C_WriteRegister(I2C_TypeDef *i2c, uint8_t slaveAddress,
                                uint8_t reg, uint8_t value);

I2C_Status_t I2C_ReadRegister(I2C_TypeDef *i2c, uint8_t slaveAddress,
                               uint8_t reg, uint8_t *value);

I2C_Status_t I2C_ReadRegisters(I2C_TypeDef *i2c, uint8_t slaveAddress,
                                uint8_t reg, uint8_t *data, uint16_t length);

#endif