#include "hmc5883l_driver.h"

static uint8_t hmc_slave_addr = HMC5883L_I2C_ADDR_DEFAULT;

HMC_Status_t HMC5883L_CheckId(I2C_TypeDef *i2c) {
    uint8_t idA = 0U, idB = 0U, idC = 0U;

    if (I2C_ReadRegister(i2c, hmc_slave_addr, HMC5883L_REG_ID_A, &idA) != I2C_OK) {
        return HMC_ERROR_READ;
    }
    if (I2C_ReadRegister(i2c, hmc_slave_addr, HMC5883L_REG_ID_B, &idB) != I2C_OK) {
        return HMC_ERROR_READ;
    }
    if (I2C_ReadRegister(i2c, hmc_slave_addr, HMC5883L_REG_ID_C, &idC) != I2C_OK) {
        return HMC_ERROR_READ;
    }

    if ((idA != HMC5883L_ID_A_EXPECTED) ||
        (idB != HMC5883L_ID_B_EXPECTED) ||
        (idC != HMC5883L_ID_C_EXPECTED)) {
        return HMC_ERROR_ID;
    }

    return HMC_OK;
}

HMC_Status_t HMC5883L_Init(I2C_TypeDef *i2c, uint8_t slaveAddress) {
    hmc_slave_addr = slaveAddress;

    /* Config A: 8 mẫu trung bình, ODR 15Hz, chế độ đo bình thường */
    if (I2C_WriteRegister(i2c, hmc_slave_addr, HMC5883L_REG_CONFIG_A,
                           HMC5883L_CRA_AVG_8 | HMC5883L_CRA_ODR_15HZ |
                           HMC5883L_CRA_MEAS_NORMAL) != I2C_OK) {
        return HMC_ERROR_WRITE;
    }

    /* Config B: gain 1.3 Gauss (1090 LSB/Gauss) - mặc định khuyến nghị */
    if (I2C_WriteRegister(i2c, hmc_slave_addr, HMC5883L_REG_CONFIG_B,
                           HMC5883L_CRB_GAIN_1_3GA) != I2C_OK) {
        return HMC_ERROR_WRITE;
    }

    /* Mode: continuous measurement mode */
    if (I2C_WriteRegister(i2c, hmc_slave_addr, HMC5883L_REG_MODE,
                           HMC5883L_MODE_CONTINUOUS) != I2C_OK) {
        return HMC_ERROR_WRITE;
    }

    return HMC_OK;
}

HMC_Status_t HMC5883L_ReadStatus(I2C_TypeDef *i2c, uint8_t *status) {
    if (status == NULL) {
        return HMC_ERROR;
    }
    if (I2C_ReadRegister(i2c, hmc_slave_addr, HMC5883L_REG_STATUS, status) != I2C_OK) {
        return HMC_ERROR_READ;
    }
    return HMC_OK;
}

HMC_Status_t HMC5883L_ReadRaw(I2C_TypeDef *i2c, int16_t *x, int16_t *y, int16_t *z) {
    uint8_t buffer[6] = {0U};

    if ((x == NULL) || (y == NULL) || (z == NULL)) {
        return HMC_ERROR;
    }

    /* Burst-read 6 byte bắt đầu từ 0x03.
     * Thứ tự trả về theo datasheet: X_MSB, X_LSB, Z_MSB, Z_LSB, Y_MSB, Y_LSB
     * (LƯU Ý: thứ tự trục là X, Z, Y - không phải X, Y, Z) */
    if (I2C_ReadRegisters(i2c, hmc_slave_addr, HMC5883L_REG_DATA_X_MSB,
                           buffer, 6U) != I2C_OK) {
        return HMC_ERROR_READ;
    }

    /* MSB đứng trước, LSB đứng sau (big-endian) - ngược với QMC5883L */
    *x = (int16_t)(((uint16_t)buffer[0U] << 8) | buffer[1U]);
    *z = (int16_t)(((uint16_t)buffer[2U] << 8) | buffer[3U]);
    *y = (int16_t)(((uint16_t)buffer[4U] << 8) | buffer[5U]);

    return HMC_OK;
}